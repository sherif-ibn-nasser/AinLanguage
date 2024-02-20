#include "SemanticsChecksVisitor.hpp"
#include "ASTVisitor.hpp"
#include "BaseScope.hpp"
#include "CannotAccessPrivateFunctionException.hpp"
#include "CannotAccessPrivateVariableException.hpp"
#include "FunDecl.hpp"
#include "FunctionNotFoundException.hpp"
#include "InvalidOperatorFunDeclarationException.hpp"
#include "MustHaveExplicitTypeException.hpp"
#include "OperatorFunctions.hpp"
#include "SharedPtrTypes.hpp"
#include "Type.hpp"
#include "TypeMismatchException.hpp"
#include "UnexpectedTypeException.hpp"
#include "Variable.hpp"
#include "VariableNotFoundException.hpp"
#include "KeywordToken.hpp"
#include "FunParam.hpp"
#include "ArrayClassScope.hpp"
#include <cassert>
#include <string>
#include <vector>

void SemanticsChecksVisitor::visit(PackageScope* scope){
    for(auto fileIt:scope->getFiles()){
        fileIt.second->accept(this);
    }

    for(auto packageIt:scope->getPackages()){
        packageIt.second->accept(this);
    }
}

void SemanticsChecksVisitor::visit(FileScope* scope){

    scope->getGlobalVarsInitStmList()->accept(this);

    for(auto classIterator:*scope->getPrivateClasses()){
	    classIterator.second->accept(this);
    }
    for(auto classIterator:*scope->getPublicClasses()){
	    classIterator.second->accept(this);
    }
    for(auto funIterator:*scope->getPrivateFunctions()){
        funIterator.second->accept(this);
    }
    for(auto funIterator:*scope->getPublicFunctions()){
        funIterator.second->accept(this);
    }
}

void SemanticsChecksVisitor::visit(ClassScope* scope){

    if(
        scope==Type::BYTE->getClassScope().get()
        ||
        scope==Type::UBYTE->getClassScope().get()
        ||
        scope==Type::INT->getClassScope().get()
        ||
        scope==Type::UINT->getClassScope().get()
        ||
        scope==Type::LONG->getClassScope().get()
        ||
        scope==Type::ULONG->getClassScope().get()
        ||
        scope==Type::UNIT->getClassScope().get()
        ||
        scope==Type::DOUBLE->getClassScope().get()
        ||
        scope==Type::FLOAT->getClassScope().get()
        ||
        scope==Type::CHAR->getClassScope().get()
        ||
        scope==Type::BOOL->getClassScope().get()
        ||
        scope==Type::STRING->getClassScope().get()
        ||
        scope==Type::ARRAY_CLASS.get()
    )
        return;

    scope->getVarsInitStmList()->accept(this);

    for(auto constructorIterator:*scope->getPrivateConstructors()){
        constructorIterator.second->accept(this);
    }
    for(auto constructorIterator:*scope->getPublicConstructors()){
        constructorIterator.second->accept(this);
    }
    for(auto funIterator:*scope->getPrivateFunctions()){
        funIterator.second->accept(this);
    }
    for(auto funIterator:*scope->getPublicFunctions()){
        funIterator.second->accept(this);
    }
}

void SemanticsChecksVisitor::visit(FunScope* scope){
    if(*scope->getDecl()->isOperator)
        doOperatorFunChecks(scope);

    doStmListScopeChecks(scope);
}

void SemanticsChecksVisitor::visit(LoopScope* scope){ 
    doStmListScopeChecks(scope);
}

void SemanticsChecksVisitor::visit(StmListScope* scope){
    doStmListScopeChecks(scope);
}

void SemanticsChecksVisitor::visit(VarStm* stm){

    initStmRunScope(stm);

    auto var=stm->getVar();

    auto ex=stm->getEx();

    ex->accept(this);

    auto exType=ex->getReturnType();

    if(var->hasImplicitType()){
        var->setType(exType);
        return;
    }

    if(*var->getType()==*exType)
        return;
    
    // TODO: message
    throw TypeMismatchException(
        L"متغير "+*var->getName()+L": "+*var->getType()->getName(),
        L"تعبير: "+*ex->getReturnType()->getName());

}

