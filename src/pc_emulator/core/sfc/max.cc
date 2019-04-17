#include "src/pc_emulator/include/sfc/max.h"
#include "src/pc_emulator/include/utils.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

void Max::Execute(std::vector<PCVariable*>& Operands) {
    auto configuration = __AssociatedResource->__configuration;
    auto CR = __AssociatedResource->__CurrentResult;

    int max_idx = 0;
    if (Operands.size() == 0) {
        configuration->PCLogger->RaiseException("Max SFC ERROR: "
            "Atleast one operand expected!");
    }
    for (int i = 0; i < Operands.size(); i++) {
        if (Operands[i]->__VariableDataType->__DataTypeCategory 
            == DataTypeCategory::ARRAY ||
            Operands[i]->__VariableDataType->__DataTypeCategory 
            == DataTypeCategory::POU ||
            Operands[i]->__VariableDataType->__DataTypeCategory 
            == DataTypeCategory::DERIVED) {
            configuration->PCLogger->RaiseException("Max SFC ERROR: "
            "Only elementary data types supported!");
        }

        if (Operands[0]->__VariableDataType->__DataTypeCategory
            != Operands[i]->__VariableDataType->__DataTypeCategory) {
            configuration->PCLogger->RaiseException("Max SFC ERROR: "
            "All operands must be of same type!");
        }

        if (*Operands[i] > *Operands[max_idx])
            max_idx = i;

    }

    if (CR->__VariableDataType->__DataTypeCategory
        != Operands[0]->__VariableDataType->__DataTypeCategory) {
        configuration->PCLogger->RaiseException("Max SFC ERROR: "
            "CR is not of same type as operands!");
    }
    if (*CR < *Operands[max_idx])
        *CR = *Operands[max_idx];
    
}