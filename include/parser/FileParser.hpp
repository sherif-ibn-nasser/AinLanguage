#pragma once
#include "BaseParser.hpp"
#include "ParserProvidersAliases.hpp"
#include "SharedPtrTypes.hpp"
class FileParser:public BaseParser<SharedFileScope>{
    private:
        std::wstring filePath;
        SharedBaseParser<SharedPackageScope> packageParser;
        FunParserProvider funParserProvider;
        ClassParserProvider classParserProvider;
        VarStmParserProvider varStmParserProvider;
        SharedPackageScope packageScope;
        SharedFileScope fileScope;
        enum class VisibilityModifier{
            NOT_PARSED,PUBLIC,PRIVATE
        };
        VisibilityModifier visibility;
        void resetVisibility();
        void parseVisibility();
        void parseFunScope();
        void parseClassScope();
        void parseVarStm();
    public:
        FileParser(
            SharedTokensIterator iterator,
            std::wstring filePath,
            SharedBaseParser<SharedPackageScope> packageParser,
            FunParserProvider funParserProvider,
            ClassParserProvider classParserProvider,
            VarStmParserProvider varStmParserProvider
        );
        SharedFileScope parse();

        std::wstring getFilePath();
        
};