#include "ASTVisitor.hpp"
#include "LexerLine.hpp"
#include "SharedPtrTypes.hpp"
#include "TokensIterator.hpp"
#include "ExpressionParser.hpp"
#include "Type.hpp"
#include "ainio.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "PackageScope.hpp"
#include "FileScope.hpp"
#include "ClassScope.hpp"
#include "FunScope.hpp"
#include "LoopScope.hpp"
#include "StmListScope.hpp"
#include "VarStm.hpp"
#include "IfStatement.hpp"
#include "AssignStatement.hpp"
#include "WhileStatement.hpp"
#include "DoWhileStatement.hpp"
#include "BreakStatement.hpp"
#include "ContinueStatement.hpp"
#include "ReturnStatement.hpp"
#include "ExpressionStatement.hpp"
#include "VarAccessExpression.hpp"
#include "FunInvokeExpression.hpp"
#include "NewObjectExpression.hpp"
#include "LiteralExpression.hpp"
#include "UnitExpression.hpp"
#include "LogicalExpression.hpp"
#include "NonStaticVarAccessExpression.hpp"
#include "NonStaticFunInvokeExpression.hpp"
#include "OperatorFunInvokeExpression.hpp"

namespace{
    struct FakeTypeParser:public BaseParser<SharedType>{
        int calledTimes=0;
        FakeTypeParser(SharedTokensIterator iterator)
        :BaseParser(iterator,nullptr){}
        
        SharedType parse()override{
            auto type=std::make_shared<Type>(
                std::make_shared<std::wstring>(iterator->currentVal())
            );
            iterator->next();
            calledTimes++;
            return type;
        }

    };

    class PrettyPrintVisitor:public ASTVisitor{
        std::vector<std::wstring> visitsPrints;
        public:
            std::vector<std::wstring> getPrints(){
                return visitsPrints;
            }
            void setPrints(std::vector<std::wstring> prints){
                visitsPrints=prints;
            }

            void visit(VarAccessExpression* ex) {
                setPrints({
                    L"VarAccessExpression \'"+ex->getVarName()+
                    L"\' at "+std::to_wstring(ex->getLineNumber())
                });
            }

            void visit(FunInvokeExpression* ex){
                auto prints=std::vector<std::wstring>();
                auto lineNumStr=std::to_wstring(ex->getLineNumber());
                prints.push_back(L"FunExpression \'"+ex->getFunName()+L"\' at "+lineNumStr);
                auto args=ex->getArgs();

                for(int i=0;i<args->size();i++){
                    args->at(i)->accept(this);

                    auto argPrints=getPrints();
                    
                    argPrints[0]=
                        ((i==args->size()-1)?L"└──":L"├──")+argPrints[0];

                    for(int j=1;j<argPrints.size();j++){
                        argPrints[j]=
                            ((i==args->size()-1)?L"    ":L"│   ")+argPrints[j];
                    }

                    prints.insert(prints.end(),argPrints.begin(),argPrints.end());
                }

                setPrints(prints);
            }

            void visit(NewObjectExpression* ex){
                auto prints=std::vector<std::wstring>();
                auto lineNumStr=std::to_wstring(ex->getLineNumber());
                prints.push_back(L"NewObjectExpression \'"+*ex->getReturnType()->getName()+L"\' at "+lineNumStr);
                auto args=ex->getArgs();
                for(int i=0;i<args->size();i++){

                    args->at(i)->accept(this);

                    auto argPrints=getPrints();
                    
                    argPrints[0]=
                        ((i==args->size()-1)?L"└──":L"├──")+argPrints[0];

                    for(int j=1;j<argPrints.size();j++){
                        argPrints[j]=
                            ((i==args->size()-1)?L"    ":L"│   ")+argPrints[j];
                    }

                    prints.insert(prints.end(),argPrints.begin(),argPrints.end());
                }

                setPrints(prints);
            }

            void visit(LiteralExpression* ex){
                setPrints({
                    L"LiteralExpression \'"+ex->getValue()->toString()+
                    L"\' at "+std::to_wstring(ex->getLineNumber())
                });
            }

            void visit(UnitExpression* ex){
                setPrints({L"UnitExpression at "+std::to_wstring(ex->getLineNumber())});
            }

