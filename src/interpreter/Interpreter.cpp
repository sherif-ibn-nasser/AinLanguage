#include "BuiltInFunScope.hpp"
#include "ArrayNegativeCapacityException.hpp"
#include "BoolValue.hpp"
#include "ArrayClassScope.hpp"
#include "CharValue.hpp"
#include "FunDecl.hpp"
#include "FunScope.hpp"
#include "IValue.hpp"
#include "IntValue.hpp"
#include "OperatorFunInvokeExpression.hpp"
#include "OutOfMemoryException.hpp"
#include "RefValue.hpp"
#include "SetOperatorExpression.hpp"
#include "SharedPtrTypes.hpp"
#include "StackOverFlowException.hpp"
#include "StackUnderFlowException.hpp"
#include "Type.hpp"
#include "VoidValue.hpp"
#include "Variable.hpp"
#include "FunParam.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

void Interpreter::push(SharedIValue val){
    if(--*SP<=*SS-1)
        throw StackOverFlowException();

    memory[*SP]=val;
}

SharedIValue Interpreter::top(){
    if(*SP>=MEM_SIZE)
        throw StackUnderFlowException();
    
    return memory[*SP];
}

SharedIValue Interpreter::pop(){
    if(*SP>=MEM_SIZE)
        throw StackUnderFlowException();
    auto val=memory[*SP];
    memory[(*SP)++]=0;
    return val;
}

void Interpreter::runStmList(StmListScope* scope){
    for(auto stm:*scope->getStmList()){
        stm->accept(this);
        if(funReturn||loopBreak||loopContinue)
            break;
    }
}

void Interpreter::reserveSpaceForStmListLocals(int size){
    *SP-=size;
}

void Interpreter::removeReservedSpaceForStmListLocals(int size){
    *SP+=size;
}

void Interpreter::invokeNonStaticFun(NonStaticFunInvokeExpression* ex){
    push(std::make_shared<RefValue>(*BX));

    ex->getInside()->accept(this);

    *BX=popAs<RefValue>()->getAddress();

    auto args=ex->getArgs();

    for(auto arg:*args){
        arg->accept(this);
    }

    ex->getFun()->accept(this);

    for(auto arg:*args){
        pop();
    }

    *BX=popAs<RefValue>()->getAddress();

    push(AX); // return value
}

void Interpreter::invokeNonStaticBuiltInFun(NonStaticFunInvokeExpression* ex){
    ex->getInside()->accept(this);

    auto args=ex->getArgs();

    for(auto arg:*args){
        arg->accept(this);
    }

    switch(args->size()){
        case 2:
            DX=pop();
        case 1:
            CX=pop();
        case 0:
        default:
            AX=pop();
    }

    ex->getFun()->accept(this);

    push(AX); // return value
}