void SemanticsChecksVisitor::visit(AssignStatement* stm){

    initStmRunScope(stm);

    auto left=stm->getLeft();

    auto right=stm->getRight();

    left->accept(this);

    right->accept(this);

    auto t1=left->getReturnType();

    auto t2=right->getReturnType();

    if(*t1!=*t2)
        throw UnexpectedTypeException(
            stm->getLineNumber(),
            *t1->getName(),
            *t2->getName()
        );
}

void SemanticsChecksVisitor::visit(AugmentedAssignStatement* stm){

    initStmRunScope(stm);

    auto lineNumber=stm->getLineNumber();

    auto left=stm->getLeft();

    auto right=stm->getRight();

    left->accept(this);

    right->accept(this);

    auto lType=left->getReturnType();

    auto exType=right->getReturnType();

    std::wstring funName=getAugmentedAssignOpFunName(stm->getOp());

    auto fun=findOpFunInType(
        lType,
        funName,
        {exType},
        lineNumber
    );

    auto rType=fun->getReturnType();

    if(*lType!=*rType)
        throw UnexpectedTypeException(
            stm->getLineNumber(),
            *lType->getName(),
            *rType->getName()
        );

    stm->setOpFun(fun);
}

void SemanticsChecksVisitor::visit(IfStatement* stm){

    initStmRunScope(stm);

    auto ifCondition=stm->getIfCondition();

    ifCondition->accept(this);

    if(*ifCondition->getReturnType()!=*Type::BOOL)
        throw UnexpectedTypeException(
            stm->getLineNumber(),
            *Type::BOOL_NAME,
            *ifCondition->getReturnType()->getName()
        );
    
    stm->getIfScope()->accept(this);

    if(auto elseScope=stm->getElseScope())
        elseScope->accept(this);
}

void SemanticsChecksVisitor::visit(WhileStatement* stm){
    doWhileStmChecks(stm);
}

void SemanticsChecksVisitor::visit(DoWhileStatement* stm){
    doWhileStmChecks(stm);
}

void SemanticsChecksVisitor::visit(ReturnStatement* stm){

    initStmRunScope(stm);

    auto ex=stm->getEx();

    ex->accept(this);

    auto exType=ex->getReturnType();

    auto funScope=BaseScope::getContainingFun(stm->getRunScope());
    
    // Return statements in constructors should return Unit
    if(funScope->getDecl()->isConstructor()){
        if(*exType!=*Type::UNIT)
            throw UnexpectedTypeException(
                stm->getLineNumber(),
                *Type::UNIT_NAME,
                *exType->getName()
            );
    }
    else if(*funScope->getReturnType()!=*exType)
        throw UnexpectedTypeException(
            stm->getLineNumber(),
            *funScope->getReturnType()->getName(),
            *exType->getName()
        );
}

void SemanticsChecksVisitor::visit(ExpressionStatement* stm){
    initStmRunScope(stm);

    stm->getEx()->accept(this);
}

void SemanticsChecksVisitor::visit(VarAccessExpression* ex){

    auto lineNumber=ex->getLineNumber();

    auto setVar=[=](SharedVariable newVar){
        if(!newVar)
            return false;
        if(!newVar->getType())
            throw MustHaveExplicitTypeException(lineNumber);
        ex->setVar(newVar);
        ex->setReturnType(newVar->getType());
        return true;
    };

    // To enable implicit var type checks when throwing MustHaveExplicitTypeException
    if(auto var=ex->getVar()){
        auto varType=var->getType();
        if(!varType)
            throw MustHaveExplicitTypeException(lineNumber);
        ex->setReturnType(varType);
        return;
    }

    auto varName=ex->getVarName();

    auto parentStmListScope=BaseScope::toStmListScope(checkScope);

    if(parentStmListScope){

        auto containingClassScope=BaseScope::getContainingClass(checkScope);
        if(
            containingClassScope
            &&
            (
                setVar(containingClassScope->findPublicVariable(varName))
                ||
                setVar(containingClassScope->findPrivateVariable(varName))
            )
        )
            return;
        
    }

    auto containingFileScope=BaseScope::getContainingFile(checkScope);
    
    if(parentStmListScope||BaseScope::toClassScope(checkScope)){
        
        if(setVar(containingFileScope->findPrivateVariable(varName)))
            return;
        
        auto package=BaseScope::toPackageScope(containingFileScope->getParentScope());
        for(auto fileIt:package->getFiles()){
            if(setVar(fileIt.second->findPublicVariable(varName)))
                return;
        }
    }
    
    // TODO: make trace more readable
    auto trace=
        containingFileScope->getName()+
        L"::"+checkScope->getName()+L"("+std::to_wstring(lineNumber)+L")";
    
    throw VariableNotFoundException(trace,varName);
}

