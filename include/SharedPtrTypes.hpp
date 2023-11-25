#pragma once
#include<iostream>
#include<memory>
#include <unordered_map>
#include<vector>
#include<map>
#include<string>


template<typename T>
class BaseParser;
namespace Semantics {
    class ITypeChecker;
};
class IAinFile;
class ILexer;
class ILexerLine;
class LexerToken;
class BaseScope;
class PackageScope;
class FileScope;
class StmListScope;
class LoopScope;
class FunScope;
class ConstructorScope;
class ClassScope;
struct FunDecl;
struct VarDecl;
struct Type;
struct FunParam;
class IValue;
class Variable;
class IStatement;
class VarStm;
class IExpression;
class OperatorFunInvokeExpression;
template<typename T>
struct LinkedListNode;
template<typename T>
struct LinkedList;
struct TokensIterator;

template<typename T>
using SharedVector=std::shared_ptr<std::vector<T>>;

template<typename Key,typename Val>
using SharedMap=std::shared_ptr<std::unordered_map<Key,Val>>;

template<typename T>
using SharedLinkedListNode=std::shared_ptr<LinkedListNode<T>>;

template<typename T>
using SharedLinkedList=std::shared_ptr<LinkedList<T>>;

using SharedWString=std::shared_ptr<std::wstring>;

using SharedBool=std::shared_ptr<bool>;

using SharedILexerLine=std::shared_ptr<ILexerLine>;

using SharedLexerToken=std::shared_ptr<LexerToken>;

using SharedIAinFile=std::shared_ptr<IAinFile>;

using SharedILexer=std::shared_ptr<ILexer>;

using SharedBaseScope=std::shared_ptr<BaseScope>;

using SharedPackageScope=std::shared_ptr<PackageScope>;

using SharedFileScope=std::shared_ptr<FileScope>;

using SharedClassScope=std::shared_ptr<ClassScope>;

using SharedFunScope=std::shared_ptr<FunScope>;

using SharedLoopScope=std::shared_ptr<LoopScope>;

using SharedStmListScope=std::shared_ptr<StmListScope>;

using SharedFunDecl=std::shared_ptr<FunDecl>;

using SharedVarDecl=std::shared_ptr<VarDecl>;

using SharedType=std::shared_ptr<Type>;

using SharedFunParam=std::shared_ptr<FunParam>;

using SharedIValue=std::shared_ptr<IValue>;

using SharedVariable=std::shared_ptr<Variable>;

using SharedIStatement=std::shared_ptr<IStatement>;

using SharedVarStm=std::shared_ptr<VarStm>;

using SharedIExpression=std::shared_ptr<IExpression>;

using SharedOpFunInvokeExpression=std::shared_ptr<OperatorFunInvokeExpression>;

using SharedStmList=SharedVector<SharedIStatement>;

using SharedTokensIterator=std::shared_ptr<TokensIterator>;

template<typename T>
using SharedBaseParser=std::shared_ptr<BaseParser<T>>;

using SharedITypeChecker=std::shared_ptr<Semantics::ITypeChecker>;