void Interpreter::invokeBuiltInOpFun(OperatorFunInvokeExpression* ex){
    
    auto op=ex->getOp();

    if(
        op==OperatorFunInvokeExpression::Operator::PRE_INC
        ||
        op==OperatorFunInvokeExpression::Operator::PRE_DEC
        ||
        op==OperatorFunInvokeExpression::Operator::POST_INC
        ||
        op==OperatorFunInvokeExpression::Operator::POST_DEC
    ){

        ex->getInside()->accept(lAssigner);

        DX=pop();

        AX=DX;

        ex->getFun()->accept(this);

        push(AX); // push return value to assign it

        ex->getInside()->accept(rAssigner);
        
        switch(op){
            case OperatorFunInvokeExpression::Operator::PRE_INC:
            case OperatorFunInvokeExpression::Operator::PRE_DEC:
                push(AX);
                break;
            case OperatorFunInvokeExpression::Operator::POST_INC:
            case OperatorFunInvokeExpression::Operator::POST_DEC:
                push(DX);
                break;
            default:{}
        }

        return;
    }
    
    ex->getInside()->accept(this);

    auto args=ex->getArgs();

    for(auto arg:*args){
        arg->accept(this);
    }

    switch(op){
        case OperatorFunInvokeExpression::Operator::SET_EQUAL:
            DX=pop();   // value arg
        case OperatorFunInvokeExpression::Operator::PLUS:
        case OperatorFunInvokeExpression::Operator::MINUS:
        case OperatorFunInvokeExpression::Operator::TIMES:
        case OperatorFunInvokeExpression::Operator::DIV:
        case OperatorFunInvokeExpression::Operator::MOD:
        case OperatorFunInvokeExpression::Operator::POW:
        case OperatorFunInvokeExpression::Operator::EQUAL_EQUAL:
        case OperatorFunInvokeExpression::Operator::NOT_EQUAL:
        case OperatorFunInvokeExpression::Operator::LESS:
        case OperatorFunInvokeExpression::Operator::LESS_EQUAL:
        case OperatorFunInvokeExpression::Operator::GREATER:
        case OperatorFunInvokeExpression::Operator::GREATER_EQUAL:
        case OperatorFunInvokeExpression::Operator::SHR:
        case OperatorFunInvokeExpression::Operator::SHL:
        case OperatorFunInvokeExpression::Operator::BIT_AND:
        case OperatorFunInvokeExpression::Operator::XOR:
        case OperatorFunInvokeExpression::Operator::BIT_OR:
        case OperatorFunInvokeExpression::Operator::GET:
            CX=pop();   // 2nd operand
        case OperatorFunInvokeExpression::Operator::UNARY_PLUS:
        case OperatorFunInvokeExpression::Operator::UNARY_MINUS:
        case OperatorFunInvokeExpression::Operator::LOGICAL_NOT:
        case OperatorFunInvokeExpression::Operator::BIT_NOT:
            AX=pop();   // 1st operand
            break;
        default:{}
    }

    ex->getFun()->accept(this);

    switch(op){
        case OperatorFunInvokeExpression::Operator::NOT_EQUAL:{
            auto val=std::dynamic_pointer_cast<BoolValue>(AX)->getValue();
            AX=std::make_shared<BoolValue>(!val);
            break;
        }
        case OperatorFunInvokeExpression::Operator::LESS:{
            auto val=std::dynamic_pointer_cast<IntValue>(AX)->getValue();
            AX=std::make_shared<BoolValue>(val<0);
            break;
        }
        case OperatorFunInvokeExpression::Operator::LESS_EQUAL:{
            auto val=std::dynamic_pointer_cast<IntValue>(AX)->getValue();
            AX=std::make_shared<BoolValue>(val<=0);
            break;
        }
        case OperatorFunInvokeExpression::Operator::GREATER:{
            auto val=std::dynamic_pointer_cast<IntValue>(AX)->getValue();
            AX=std::make_shared<BoolValue>(val>0);
            break;
        }
        case OperatorFunInvokeExpression::Operator::GREATER_EQUAL:{
            auto val=std::dynamic_pointer_cast<IntValue>(AX)->getValue();
            AX=std::make_shared<BoolValue>(val>=0);
            break;
        }
        default:
            break;
    }

    push(AX);
}

void Interpreter::initMultiDArray(){

    push(std::make_shared<RefValue>(*DI));

    auto dimensionsSize=std::dynamic_pointer_cast<IntValue>(CX)->getValue();
    
    DX=memory[*BP+dimensionsSize];
    
    auto arraySize=std::dynamic_pointer_cast<IntValue>(DX)->getValue();
    
    *DI=getAvailableHeapAddress(arraySize);

    if(dimensionsSize>1){
        for(int i=0;i<arraySize;i++){
            CX=std::make_shared<IntValue>(dimensionsSize-1);
            initMultiDArray();
            memory[*DI+i+1]=AX;
        }
    }

    AX=std::make_shared<RefValue>(*DI);

    *DI=popAs<RefValue>()->getAddress();
}