void SemanticsChecksVisitor::visit(FunInvokeExpression* ex){
    auto params=std::make_shared<std::vector<SharedFunParam>>();
    auto args=ex->getArgs();
    auto funName=ex->getFunName();
    auto lineNumber=ex->getLineNumber();

    for(auto arg:*args){
        arg->accept(this);
        auto argType=arg->getReturnType();
        params->push_back(
            std::make_shared<FunParam>(nullptr,argType)
        );
    }
    auto decl=FunDecl(
        std::make_shared<std::wstring>(funName),
        nullptr,
        std::make_shared<bool>(false),
        params
    ).toString();

    auto setFun=[=](SharedFunScope fun){
        if(!fun)
            return false;
        if(!fun->getReturnType())
            throw MustHaveExplicitTypeException(lineNumber);
        ex->setFun(fun);
        ex->setReturnType(fun->getReturnType());
        return true;
    };

    auto containingClassScope=BaseScope::getContainingClass(checkScope);
    if(
        containingClassScope
        &&
        (
            setFun(containingClassScope->findPublicFunction(decl))
            ||
            setFun(containingClassScope->findPrivateFunction(decl))
        )
    )
        return;

    auto containingFileScope=BaseScope::getContainingFile(checkScope);
    if(
        containingFileScope
        &&
        setFun(containingFileScope->findPrivateFunction(decl))
    )
        return;
    
    auto package=BaseScope::toPackageScope(containingFileScope->getParentScope());
    for(auto fileIt:package->getFiles()){
        if(setFun(fileIt.second->findPublicFunction(decl)))
            return;
    }
        
    // TODO: make trace more readable
    auto trace=
        containingFileScope->getName()+
        L"::"+checkScope->getName()+L"("+std::to_wstring(lineNumber)+L")";
    
    throw FunctionNotFoundException(trace,decl);
    
}

void SemanticsChecksVisitor::visit(NewObjectExpression* ex){
    auto params=std::make_shared<std::vector<SharedFunParam>>();
    auto args=ex->getArgs();
    auto type=ex->getReturnType();
    auto returnClassScope=type->getClassScope();

    for(auto arg:*args){
        arg->accept(this);
        auto argType=arg->getReturnType();
        params->push_back(
            std::make_shared<FunParam>(nullptr,argType)
        );
    }
    auto decl=FunDecl(
        std::make_shared<std::wstring>(KeywordToken::NEW.getVal()),
        type,
        std::make_shared<bool>(false),
        params
    ).toString();
    
    auto containingClassScope=BaseScope::getContainingClass(checkScope);
    if(containingClassScope==returnClassScope){
        auto privateConstructor=returnClassScope->findPrivateConstructor(decl);
        if(privateConstructor){
            ex->setConstructor(privateConstructor);
            return;
        }
    }

    // TODO: check for protected constructor if parent class of checkScope is child of object class scope

    // TODO: check for protected constructors

    auto publicConstructor=returnClassScope->findPublicConstructor(decl);
    if(publicConstructor){
        ex->setConstructor(publicConstructor);
        return;
    }
    
    auto trace=
        BaseScope::getContainingFile(checkScope)->getName()+
        L"::"+checkScope->getName()+L"("+std::to_wstring(ex->getLineNumber())+L")";

    throw FunctionNotFoundException(
        trace,
        decl+L" للنوع "+AinException::betweenAngleBrackets(*type->getName())
    );

}

void SemanticsChecksVisitor::visit(NewArrayExpression* ex){
    for(auto cap:ex->getCapacities()){
        cap->accept(this);
        auto capType=cap->getReturnType();
        if(capType->getClassScope()!=Type::INT->getClassScope())
            throw UnexpectedTypeException(
                cap->getLineNumber(),
                *Type::INT_NAME,
                *capType->getName()
            );
    }
}

