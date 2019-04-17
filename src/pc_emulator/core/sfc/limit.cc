#include "src/pc_emulator/include/sfc/limit.h"
#include "src/pc_emulator/include/utils.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

void LIMIT::Execute(std::vector<PCVariable*>& Operands) {
    auto configuration = __AssociatedResource->__configuration;
    auto CR = __AssociatedResource->__CurrentResult;

    int max_idx = 0;
    if (Operands.size() == 2) {
        configuration->PCLogger->RaiseException("LIMIT SFC ERROR: "
            "Exactly 2 operands expected!");
    }
    for (int i = 0; i < Operands.size(); i++) {
        if (Operands[i]->__VariableDataType->__DataTypeCategory 
            == DataTypeCategory::ARRAY ||
            Operands[i]->__VariableDataType->__DataTypeCategory 
            == DataTypeCategory::POU ||
            Operands[i]->__VariableDataType->__DataTypeCategory 
            == DataTypeCategory::DERIVED) {
            configuration->PCLogger->RaiseException("LIMIT SFC ERROR: "
            "Only elementary data types supported!");
        }

        if (Operands[0]->__VariableDataType->__DataTypeCategory
            != Operands[i]->__VariableDataType->__DataTypeCategory) {
            configuration->PCLogger->RaiseException("LIMIT SFC ERROR: "
            "All operands must be of same type!");
        }
    }

    if (CR->__VariableDataType->__DataTypeCategory
        != Operands[0]->__VariableDataType->__DataTypeCategory) {
        configuration->PCLogger->RaiseException("LIMIT SFC ERROR: "
            "All operands must be of same type!");
    }
    
    if (*Operands[0] < *CR) {
        ;
    } else if (*Operands[0] > *Operands[1])
        *CR = *Operands[1];
    else
        *CR = *Operands[0]; 
    
}