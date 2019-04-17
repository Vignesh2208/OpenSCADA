#include "src/pc_emulator/include/sfc/min.h"
#include "src/pc_emulator/include/utils.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

void Min::Execute(std::vector<PCVariable*>& Operands) {
    auto configuration = __AssociatedResource->__configuration;
    auto CR = __AssociatedResource->__CurrentResult;

    int min_idx = 0;
    if (Operands.size() == 0) {
        configuration->PCLogger->RaiseException("Min SFC ERROR: "
            "Atleast one operand expected!");
    }
    for (int i = 0; i < Operands.size(); i++) {
        if (Operands[i]->__VariableDataType->__DataTypeCategory 
            == DataTypeCategory::ARRAY ||
            Operands[i]->__VariableDataType->__DataTypeCategory 
            == DataTypeCategory::POU ||
            Operands[i]->__VariableDataType->__DataTypeCategory 
            == DataTypeCategory::DERIVED) {
            configuration->PCLogger->RaiseException("Min SFC ERROR: "
            "Only elementary data types supported!");
        }

        if (Operands[0]->__VariableDataType->__DataTypeCategory
            != Operands[i]->__VariableDataType->__DataTypeCategory) {
            configuration->PCLogger->RaiseException("Min SFC ERROR: "
            "All operands must be of same type!");
        }

        if (*Operands[i] < *Operands[min_idx])
            min_idx = i;

    }

    if (CR->__VariableDataType->__DataTypeCategory
        != Operands[0]->__VariableDataType->__DataTypeCategory) {
        configuration->PCLogger->RaiseException("Min SFC ERROR: "
            "CR is not of same type as operands!");
    }
    if (*CR > *Operands[min_idx])
        *CR = *Operands[min_idx];
    
}