void SemanticsChecksVisitor::visit(LogicalExpression* ex){
    auto left=ex->getLeft();
    auto right=ex->getRight();
    auto lineNumber=ex->getLineNumber();

    left->accept(this);
    right->accept(this);

    if(left->getReturnType()->getClassScope()!=Type::BOOL->getClassScope())
        throw UnexpectedTypeException(
            lineNumber,
            *Type::BOOL_NAME,
            *left->getReturnType()->getName()
        );
    
    if(right->getReturnType()->getClassScope()!=Type::BOOL->getClassScope())
        throw UnexpectedTypeException(
            lineNumber,
            *Type::BOOL_NAME,
            *right->getReturnType()->getName()
        );
}

void SemanticsChecksVisitor::visit(NonStaticVarAccessExpression* ex){
    auto lineNumber=ex->getLineNumber();
    auto varName=ex->getVarName();
    auto inside=ex->getInside();
    inside->accept(this);
    auto insideType=inside->getReturnType();
    auto insideClassScope=insideType->getClassScope();

    auto publicVar=insideClassScope->findPublicVariable(varName);

    if(publicVar){
        if(!publicVar->getType())
            throw MustHaveExplicitTypeException(lineNumber);
        ex->setVar(publicVar);
        ex->setReturnType(publicVar->getType());
        return;
    }

    auto privateVar=insideClassScope->findPrivateVariable(varName);

    // TODO: make trace more readable
    auto trace=
        BaseScope::getContainingFile(checkScope)->getName()+
        L"::"+checkScope->getName()+L"("+std::to_wstring(lineNumber)+L")";

    if(!privateVar)
        throw VariableNotFoundException(trace,insideClassScope->getName()+L"::"+varName);
    
    if(insideClassScope!=BaseScope::getContainingClass(checkScope))
        throw CannotAccessPrivateVariableException(trace,varName);

    if(!privateVar->getType())
        throw MustHaveExplicitTypeException(lineNumber);

    ex->setVar(privateVar);

    ex->setReturnType(privateVar->getType());
}

void SemanticsChecksVisitor::visit(NonStaticFunInvokeExpression* ex){
    auto lineNumber=ex->getLineNumber();

    auto paramsTypes=std::vector<SharedType>();
    for(auto arg:*ex->getArgs()){
        arg->accept(this);
        paramsTypes.push_back(arg->getReturnType());
    }

    auto inside=ex->getInside();
    inside->accept(this);
    auto insideType=inside->getReturnType();
    
    auto fun=findFunInType(
        insideType,
        ex->getFunName(),
        paramsTypes,
        lineNumber
    );

    ex->setFun(fun);

    ex->setReturnType(fun->getReturnType());
}

void SemanticsChecksVisitor::visit(OperatorFunInvokeExpression* ex){

    auto lineNumber=ex->getLineNumber();

    auto paramsTypes=std::vector<SharedType>();
    for(auto arg:*ex->getArgs()){
        arg->accept(this);
        paramsTypes.push_back(arg->getReturnType());
    }

    auto inside=ex->getInside();
    inside->accept(this);
    auto insideType=inside->getReturnType();
    
    auto fun=findOpFunInType(
        insideType,
        ex->getFunName(),
        paramsTypes,
        lineNumber
    );

    ex->setFun(fun);
    
    switch(ex->getOp()){
        case OperatorFunInvokeExpression::Operator::LESS:
        case OperatorFunInvokeExpression::Operator::LESS_EQUAL:
        case OperatorFunInvokeExpression::Operator::GREATER:
        case OperatorFunInvokeExpression::Operator::GREATER_EQUAL:
            ex->setReturnType(Type::BOOL);
            break;
        default:
            ex->setReturnType(fun->getReturnType());
            break;
    }
}