int Interpreter::getAvailableHeapAddress(ClassScope* scope){
    int objSize=scope->getPublicVariables()->size()+scope->getPrivateVariables()->size();
    return getAvailableHeapAddress(objSize);
}

int Interpreter::getAvailableHeapAddress(int size){
    int heapAddress=*SS;

    for(int i=DATA_SIZE;i<*SS;){
        if(auto reservedObj=memory[i]){
            auto reservedObjSize=std::dynamic_pointer_cast<IntValue>(reservedObj)->getValue();
            i+=reservedObjSize+1;
            continue;
        }
        int j=i;
        int lastValidCell=i+size;
        if(lastValidCell>=*SS)
            break; // This will throw OutOfMemoryException as heapAddress equals SS
        for(;j<=lastValidCell;j++){
            if(memory[j])
                break;
        }
        if(j>lastValidCell){
            heapAddress=i;
            break;
        }
        i=j;
    }

    if(heapAddress>=*SS)
        throw OutOfMemoryException();

    memory[heapAddress]=std::make_shared<IntValue>(size);

    return heapAddress;
}

Interpreter::Interpreter():
    DI(new int(-1)),
    BP(new int(MEM_SIZE-1)),
    SP(new int(*BP)),
    BX(new int(-1)),
    DS(new int(0)),
    SS(new int(DATA_SIZE+HEAP_SIZE)),
    funReturn(false),
    loopBreak(false),
    loopContinue(false)
{}

Interpreter::LeftSideAssigner::LeftSideAssigner(Interpreter* interpreter)
:interpreter(interpreter){}

void Interpreter::LeftSideAssigner::visit(VarAccessExpression* ex){
    ex->accept(interpreter);
}

void Interpreter::LeftSideAssigner::visit(NonStaticVarAccessExpression* ex){
    ex->getInside()->accept(interpreter);
    auto ref=interpreter->topAs<RefValue>()->getAddress();
    interpreter->push(
        interpreter->memory[ref+interpreter->offsets[ex->getVar().get()].value]
    );
}

Interpreter::RightSideAssigner::RightSideAssigner(Interpreter* interpreter)
:interpreter(interpreter){}

void Interpreter::RightSideAssigner::visit(VarAccessExpression* ex){
    auto offset=interpreter->offsets[ex->getVar().get()];
    interpreter->memory[*offset.reg+offset.value]=interpreter->pop();
}

void Interpreter::RightSideAssigner::visit(NonStaticVarAccessExpression* ex){
    auto val=interpreter->pop();
    auto ref=interpreter->popAs<RefValue>()->getAddress();
    interpreter->memory[ref+interpreter->offsets[ex->getVar().get()].value]=val;
}

void Interpreter::visit(PackageScope* scope){
    for(auto fileIterator:scope->getFiles()){
        fileIterator.second->accept(this);
    }
    for(auto packageIterator:scope->getPackages()){
        packageIterator.second->accept(this);
    }
}

void Interpreter::visit(FileScope* scope){
    scope->getGlobalVarsInitStmList()->accept(this);
}

void Interpreter::visit(ClassScope* scope){
    scope->getVarsInitStmList()->accept(this);
}

void Interpreter::visit(FunScope* scope){

    push(std::make_shared<IntValue>(*BP));

    *BP=*SP;

    auto decl=scope->getDecl();
    auto isConstructor=decl->isConstructor();

    if(isConstructor)
        decl->returnType->getClassScope()->accept(this);

    // locals contains params, so we don't offset params
    auto localsSize=scope->getLocals()->size();
    auto paramsSize=scope->getDecl()->params->size();

    if(localsSize>paramsSize)
        reserveSpaceForStmListLocals(localsSize-paramsSize);

    for(auto stm:*scope->getStmList()){
        stm->accept(this);
        if(funReturn)
            break;
    }

    if(isConstructor)
        AX=std::make_shared<RefValue>(*BX);

    // When return type of a function is Void but it reaches its end, without an explicit return
    else if(!funReturn) 
        AX=std::make_shared<VoidValue>();
    
    funReturn=false;

    *SP=*BP;

    *BP=popAs<IntValue>()->getValue();

}

