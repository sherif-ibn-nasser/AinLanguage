#pragma once
#include "ASTNode.hpp"
#include "SharedPtrTypes.hpp"
#include <string>
class BaseScope:public ASTNode{
protected:
    SharedBaseScope parentScope;

    std::wstring name;
public:
    BaseScope(int lineNumber,std::wstring name,SharedBaseScope parentScope);
    SharedBaseScope getParentScope();
    std::wstring getName();
    virtual ~BaseScope();
    virtual SharedClassScope getClassByType(SharedType type);
    virtual SharedVariable getVarByName(std::wstring varName);
    virtual SharedVariable getFunByDecl(SharedFunDecl funDecl);
    static SharedLoopScope toLoopScope(SharedBaseScope scope);
    static SharedStmListScope toStmListScope(SharedBaseScope scope);
    static SharedFunScope toFunScope(SharedBaseScope scope);
    static SharedClassScope toClassScope(SharedBaseScope scope);
    static SharedFileScope toFileScope(SharedBaseScope scope);
    static SharedPackageScope toPackageScope(SharedBaseScope scope);
    static SharedLoopScope getContainingLoop(SharedBaseScope scope);
    static SharedFunScope getContainingFun(SharedBaseScope scope);
    static SharedClassScope getContainingClass(SharedBaseScope scope);
    static SharedFileScope getContainingFile(SharedBaseScope scope);
};