void SemanticsChecksVisitor::visit(SetOperatorExpression* ex){
    
    auto lineNumber=ex->getLineNumber();

    auto exOfGet=ex->getExOfGet();

    exOfGet->accept(this);

    auto typeOfGetEx=exOfGet->getReturnType();

    std::vector<SharedType> paramsTypes;

    // When operator is not inc or dec operators
    if(auto exOfValue=ex->getValueEx()){
        exOfValue->accept(this);
        auto exType=exOfValue->getReturnType();
        paramsTypes.push_back(exType);
    }
    
    std::wstring opFunName=getOpFunNameOfSetOp(ex->getOp());

    auto opFun=findOpFunInType(
        typeOfGetEx,
        opFunName,
        paramsTypes,
        lineNumber
    );

    ex->setFunOfOp(opFun);

    auto typeOfValueToSet=opFun->getReturnType();

    auto funOfSet=findOpFunInType(
        ex->getExHasGetOp()->getReturnType(),
        OperatorFunctions::SET_NAME,
        {
            ex->getIndexEx()->getReturnType(),
            typeOfValueToSet
        },
        lineNumber
    );
    
    ex->setFunOfSet(funOfSet);

    switch(ex->getOp()){
        case SetOperatorExpression::Operator::PRE_INC:
        case SetOperatorExpression::Operator::PRE_DEC:
        case SetOperatorExpression::Operator::POST_INC:
        case SetOperatorExpression::Operator::POST_DEC:
            ex->setReturnType(ex->getExOfGet()->getReturnType());
            break;
        default:
            ex->setReturnType(funOfSet->getReturnType());
    }
    
}

void SemanticsChecksVisitor::visit(ThisVarAccessExpression* ex){
    auto lineNumber=ex->getLineNumber();
    auto varName=ex->getVarName();
    auto classScope=ex->getClassScope();
    
    auto publicVar=classScope->findPublicVariable(varName);

    if(publicVar){
        if(!publicVar->getType())
            throw MustHaveExplicitTypeException(lineNumber);
        ex->setVar(publicVar);
        ex->setReturnType(publicVar->getType());
        return;
    }

    auto privateVar=classScope->findPrivateVariable(varName);

    // TODO: make trace more readable
    auto trace=
        BaseScope::getContainingFile(checkScope)->getName()+
        L"::"+checkScope->getName()+L"("+std::to_wstring(lineNumber)+L")";

    if(!privateVar)
        throw VariableNotFoundException(trace,classScope->getName()+L"::"+varName);
    
    if(!privateVar->getType())
        throw MustHaveExplicitTypeException(lineNumber);

    ex->setVar(privateVar);

    ex->setReturnType(privateVar->getType());
}

void SemanticsChecksVisitor::visit(ThisFunInvokeExpression* ex){
    auto lineNumber=ex->getLineNumber();
    auto funName=ex->getFunName();
    auto classScope=ex->getClassScope();

    auto params=std::make_shared<std::vector<SharedFunParam>>();
    
    for(auto arg:*ex->getArgs()){
        arg->accept(this);
        params->push_back(
            std::make_shared<FunParam>(nullptr,arg->getReturnType())
        );
    }
    
    auto decl=FunDecl(
        std::make_shared<std::wstring>(funName),
        nullptr,
        std::make_shared<bool>(true),
        params
    ).toString();

    SharedFunScope fun=classScope->findPublicFunction(decl);
    // TODO: make trace more readable
    auto trace=
        BaseScope::getContainingFile(checkScope)->getName()+
        L"::"+checkScope->getName()+L"("+std::to_wstring(lineNumber)+L")";

    if(!fun)
        fun=classScope->findPrivateFunction(decl);

    if(!fun)
        throw FunctionNotFoundException(trace,classScope->getName()+L"::"+decl);
    
    ex->setFun(fun);

    ex->setReturnType(fun->getReturnType());
}

void SemanticsChecksVisitor::initStmRunScope(IStatement* stm){
    checkScope=stm->getRunScope();
}

void SemanticsChecksVisitor::doWhileStmChecks(WhileStatement* stm){

    initStmRunScope(stm);

    auto condition=stm->getCondition();

    condition->accept(this);

    if(*condition->getReturnType()!=*Type::BOOL)
        throw UnexpectedTypeException(
            stm->getLineNumber(),
            *Type::BOOL_NAME,
            *condition->getReturnType()->getName()
        );
    
    stm->getLoopScope()->accept(this);
}

void SemanticsChecksVisitor::doStmListScopeChecks(StmListScope* scope){
    for(auto stm:*scope->getStmList()){
        stm->accept(this);
    }
}

void SemanticsChecksVisitor::doOperatorFunChecks(FunScope* scope){
    checkOperatorFunParamsSize(scope);
    checkOperatorFunReturnType(scope);
}