void Interpreter::visit(BuiltInFunScope* scope){
    // Built-in functions don't affect BP, so we don't need to push it on the stack

    scope->invokeOnInterpreter(this);

}

void Interpreter::visit(LoopScope* scope){
    reserveSpaceForStmListLocals(scope->getLocals()->size());
    runStmList(scope);
    removeReservedSpaceForStmListLocals(scope->getLocals()->size());
}

void Interpreter::visit(StmListScope* scope){
    reserveSpaceForStmListLocals(scope->getLocals()->size());
    runStmList(scope);
    removeReservedSpaceForStmListLocals(scope->getLocals()->size());
}

void Interpreter::visit(VarStm* stm){
    stm->getEx()->accept(this);
    auto offset=offsets[stm->getVar().get()];
    memory[*offset.reg+offset.value]=pop();
}

void Interpreter::visit(AssignStatement* stm){
    stm->getLeft()->accept(lAssigner);
    pop();
    stm->getRight()->accept(this);
    stm->getLeft()->accept(rAssigner);
}

void Interpreter::visit(AugmentedAssignStatement* stm){

    if(auto isBuiltIn=std::dynamic_pointer_cast<BuiltInFunScope>(stm->getOpFun())){
        stm->getLeft()->accept(lAssigner);
        stm->getRight()->accept(this);
        CX=pop();
        AX=pop();
        stm->getOpFun()->accept(this);
        push(AX);
        stm->getLeft()->accept(rAssigner);
    }else{
        push(std::make_shared<RefValue>(*BX));
        stm->getLeft()->accept(lAssigner);
        *BX=popAs<RefValue>()->getAddress();
        stm->getRight()->accept(this);
        stm->getOpFun()->accept(this);
        pop();  // pop right
        push(AX);
        stm->getLeft()->accept(rAssigner);
        *BX=popAs<RefValue>()->getAddress();
    }
    
}

void Interpreter::visit(IfStatement* stm){
    SharedStmListScope scope;

    stm->getIfCondition()->accept(this);

    if(popAs<BoolValue>()->getValue()){
        scope=stm->getIfScope();
    }
    else if(auto elseScope=stm->getElseScope()){
        scope=elseScope;
    }
    else return;
    
    scope->accept(this);
}

void Interpreter::visit(WhileStatement* stm){
    
    while(true){
        stm->getCondition()->accept(this);

        if(!popAs<BoolValue>()->getValue())
            break;
        
        stm->getLoopScope()->accept(this);

        if(loopContinue)
            loopContinue=false;

        if(loopBreak){
            loopBreak=false;
            break;
        }

        if(funReturn)
            break;
        
    }
}

void Interpreter::visit(DoWhileStatement* stm){

    do{
        stm->getLoopScope()->accept(this);

        if(loopContinue)
            loopContinue=false;

        if(loopBreak){
            loopBreak=false;
            break;
        }

        if(funReturn)
            break;
        
        stm->getCondition()->accept(this);

        if(!popAs<BoolValue>()->getValue())
            break;
        
    }while(true);
}

void Interpreter::visit(BreakStatement* stm){
    loopBreak=true;
}

void Interpreter::visit(ContinueStatement* stm){
    loopContinue=true;
}

void Interpreter::visit(ReturnStatement* stm){
    stm->getEx()->accept(this);
    AX=pop();
    funReturn=true;
}

void Interpreter::visit(ExpressionStatement* stm){
    stm->getEx()->accept(this);
    pop();
}

void Interpreter::visit(VarAccessExpression* ex){
    auto offset=offsets[ex->getVar().get()];
    push(memory[*offset.reg+offset.value]);
}

void Interpreter::visit(FunInvokeExpression* ex){

    auto args=ex->getArgs();

    for(auto arg:*args){
        arg->accept(this);
    }

    ex->getFun()->accept(this);

    for(auto arg:*args){
        pop();
    }

    push(AX); // return value
}

