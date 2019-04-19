#include "src/pc_emulator/include/sfc/mux.h"
#include "src/pc_emulator/include/utils.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

void MUX::Execute(std::vector<PCVariable*>& MOperands) {
    auto configuration = __AssociatedResource->__configuration;
    auto CR = __AssociatedResource->__CurrentResult;
    if (CR->__VariableDataType->__DataTypeCategory
        < DataTypeCategory::USINT
        || CR->__VariableDataType->__DataTypeCategory
        > DataTypeCategory::ULINT) {
        configuration->PCLogger->RaiseException("MUX SFC error: CR is not "
            " a integer");
    }

    std::vector<PCVariable*> Operands;
    for(int i = 0; i < MOperands.size(); i++) {
        if (MOperands[i]->__IsVariableContentTypeAPtr) {
            auto Tmp = MOperands[i]->GetPtrStoredAtField("");
            assert(Tmp != nullptr);
            Operands.push_back(Tmp);
        } else {
            Operands.push_back(MOperands[i]);
        }
    }

    uint64_t SelValue;

    switch(CR->__VariableDataType->__DataTypeCategory) {
        case DataTypeCategory::USINT:
            SelValue = (uint64_t) CR->GetValueStoredAtField<uint8_t>("",
                DataTypeCategory::USINT);
                break;
        case DataTypeCategory::SINT:
            SelValue = (uint64_t) CR->GetValueStoredAtField<uint8_t>("",
                DataTypeCategory::SINT);
                break;
        case DataTypeCategory::UINT:
            SelValue = (uint64_t) CR->GetValueStoredAtField<uint8_t>("",
                DataTypeCategory::UINT);
                break;
        case DataTypeCategory::INT:
            SelValue = (uint64_t) CR->GetValueStoredAtField<uint8_t>("",
                DataTypeCategory::INT);
                break;
        case DataTypeCategory::UDINT:
            SelValue = (uint64_t) CR->GetValueStoredAtField<uint8_t>("",
                DataTypeCategory::UDINT);
                break;
        case DataTypeCategory::DINT:
            SelValue = (uint64_t) CR->GetValueStoredAtField<uint8_t>("",
                DataTypeCategory::DINT);
                break;
        case DataTypeCategory::ULINT:
            SelValue = (uint64_t) CR->GetValueStoredAtField<uint8_t>("",
                DataTypeCategory::ULINT);
                break;
        case DataTypeCategory::LINT:
            SelValue = (uint64_t) CR->GetValueStoredAtField<uint8_t>("",
                DataTypeCategory::LINT);
                break;
    }
    if (Operands.size() == 0) {
        configuration->PCLogger->RaiseException("MUX SFC error: "
            "Atleast 1 operand needed!");
    }

    if (SelValue > Operands.size() - 1) {
        configuration->PCLogger->RaiseException("MUX SFC error: "
            "Selection value out of bounds");
    }
    
    
    *CR = *Operands[SelValue];
}