void SemanticsChecksVisitor::checkOperatorFunParamsSize(FunScope* scope){

    auto opName=scope->getName();
    auto paramsSize=scope->getDecl()->params->size();

    auto isBinOp=
        opName==OperatorFunctions::PLUS_NAME
        ||
        opName==OperatorFunctions::MINUS_NAME
        ||
        opName==OperatorFunctions::TIMES_NAME
        ||
        opName==OperatorFunctions::DIV_NAME
        ||
        opName==OperatorFunctions::MOD_NAME
        ||
        opName==OperatorFunctions::POW_NAME
        ||
        opName==OperatorFunctions::COMPARE_TO_NAME
        ||
        opName==OperatorFunctions::EQUALS_NAME
        ||
        opName==OperatorFunctions::SHL_NAME
        ||
        opName==OperatorFunctions::SHR_NAME
        ||
        opName==OperatorFunctions::BIT_AND_NAME
        ||
        opName==OperatorFunctions::XOR_NAME
        ||
        opName==OperatorFunctions::BIT_OR_NAME
        ||
        opName==OperatorFunctions::GET_NAME
    ;

    if(isBinOp&&paramsSize!=1)
        throw InvalidOperatorFunDeclarationException(L"عدد المعاملات غير صالح لدالة "+opName);

    auto isUnaryOp=
        opName==OperatorFunctions::UNARY_PLUS_NAME
        ||
        opName==OperatorFunctions::UNARY_MINUS_NAME
        ||
        opName==OperatorFunctions::LOGICAL_NOT_NAME
        ||
        opName==OperatorFunctions::BIT_NOT_NAME
        ||
        opName==OperatorFunctions::INC_NAME
        ||
        opName==OperatorFunctions::DEC_NAME
    ;

    if(isUnaryOp&&paramsSize!=0)
        throw InvalidOperatorFunDeclarationException(L"عدد المعاملات غير صالح لدالة "+opName);

    if(opName==OperatorFunctions::SET_NAME&&paramsSize<2)
        throw InvalidOperatorFunDeclarationException(L"عدد المعاملات غير صالح لدالة "+opName);

}

void SemanticsChecksVisitor::checkOperatorFunReturnType(FunScope* scope){
    auto opName=scope->getName();

    auto returnType=scope->getReturnType();

    if(opName==OperatorFunctions::COMPARE_TO_NAME&&returnType->getClassScope()!=Type::INT->getClassScope())
        throw InvalidOperatorFunDeclarationException(L"دالة قارن_مع يجب أن ترجع قيمة من نوع صحيح.");

    if(opName==OperatorFunctions::EQUALS_NAME&&returnType->getClassScope()!=Type::BOOL->getClassScope())
        throw InvalidOperatorFunDeclarationException(L"دالة يساوي يجب أن ترجع قيمة من نوع منطقي.");
    
    auto parentClass=BaseScope::getContainingClass(scope->getParentScope());
    if(
        (opName==OperatorFunctions::INC_NAME||opName==OperatorFunctions::DEC_NAME)
        &&
        returnType->getClassScope()!=parentClass
    )
        throw InvalidOperatorFunDeclarationException(
            L"دالة "+opName+
            L" يجب أن ترجع قيمة من نفس نوع "+parentClass->getName()
        );
    
}

std::wstring SemanticsChecksVisitor::getAugmentedAssignOpFunName(
    AugmentedAssignStatement::Operator op
){
    switch(op){
        case AugmentedAssignStatement::Operator::PLUS:
            return OperatorFunctions::PLUS_NAME;
        case AugmentedAssignStatement::Operator::MINUS:
            return OperatorFunctions::MINUS_NAME;
        case AugmentedAssignStatement::Operator::TIMES:
            return OperatorFunctions::TIMES_NAME;
        case AugmentedAssignStatement::Operator::DIV:
            return OperatorFunctions::DIV_NAME;
        case AugmentedAssignStatement::Operator::MOD:
            return OperatorFunctions::MOD_NAME;
        case AugmentedAssignStatement::Operator::POW:
            return OperatorFunctions::POW_NAME;
        case AugmentedAssignStatement::Operator::SHR:
            return OperatorFunctions::SHR_NAME;
        case AugmentedAssignStatement::Operator::SHL:
            return OperatorFunctions::SHL_NAME;
        case AugmentedAssignStatement::Operator::BIT_AND:
            return OperatorFunctions::BIT_AND_NAME;
        case AugmentedAssignStatement::Operator::XOR:
            return OperatorFunctions::XOR_NAME;
        case AugmentedAssignStatement::Operator::BIT_OR:
            return OperatorFunctions::BIT_OR_NAME;
        default:{}
    }

    assert(false); // Unreachable
}

