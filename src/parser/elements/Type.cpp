#include "Type.hpp"
#include "ArrayClassScope.hpp"
#include "BuiltInFunScope.hpp"
#include "CharClassScope.hpp"
#include "SharedPtrTypes.hpp"
#include "StringClassScope.hpp"
#include "BoolClassScope.hpp"
#include "UnitClassScope.hpp"
#include "IntClassScope.hpp"
#include "UIntClassScope.hpp"
#include "LongClassScope.hpp"
#include "ULongClassScope.hpp"
#include "FloatClassScope.hpp"
#include "DoubleClassScope.hpp"
#include "FileScope.hpp"
#include "FunScope.hpp"
#include "FunDecl.hpp"
#include "FunParam.hpp"
#include <algorithm>
#include <memory>
#include <string>

Type::Type(SharedWString name,SharedClassScope classScope)
:name(name),classScope(classScope){}

const Type::Array* Type::asArray()const{
    return dynamic_cast<const Array*>(this);
}

bool Type::operator==(const Type& type)const{
    
    if(classScope!=type.classScope)
        return false;
    
    auto thisArray=this->asArray();
    
    if(!thisArray)
        return true;

    auto otherArray=type.asArray();

    return *thisArray->getType()==*otherArray->getType();
}

bool Type::operator!=(const Type& type)const{
    return !(*this==type);
}

SharedClassScope Type::getClassScope(){
    return classScope;
}

void Type::setClassScope(SharedClassScope classScope){
    this->classScope = classScope;
}

SharedWString Type::getName(){
    return name;
}

SharedWString Type::UNIT_NAME=std::make_shared<std::wstring>(L"الوحدة");
SharedWString Type::INT_NAME=std::make_shared<std::wstring>(L"صحيح");
SharedWString Type::UINT_NAME=std::make_shared<std::wstring>(L"صحيح_م");
SharedWString Type::LONG_NAME=std::make_shared<std::wstring>(L"كبير");
SharedWString Type::ULONG_NAME=std::make_shared<std::wstring>(L"كبير_م");
SharedWString Type::FLOAT_NAME=std::make_shared<std::wstring>(L"عشري");
SharedWString Type::DOUBLE_NAME=std::make_shared<std::wstring>(L"عشري_م");
SharedWString Type::CHAR_NAME=std::make_shared<std::wstring>(L"حرف");
SharedWString Type::STRING_NAME=std::make_shared<std::wstring>(L"نص");
SharedWString Type::BOOL_NAME=std::make_shared<std::wstring>(L"منطقي");
SharedWString Type::ARRAY_NAME=std::make_shared<std::wstring>(L"مصفوفة");

SharedType Type::UNIT=std::make_shared<Type>(
    UNIT_NAME,
    std::make_shared<UnitClassScope>()
);

SharedType Type::CHAR=std::make_shared<Type>(
    CHAR_NAME,
    std::make_shared<CharClassScope>()
);

SharedType Type::INT=std::make_shared<Type>(
    INT_NAME,
    std::make_shared<IntClassScope>()
);

SharedType Type::LONG=std::make_shared<Type>(
    LONG_NAME,
    std::make_shared<LongClassScope>()
);

SharedType Type::FLOAT=std::make_shared<Type>(
    FLOAT_NAME,
    std::make_shared<FloatClassScope>()
);

// صحيح موجب
SharedType Type::UINT=std::make_shared<Type>(
    UINT_NAME,
    std::make_shared<UIntClassScope>()
);

// كبير موجب
SharedType Type::ULONG=std::make_shared<Type>(
    ULONG_NAME,
    std::make_shared<ULongClassScope>()
);

// عشري مزدوج
SharedType Type::DOUBLE=std::make_shared<Type>(
    DOUBLE_NAME,
    std::make_shared<DoubleClassScope>()
);

SharedType Type::BOOL=std::make_shared<Type>(
    BOOL_NAME,
    std::make_shared<BoolClassScope>()
);

SharedType Type::STRING=std::make_shared<Type>(
    STRING_NAME,
    std::make_shared<StringClassScope>()
);

std::shared_ptr<ArrayClassScope> Type::ARRAY_CLASS=std::make_shared<ArrayClassScope>();

void Type::addBuiltInClassesTo(SharedFileScope fileScope) {
    auto builtInCLasses={
        UNIT,
        INT,UINT,LONG,ULONG,
        FLOAT,DOUBLE,
        CHAR,STRING,
        BOOL
    };
    auto privateClasses=fileScope->getPrivateClasses();
    for(auto builtInClass:builtInCLasses){
        (*privateClasses)[*builtInClass->getName()]=builtInClass->getClassScope();
    }
}

Type::~Type(){}

Type::Array::Array(SharedType type):
    Type(
        std::make_shared<std::wstring>(
            L"["+*type->getName()+L"]"
        ),
        Type::ARRAY_CLASS
    ),
    type(type){}

SharedType Type::Array::getType()const{
    return this->type;
}

SharedClassScope Type::Array::getClassScope(){

    auto funs=ARRAY_CLASS->getPublicFunctions();

    std::wstring GET_OLD_NAME=L"";
    std::wstring SET_OLD_NAME=L"";

    for(auto funIt:*funs){
        if(funIt.second==ArrayClassScope::GET){
            GET_OLD_NAME=funIt.first;
            continue;
        }
        if(funIt.second==ArrayClassScope::SET){
            SET_OLD_NAME=funIt.first;
            continue;
        }
        if(!GET_OLD_NAME.empty()&&!SET_OLD_NAME.empty())
            break;
    }

    ArrayClassScope::GET->getDecl()->returnType=this->type;
    ArrayClassScope::SET->getDecl()->params->at(1)->type=this->type;

    auto GET_nodeHandler=funs->extract(GET_OLD_NAME);
    auto SET_nodeHandler=funs->extract(SET_OLD_NAME);

    GET_nodeHandler.key()=ArrayClassScope::GET->getDecl()->toString();
    SET_nodeHandler.key()=ArrayClassScope::SET->getDecl()->toString();

    funs->insert(std::move(GET_nodeHandler));
    funs->insert(std::move(SET_nodeHandler));

    return this->classScope;
}

int Type::getSize(Type *type_ptr){

    auto type=*type_ptr;

    if (type==*Type::BOOL)
        return 1;
    
    if (type==*Type::CHAR||type==*Type::FLOAT||type==*Type::INT||type==*Type::UINT)
        return 4;

    if (type==*Type::DOUBLE||type==*Type::LONG||type==*Type::ULONG)
        return 8;

    // addresses are 8 bytes, TODO: need to handle if the system is 32-bit
    return 8;
}