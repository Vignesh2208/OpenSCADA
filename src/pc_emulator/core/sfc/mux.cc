#include "src/pc_emulator/include/sfc/mux.h"
#include "src/pc_emulator/include/utils.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

void MUX::Execute(std::vector<PCVariable*>& Operands) {
    auto configuration = __AssociatedResource->__configuration;
    auto CR = __AssociatedResource->__CurrentResult;
    if (CR->__VariableDataType->__DataTypeCategory
        != DataTypeCategory::INT) {
        configuration->PCLogger->RaiseException("MUX SFC error: CR is not "
            " a integer");
    }

    int16_t SelValue = CR->GetValueStoredAtField<int16_t>("",
                DataTypeCategory::INT);
    if (Operands.size() == 0) {
        configuration->PCLogger->RaiseException("MUX SFC error: "
            "Atleast 1 operand needed!");
    }

    if (SelValue < 0 || SelValue > Operands.size() - 1) {
        configuration->PCLogger->RaiseException("MUX SFC error: "
            "Selection value out of bounds");
    }

    for (int i = 1; i < Operands.size(); i++) {
        if (Operands[0]->__VariableDataType->__DataTypeCategory 
            != Operands[i]->__VariableDataType->__DataTypeCategory) {
            configuration->PCLogger->RaiseException("MUX SFC error: "
                "All operands must be of same type!");
        }
    }

    
    
    *CR = *Operands[SelValue];
}