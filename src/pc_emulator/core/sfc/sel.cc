#include "src/pc_emulator/include/sfc/sel.h"
#include "src/pc_emulator/include/utils.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

void SEL::Execute(std::vector<PCVariable*>& Operands) {
    auto configuration = __AssociatedResource->__configuration;
    auto CR = __AssociatedResource->__CurrentResult;
    if (CR->__VariableDataType->__DataTypeCategory
        != DataTypeCategory::BOOL) {
        configuration->PCLogger->RaiseException("SEL SFC error: CR is not "
            " a boolean");
    }

    if (Operands.size() != 2) {
        configuration->PCLogger->RaiseException("SEL SFC error: "
            "Exactly 2 operands needed!");
    }

    if (Operands[0]->__VariableDataType->__DataTypeCategory 
        != Operands[1]->__VariableDataType->__DataTypeCategory) {
        configuration->PCLogger->RaiseException("SEL SFC error: "
            "Both operands must be of same type!");
    }
    bool SelValue = CR->GetValueStoredAtField<bool>("", DataTypeCategory::BOOL);

    if(SelValue) {
        *CR = *Operands[0];
    } else {
        *CR = *Operands[1];      
    }
    
}