std::wstring SemanticsChecksVisitor::getOpFunNameOfSetOp(SetOperatorExpression::Operator op) {
    switch(op){
        case SetOperatorExpression::Operator::PLUS_EQUAL:
            return OperatorFunctions::PLUS_NAME;
        case SetOperatorExpression::Operator::MINUS_EQUAL:
            return OperatorFunctions::MINUS_NAME;
        case SetOperatorExpression::Operator::TIMES_EQUAL:
            return OperatorFunctions::TIMES_NAME;
        case SetOperatorExpression::Operator::DIV_EQUAL:
            return OperatorFunctions::DIV_NAME;
        case SetOperatorExpression::Operator::MOD_EQUAL:
            return OperatorFunctions::MOD_NAME;
        case SetOperatorExpression::Operator::POW_EQUAL:
            return OperatorFunctions::POW_NAME;
        case SetOperatorExpression::Operator::SHR_EQUAL:
            return OperatorFunctions::SHR_NAME;
        case SetOperatorExpression::Operator::SHL_EQUAL:
            return OperatorFunctions::SHL_NAME;
        case SetOperatorExpression::Operator::BIT_AND_EQUAL:
            return OperatorFunctions::BIT_AND_NAME;
        case SetOperatorExpression::Operator::XOR_EQUAL:
            return OperatorFunctions::XOR_NAME;
        case SetOperatorExpression::Operator::BIT_OR_EQUAL:
            return OperatorFunctions::BIT_OR_NAME;
        case SetOperatorExpression::Operator::PRE_INC:
        case SetOperatorExpression::Operator::POST_INC:
            return OperatorFunctions::INC_NAME;
        case SetOperatorExpression::Operator::PRE_DEC:
        case SetOperatorExpression::Operator::POST_DEC:
            return OperatorFunctions::DEC_NAME;
        default:
            return L"";
    }
}

SharedFunScope SemanticsChecksVisitor::findFunInType(
    SharedType type,
    std::wstring funName,
    std::vector<SharedType> paramsTypes,
    int traceLineNumber
){
    auto params=std::make_shared<std::vector<SharedFunParam>>();
    for(auto paramType:paramsTypes){
        params->push_back(
            std::make_shared<FunParam>(nullptr,paramType)
        );
    }
    auto decl=FunDecl(
        std::make_shared<std::wstring>(funName),
        nullptr,
        std::make_shared<bool>(true),
        params
    ).toString();

    SharedFunScope fun=type->getClassScope()->findPublicFunction(decl);
    // TODO: make trace more readable
    auto trace=
        BaseScope::getContainingFile(checkScope)->getName()+
        L"::"+checkScope->getName()+L"("+std::to_wstring(traceLineNumber)+L")";

    if(!fun){
        fun=type->getClassScope()->findPrivateFunction(decl);

        if(!fun)
            throw FunctionNotFoundException(trace,*type->getName()+L"::"+decl);

        if(type->getClassScope()!=BaseScope::getContainingClass(checkScope))
            throw CannotAccessPrivateFunctionException(trace,decl);
    }

    return fun;
}

SharedFunScope SemanticsChecksVisitor::findOpFunInType(
    SharedType type,
    std::wstring funName,
    std::vector<SharedType> paramsTypes,
    int traceLineNumber
){
    auto fun=findFunInType(
        type,
        funName,
        paramsTypes,
        traceLineNumber
    );

    if(*fun->getDecl()->isOperator)
        return fun;

    // TODO: make trace more readable
    auto trace=
        BaseScope::getContainingFile(checkScope)->getName()+
        L"::"+checkScope->getName()+L"("+std::to_wstring(traceLineNumber)+L")";

    // append operator to decl, so it says "operator function ... not found "
    throw FunctionNotFoundException(trace,L"مؤثر "+fun->getDecl()->toString());

}