void Interpreter::visit(NewObjectExpression* ex){

    int heapAddress=getAvailableHeapAddress(ex->getReturnType()->getClassScope().get());

    push(std::make_shared<RefValue>(*BX));

    *BX=heapAddress;

    auto args=ex->getArgs();

    for(auto arg:*args){
        arg->accept(this);
    }

    ex->getConstructor()->accept(this);

    for(auto arg:*args){
        pop();
    }

    *BX=popAs<RefValue>()->getAddress();

    push(AX); // return address of object
}

void Interpreter::visit(NewArrayExpression* ex){

    auto capExs=ex->getCapacities();
    
    for(auto capEx:capExs){
        capEx->accept(this);
        auto cap=topAs<IntValue>()->getValue();
        if(cap<0)
            throw ArrayNegativeCapacityException(cap);
    }

    push(std::make_shared<IntValue>(*BP));

    *BP=*SP;

    // dimensions size
    CX=std::make_shared<IntValue>(ex->getCapacities().size());

    initMultiDArray();

    *SP=*BP;

    *BP=popAs<IntValue>()->getValue();

    for(auto capEx:capExs){
        pop();
    }

    push(AX);

}

void Interpreter::visit(LiteralExpression* ex){
    push(ex->getValue());
}

void Interpreter::visit(VoidExpression* ex){
    push(std::make_shared<VoidValue>());
}

void Interpreter::visit(LogicalExpression* ex){
    ex->getLeft()->accept(this);
    auto leftVal=popAs<BoolValue>();
    auto leftBoolVal=leftVal->getValue();
    auto op=ex->getLogicalOp();
    if(
        leftBoolVal&&op==LogicalExpression::Operation::OR
        ||
        !leftBoolVal&&op==LogicalExpression::Operation::AND
    ){
        push(leftVal);
        return;
    }
    ex->getRight()->accept(this);
}

void Interpreter::visit(NonStaticVarAccessExpression* ex){
    ex->getInside()->accept(this);
    AX=pop();
    auto ax=std::dynamic_pointer_cast<RefValue>(AX)->getAddress();
    push(
        memory[ax+offsets[ex->getVar().get()].value]
    );
}

void Interpreter::visit(NonStaticFunInvokeExpression* ex){
    if(auto builtIn=std::dynamic_pointer_cast<BuiltInFunScope>(ex->getFun())){
        invokeNonStaticBuiltInFun(ex);
        return;
    }
    invokeNonStaticFun(ex);
}

void Interpreter::visit(OperatorFunInvokeExpression* ex){
    
    if(auto builtIn=std::dynamic_pointer_cast<BuiltInFunScope>(ex->getFun())){
        invokeBuiltInOpFun(ex);
        return;
    }

    auto op=ex->getOp();

    if(
        op==OperatorFunInvokeExpression::Operator::PRE_INC
        ||
        op==OperatorFunInvokeExpression::Operator::PRE_DEC
        ||
        op==OperatorFunInvokeExpression::Operator::POST_INC
        ||
        op==OperatorFunInvokeExpression::Operator::POST_DEC
    ){
        push(std::make_shared<RefValue>(*BX));

        ex->getInside()->accept(lAssigner);

        auto oldVal=pop();

        *BX=std::dynamic_pointer_cast<RefValue>(oldVal)->getAddress();
        
        ex->getFun()->accept(this);

        *BX=popAs<RefValue>()->getAddress();

        push(AX); // return value

        ex->getInside()->accept(rAssigner);
        
        switch(op){
            case OperatorFunInvokeExpression::Operator::PRE_INC:
            case OperatorFunInvokeExpression::Operator::PRE_DEC:
                push(AX);
                break;
            case OperatorFunInvokeExpression::Operator::POST_INC:
            case OperatorFunInvokeExpression::Operator::POST_DEC:
                push(oldVal);
                break;
            default:break;
        }

        return;
    }

    invokeNonStaticFun(ex);
    switch(op){
        case OperatorFunInvokeExpression::Operator::NOT_EQUAL:{
            auto val=popAs<BoolValue>()->getValue();
            push(std::make_shared<BoolValue>(!val));
            break;
        }
        case OperatorFunInvokeExpression::Operator::LESS:{
            auto val=popAs<IntValue>()->getValue();
            push(std::make_shared<BoolValue>(val<0));
            break;
        }
        case OperatorFunInvokeExpression::Operator::LESS_EQUAL:{
            auto val=popAs<IntValue>()->getValue();
            push(std::make_shared<BoolValue>(val<=0));
            break;
        }
        case OperatorFunInvokeExpression::Operator::GREATER:{
            auto val=popAs<IntValue>()->getValue();
            push(std::make_shared<BoolValue>(val>0));
            break;
        }
        case OperatorFunInvokeExpression::Operator::GREATER_EQUAL:{
            auto val=popAs<IntValue>()->getValue();
            push(std::make_shared<BoolValue>(val>=0));
            break;
        }
        default:
            break;
    }
}

