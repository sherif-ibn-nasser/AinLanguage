#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include "AinException.hpp"
#include "BuiltInFunScope.hpp"
#include "ClassParser.hpp"
#include "Compiler.hpp"
#include "CompilerVarsOffsetSetter.hpp"
#include "ExpressionParser.hpp"
#include "FileParser.hpp"
#include "FunDeclParser.hpp"
#include "FunParamParser.hpp"
#include "FunParser.hpp"
#include "PackageParser.hpp"
#include "PackageScope.hpp"
#include "ParserProvidersAliases.hpp"
#include "SemanticsChecksVisitor.hpp"
#include "SharedPtrTypes.hpp"
#include "AinFile.hpp"
#include "Lexer.hpp"
#include "KeywordToken.hpp"
#include "StmListParser.hpp"
#include "SymbolToken.hpp"
#include "TokensIterator.hpp"
#include "TypeChecker.hpp"
#include "ImplicitVarTypeChecker.hpp"
#include "TypeParser.hpp"
#include "VarDeclParser.hpp"
#include "VarStatementParser.hpp"
#include "PackageScope.hpp"
#include "FileScope.hpp"
#include "FunScope.hpp"
#include "Type.hpp"
#include "Interpreter.hpp"
#include "VarsOffsetSetter.hpp"
#include "ainio.hpp"
#include "string_helper.hpp"
#include "BuiltInFilePaths.hpp"

auto typeParserProvider=[](SharedTokensIterator iterator,SharedBaseScope scope){
    return std::make_shared<TypeParser>(
        iterator,scope,Semantics::TypeChecker::getInstance()
    );
};

auto funParamParserProvider=[](
    SharedTokensIterator iterator,SharedBaseScope scope,TypeParserProvider typeParserProvider
){
    return std::make_shared<FunParamParser>(
        iterator,scope,typeParserProvider
    );
};

auto varDeclParserProvider=[](SharedTokensIterator iterator,SharedBaseScope scope){
    return std::make_shared<VarDeclParser>(
        iterator,scope,typeParserProvider
    );
};

auto expressionParserProvider=[](SharedTokensIterator iterator,SharedBaseScope scope){
    return std::make_shared<ExpressionParser>(
        iterator,scope,typeParserProvider
    );
};

auto varStmParserProvider=[](SharedTokensIterator iterator,SharedBaseScope scope){
    return std::make_shared<VarStatementParser>(
        iterator,scope,varDeclParserProvider,expressionParserProvider
    );
};

auto stmListParserProvider=[](SharedTokensIterator iterator,SharedStmListScope scope){
    return std::make_shared<StmListParser>(
        iterator,scope,varStmParserProvider,expressionParserProvider
    );
};

auto funDeclParserProvider=[](SharedTokensIterator iterator,SharedBaseScope scope){
    return std::make_shared<FunDeclParser>(
        iterator,scope,typeParserProvider,funParamParserProvider
    );
};

auto funParserProvider=[](SharedTokensIterator iterator,SharedBaseScope scope){
    return std::make_shared<FunParser>(
        iterator,scope,funDeclParserProvider,stmListParserProvider
    );
};

auto classParserProvider=[](SharedTokensIterator iterator,SharedBaseScope scope){
    return std::make_shared<ClassParser>(
        iterator,
        scope,
        stmListParserProvider,
        typeParserProvider,
        funParamParserProvider,
        funParserProvider,
        varStmParserProvider
    );
};

void readAndParse(std::string path){
    auto file=std::make_shared<AinFile>(path);
    auto lexer=std::make_shared<Lexer>(file);
    auto tokens=lexer->getTokens();
    auto iterator=std::make_shared<TokensIterator>(*tokens);
    auto packageParser=std::make_shared<PackageParser>(iterator,PackageScope::AIN_PACKAGE);
    auto wpath=toWstring(path);
    auto fileScope=
        FileParser(
            iterator,
            wpath,
            packageParser,
            funParserProvider,
            classParserProvider,
            varStmParserProvider
        ).parse();

    Type::addBuiltInClassesTo(fileScope);
    BuiltInFunScope::addBuiltInFunctionsTo(fileScope);

}

bool isMainFileOption(std::string o){
    return o=="-m" || o=="--main";
}

