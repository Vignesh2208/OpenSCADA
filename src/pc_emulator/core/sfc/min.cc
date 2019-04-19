#include "src/pc_emulator/include/sfc/min.h"
#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/include/sfc_registry.h"
#include "src/pc_emulator/include/sfc/any_to_any.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

void Min::Execute(std::vector<PCVariable*>& MOperands) {
    auto configuration = __AssociatedResource->__configuration;
    auto CR = __AssociatedResource->__CurrentResult;

    int min_idx = 0;
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
    if (Operands.size() == 0) {
        configuration->PCLogger->RaiseException("Min SFC ERROR: "
            "Atleast one operand expected!");
    }

    auto DesiredDataType 
                = Utils::GetMostAppropriateTypeCast(CR, Operands);
    if (!DesiredDataType) {
            configuration->PCLogger->RaiseException(
                    "Max: Cannot type cast operands");
    }
    
    for (int i = 0; i < Operands.size(); i++) {
        if (Operands[i]->__IsTemporary
                && Operands[i]->__VariableDataType
                != DesiredDataType) {
            string conv_sfc_name;
            conv_sfc_name 
            = Operands[i]->__VariableDataType->__DataTypeName
                    + "_TO_" + DesiredDataType->__DataTypeName;
            ANY_TO_ANY * sfc = (ANY_TO_ANY*)(__AssociatedResource
                    ->__SFCRegistry->GetSFC(conv_sfc_name));

            assert(sfc != nullptr);
            string ActualDataType 
            = Operands[i]->__VariableDataType->__DataTypeName;
            Operands[i] = sfc->Execute(Operands[i]);
            
            if (!Operands[i]) {
                    configuration->PCLogger->RaiseException(
                    "Type casting error: Actual DataType: "
                            + ActualDataType + " Desired DataType: "
                            + DesiredDataType->__DataTypeName);
            } 
        }
    }

    if (CR->__VariableDataType != DesiredDataType) {
        string conv_sfc_name 
            = CR->__VariableDataType->__DataTypeName
                + "_TO_" + DesiredDataType->__DataTypeName;
        ANY_TO_ANY * sfc = (ANY_TO_ANY*)(__AssociatedResource
                            ->__SFCRegistry->GetSFC(conv_sfc_name));
        assert(sfc != nullptr);
        sfc->Execute(CR);
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