#include "ImplicitVarTypeChecker.hpp"
#include "Variable.hpp"
#include "MustHaveExplicitTypeException.hpp"
namespace Semantics {

    ImplicitVarTypeChecker::ImplicitVarTypeChecker(){}

    std::shared_ptr<ImplicitVarTypeChecker> ImplicitVarTypeChecker::getInstance(){
        if(!instance){
            ImplicitVarTypeChecker t;
            instance=std::make_shared<ImplicitVarTypeChecker>(t);
        }
        return instance;
    }

    void ImplicitVarTypeChecker::subscribe(std::shared_ptr<VarStm> varStm){
        checkStatements.push_back(varStm);
    }

    void ImplicitVarTypeChecker::check(SemanticsChecksVisitor* checker){
    	static int checkTimes=0;
    	checkTimes++;
        auto remainingStms=std::vector<std::shared_ptr<VarStm>>{};
        for(auto& stm:checkStatements){
            try{
            	stm->accept(checker);
            }catch(MustHaveExplicitTypeException& e){
                remainingStms.push_back(stm);
            }
            
        }
        
        if(remainingStms.empty())
        	return;
        
        if(remainingStms.size()==checkStatements.size()&&checkTimes>1){
        	throw MustHaveExplicitTypeException(0); //TODO: show trace
        }	
        	
        this->checkStatements=remainingStms;
        this->check(checker);
    }
}
