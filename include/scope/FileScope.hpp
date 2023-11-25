#pragma once
#include "ASTNode.hpp"
#include "BaseScope.hpp"
#include "SharedPtrTypes.hpp"
#include <string>
class FileScope:public BaseScope{
    private:
        // map fun decl to functions
        SharedMap<std::wstring,SharedFunScope> publicFunctions;
        SharedMap<std::wstring,SharedFunScope> privateFunctions;
        
        // map classes names to classes
        SharedMap<std::wstring,SharedClassScope> publicClasses;
        SharedMap<std::wstring,SharedClassScope> privateClasses;

        // map variable names to variables
        SharedMap<std::wstring,SharedVariable> publicVariables;
        SharedMap<std::wstring,SharedVariable> privateVariables;

        SharedStmListScope globalVarsInitStmList;

    public:

        void accept(ASTVisitor *visitor) override;
        
        FileScope(std::wstring filePath,SharedPackageScope parentScope);

        SharedMap<std::wstring,SharedFunScope> getPublicFunctions() const;

        SharedMap<std::wstring,SharedFunScope> getPrivateFunctions() const;

        SharedClassScope getClassByType(SharedType type) override;

        SharedMap<std::wstring,SharedClassScope> getPublicClasses() const;

        SharedMap<std::wstring,SharedClassScope> getPrivateClasses() const;

        SharedMap<std::wstring,SharedVariable> getPublicVariables()const;

        SharedMap<std::wstring,SharedVariable> getPrivateVariables()const;

        SharedFunScope findPublicFunction(std::wstring decl);

        SharedFunScope findPrivateFunction(std::wstring decl);

        SharedClassScope findPublicClass(std::wstring name);

        SharedClassScope findPrivateClass(std::wstring name);

        SharedVariable findPublicVariable(std::wstring varName);
        
        SharedVariable findPrivateVariable(std::wstring varName);

        SharedStmListScope getGlobalVarsInitStmList()const;

        void setGlobalVarsInitStmList(SharedStmListScope globalVarsInitStmList);
};