void Interpreter::visit(SetOperatorExpression* ex){

    auto op=ex->getOp();

    auto funOfGet=ex->getExOfGet()->getFun().get();
    auto funOfOp=ex->getFunOfOp().get();
    auto funOfSet=ex->getFunOfSet().get();
    
    if(auto isBuiltIn=dynamic_cast<BuiltInFunScope*>(funOfGet)!=nullptr){

        ex->getExHasGetOp()->accept(this);

        ex->getIndexEx()->accept(this);

        CX=pop(); // index

        AX=top(); // ex has get() and set()

        funOfGet->accept(this);

    }else{
        push(std::make_shared<RefValue>(*BX));

        ex->getExHasGetOp()->accept(this);
        
        *BX=topAs<RefValue>()->getAddress();

        ex->getIndexEx()->accept(this);

        funOfGet->accept(this);

        CX=pop(); // index

        DX=pop(); // ex has get() and set()

        *BX=popAs<RefValue>()->getAddress();

        push(DX); // ex has get() and set()
    }

    push(CX); // index

    push(AX); // value of get()

    switch(op){
        case SetOperatorExpression::Operator::PLUS_EQUAL:
        case SetOperatorExpression::Operator::MINUS_EQUAL:
        case SetOperatorExpression::Operator::TIMES_EQUAL:
        case SetOperatorExpression::Operator::DIV_EQUAL:
        case SetOperatorExpression::Operator::MOD_EQUAL:
        case SetOperatorExpression::Operator::POW_EQUAL:
        case SetOperatorExpression::Operator::SHR_EQUAL:
        case SetOperatorExpression::Operator::SHL_EQUAL:
        case SetOperatorExpression::Operator::BIT_AND_EQUAL:
        case SetOperatorExpression::Operator::XOR_EQUAL:
        case SetOperatorExpression::Operator::BIT_OR_EQUAL:{
            // built-in is for a primitive
            if(auto isBuiltIn=dynamic_cast<BuiltInFunScope*>(funOfOp)){
                ex->getValueEx()->accept(this);
                CX=pop(); // 2nd operand i.e. the value of right side
                AX=top(); // 1st operand i.e. value of get()
                funOfOp->accept(this);
            }else{
                push(std::make_shared<RefValue>(*BX));
                // AX contains the value of get()
                *BX=std::dynamic_pointer_cast<RefValue>(AX)->getAddress();
                ex->getValueEx()->accept(this);
                funOfOp->accept(this);
                pop(); // pop value
                *BX=popAs<RefValue>()->getAddress();
            }
            break;
        }
        case SetOperatorExpression::Operator::PRE_INC:
        case SetOperatorExpression::Operator::PRE_DEC:
        case SetOperatorExpression::Operator::POST_INC:
        case SetOperatorExpression::Operator::POST_DEC:{
            // built-in is for a primitive
            if(auto isBuiltIn=dynamic_cast<BuiltInFunScope*>(funOfOp)){
                // AX is the 1st operand and contains the value of get()
                funOfOp->accept(this);
            }else{
                push(std::make_shared<RefValue>(*BX));
                // AX contains the value of get()
                *BX=std::dynamic_pointer_cast<RefValue>(AX)->getAddress();
                funOfOp->accept(this);
                *BX=popAs<RefValue>()->getAddress();
            }
            break;
        }
        default:break;
    }

    // AX has the value of op fun i.e. the value to set

    if(auto isBuiltIn=dynamic_cast<BuiltInFunScope*>(funOfSet)){
        DX=AX;  // AX has the value of op fun i.e. the value to set
        // memory[*SP] is the value of get(), but we don't need it here
        CX=memory[*SP+1]; // index
        AX=memory[*SP+2]; // ex has get() and set() i.e. the array
        funOfSet->accept(this);
        DX=pop();   // the value of get()
        CX=pop();   // index
    }else{
        DX=pop(); // The value of get()
        CX=pop(); // index
        push(DX); // The value of get()
        push(std::make_shared<RefValue>(*BX));
        push(CX); // index
        push(AX); // the value to set
        // memory[*SP+4] contains the ex has get() and set()
        *BX=std::dynamic_pointer_cast<RefValue>(memory[*SP+4])->getAddress();
        funOfSet->accept(this);
        pop();    // the value to set
        CX=pop(); // index
        *BX=popAs<RefValue>()->getAddress();
        DX=pop(); // The value of get()
    }

    // AX has the value of set fun

    switch(op){
        case SetOperatorExpression::Operator::PLUS_EQUAL:
        case SetOperatorExpression::Operator::MINUS_EQUAL:
        case SetOperatorExpression::Operator::TIMES_EQUAL:
        case SetOperatorExpression::Operator::DIV_EQUAL:
        case SetOperatorExpression::Operator::MOD_EQUAL:
        case SetOperatorExpression::Operator::POW_EQUAL:
        case SetOperatorExpression::Operator::SHR_EQUAL:
        case SetOperatorExpression::Operator::SHL_EQUAL:
        case SetOperatorExpression::Operator::BIT_AND_EQUAL:
        case SetOperatorExpression::Operator::XOR_EQUAL:
        case SetOperatorExpression::Operator::BIT_OR_EQUAL:{
            pop();  // the ex has get() and set()
            push(AX);   // return value of set fun
            break;
        }
        case SetOperatorExpression::Operator::PRE_INC:
        case SetOperatorExpression::Operator::PRE_DEC:{
            AX=pop(); // ex has get() and set()
            if(auto isBuiltIn=dynamic_cast<BuiltInFunScope*>(funOfGet)){
                funOfGet->accept(this);
            }else{
                push(std::make_shared<RefValue>(*BX));
                *BX=std::dynamic_pointer_cast<RefValue>(AX)->getAddress();
                push(CX); // index
                funOfGet->accept(this);
                pop(); // index
                *BX=popAs<RefValue>()->getAddress();
            }
            push(AX);
            break;
        }
        case SetOperatorExpression::Operator::POST_INC:
        case SetOperatorExpression::Operator::POST_DEC:{
            pop();  // the ex has get() and set()
            push(DX); // The value of get()
            break;
        }
        default:break;
    }
}

void Interpreter::visit(ThisExpression* ex){
    push(std::make_shared<RefValue>(*BX));
}

void Interpreter::visit(ThisVarAccessExpression* ex){
    auto offset=offsets[ex->getVar().get()];
    push(memory[*offset.reg+offset.value]);
}

void Interpreter::visit(ThisFunInvokeExpression* ex){
    
    auto args=ex->getArgs();

    for(auto arg:*args){
        arg->accept(this);
    }

    ex->getFun()->accept(this);

    for(auto arg:*args){
        pop();
    }

    push(AX); // return value
}