            void visit(LogicalExpression* ex){
                auto prints=std::vector<std::wstring>();
                auto lineNumStr=std::to_wstring(ex->getLineNumber());
                
                std::wstring opStr=
                    (ex->getLogicalOp()==LogicalExpression::Operation::OR)
                    ?L"||":
                    L"&&";
                prints.push_back(L"LogicalExpression \'"+opStr+L"\' at "+lineNumStr);

                ex->getLeft()->accept(this);
                auto leftPrints=getPrints();

                leftPrints[0]=L"├──"+leftPrints[0];
                for(int j=1;j<leftPrints.size();j++){
                    leftPrints[j]=L"│   "+leftPrints[j];
                }
                prints.insert(prints.end(),leftPrints.begin(),leftPrints.end());
                
                ex->getRight()->accept(this);
                auto rightPrints=getPrints();
                rightPrints[0]=L"└──"+rightPrints[0];
                for(int j=1;j<rightPrints.size();j++){
                    rightPrints[j]=L"    "+rightPrints[j];
                }

                prints.insert(prints.end(),rightPrints.begin(),rightPrints.end());

                setPrints(prints);
            }

            void visit(NonStaticVarAccessExpression* ex){
                auto prints=std::vector<std::wstring>();
                auto lineNumStr=std::to_wstring(ex->getLineNumber());
                prints.push_back(L"NonStaticVarAccessExpression \'"+ex->getVarName()+L"\' at "+lineNumStr);

                ex->getInside()->accept(this);
                auto insidePrints=getPrints();
                
                insidePrints[0]=L"└──\033[1;33m"+insidePrints[0]+L"\033[0m";

                for(int i=1;i<insidePrints.size();i++){   
                    insidePrints[i]=L"    "+insidePrints[i];
                }

                prints.insert(prints.end(),insidePrints.begin(),insidePrints.end());

                setPrints(prints);
            }

            void visit(NonStaticFunInvokeExpression* ex){
                auto lineNumStr=std::to_wstring(ex->getLineNumber());
                prettyNonStaticFunInvokeExpression(
                    ex,
                    L"NonStaticFunInvokeExpression \'"+ex->getFunName()+L"\' at "+lineNumStr
                );
            }

            void visit(OperatorFunInvokeExpression* ex){
                auto prints=std::vector<std::wstring>();
                auto lineNumStr=std::to_wstring(ex->getLineNumber());
                prettyNonStaticFunInvokeExpression(
                    ex,
                    L"\033[36;1mOperatorFunInvokeExpression \'"+ex->getFunName()+L"\' at "+lineNumStr+L"\033[0m"
                );
            }

            void prettyNonStaticFunInvokeExpression(NonStaticFunInvokeExpression* ex,std::wstring name){
                auto prints=std::vector<std::wstring>();
                auto lineNumStr=std::to_wstring(ex->getLineNumber());
                prints.push_back(name);

                ex->getInside()->accept(this);
                auto insidePrints=getPrints();
                
                auto args=ex->getArgs();
                insidePrints[0]=
                ((args->size()==0)?L"└──":L"├──")+
                (L"\033[1;33m"+insidePrints[0]+L"\033[0m");


                for(int i=1;i<insidePrints.size();i++){   
                    insidePrints[i]=
                        ((args->size()==0)?L"    ":L"│   ")+insidePrints[i];
                }

                prints.insert(prints.end(),insidePrints.begin(),insidePrints.end());

                for(int i=0;i<args->size();i++){
                    args->at(i)->accept(this);

                    auto argPrints=getPrints();
                    
                    argPrints[0]=
                        ((i==args->size()-1)?L"└──":L"├──")+argPrints[0];

                    for(int j=1;j<argPrints.size();j++){
                        argPrints[j]=
                            ((i==args->size()-1)?L"    ":L"│   ")+argPrints[j];
                    }

                    prints.insert(prints.end(),argPrints.begin(),argPrints.end());
                }

                setPrints(prints);
            }

    };
}

int main(){

    auto scope = std::make_shared<FileScope>(L"Pretty print",PackageScope::AIN_PACKAGE);
    
    ainprint(L"Enter expression: ", true);
    std::wstring line=ainread(true);
    LexerLine lexerLine(line,1);

    lexerLine.tokenize();

    auto tokens=lexerLine.getTokens();

    auto iterator=std::make_shared<TokensIterator>(*tokens);

    auto fakeTypeParser=std::make_shared<FakeTypeParser>(iterator);

    auto fakeTypeParserProvider=[&](SharedTokensIterator,SharedBaseScope){
        return fakeTypeParser;
    };

    ExpressionParser parser(iterator,scope,fakeTypeParserProvider);

    //ainprint(std::to_wstring(fakeTypeParser->calledTimes)+L" times",true);

    auto prettyPrintVisitor=new PrettyPrintVisitor;

    parser.parse()->accept(prettyPrintVisitor);

    for(auto& print:prettyPrintVisitor->getPrints()){
        ainprint(print, true);
    }

    delete prettyPrintVisitor;

    return 0;
}