std::string removeExtension(std::string fileName){
    return fileName.substr(0, fileName.find_last_of("."));
}

int main(int argc, char * argv[]){

    // TODO: show info about ain and available options
    if(argc < 2){ // 2 as it includes the program name
        std::__throw_out_of_range("You should pass at least one ain file path.");
        return -1;
    }

    auto filesStack=std::vector<std::string>();

    // to make sure that -m or --main is used only once 
    auto mainOptionUsed=false;
    for(int i=1;i<argc;i++){

        if(!isMainFileOption(argv[i])){
            filesStack.push_back(argv[i]);
            continue;
        }

        if(mainOptionUsed)
            throw std::invalid_argument("\033[1;31mأمر -m أو --main يجب أن يُستخدم مرة واحدة فقط.\033[0m");
        
        mainOptionUsed=true;

        if(++i==argc)
            throw std::invalid_argument("يُتوقّع ملف بعد الأمر "+std::string(argv[i-1]));
        
        auto temp=filesStack[0];
        filesStack[0]=argv[i];
        filesStack.push_back(temp);
    }


    try{

        // TODO: Make them as ainstd lib

        if(auto ainStdPath=std::getenv("AIN_STD")){
            for(const auto &entry:std::filesystem::recursive_directory_iterator(ainStdPath)){
                if(!std::filesystem::is_directory(entry))
                    filesStack.push_back(entry.path());
            }
        }
        else
            throw AinException(
                AinException::errorWString(
                    L"لم يتم العثور على مسار AIN_STD في متغيرات النظام،\nقم بزيارة https://gitlab.com/sherifnasser/AinLanguage للتعرف على كيفية إضافتها في نظامك."
                )
            );
        
        // parse in reverse and make the main file at the end
        for(int i=filesStack.size()-1;i>=0;i--){
            readAndParse(filesStack[i]);
        }

        BuiltInFunScope::addBuiltInFunctionsToBuiltInClasses();

        Semantics::TypeChecker::getInstance()->check();

        auto checker=new SemanticsChecksVisitor;

        Semantics::ImplicitVarTypeChecker::getInstance()->check(checker);

        PackageScope::AIN_PACKAGE->accept(checker);
        
        delete checker;
        /*
        auto interpreter=new Interpreter;
        auto lAssigner=new Interpreter::LeftSideAssigner(interpreter);
        auto rAssigner=new Interpreter::RightSideAssigner(interpreter);
        
        auto varsOffsetSetter=new VarsOffsetSetter(
            &interpreter->offsets,
            interpreter->BP,
            interpreter->BX,
            interpreter->DS
        );

        PackageScope::AIN_PACKAGE->accept(varsOffsetSetter);

        delete varsOffsetSetter;

        interpreter->lAssigner=lAssigner;
        interpreter->rAssigner=rAssigner;

        */

        auto compiler=new Compiler;
        auto compilerVarsOffsetSetter=new CompilerVarsOffsetSetter(&compiler->offsets);

        PackageScope::AIN_PACKAGE->accept(compilerVarsOffsetSetter);

        delete compilerVarsOffsetSetter;
        
        auto main=PackageScope::AIN_PACKAGE->
            findFileByPath(toWstring(filesStack[0]))->
            findPublicFunction(L"البداية()");

        main->accept(compiler);

        auto generatedAsm=compiler->getAssemblyFile();

        // ainprint(generatedAsm, false);

        delete compiler;

        auto outputBinName=removeExtension(filesStack[0]);
        auto outputObjFileName=outputBinName+".o";
        auto outputAsmFileName=outputBinName+".asm";

        std::ofstream outputAsmFile(outputAsmFileName);
        outputAsmFile<<toCharPointer(generatedAsm);
        outputAsmFile.close();

        auto command=
            "nasm -felf64 -o "+outputObjFileName+" -gdwarf "+outputAsmFileName
            +"; ld "+outputObjFileName+" -o "+outputBinName
            +"; rm "+outputObjFileName+" "+outputAsmFileName;
        system(command.c_str());
        
        /*
        PackageScope::AIN_PACKAGE->accept(interpreter); // To init global vars
        main->accept(interpreter);

        delete lAssigner;
        delete rAssigner;
        delete interpreter;
        */

    }
    catch(std::exception& e){
        std::cout<<e.what()<<std::endl;
    }
    

    return 0;
}