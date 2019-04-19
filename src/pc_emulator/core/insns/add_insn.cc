#include "src/pc_emulator/include/insns/add_insn.h"
#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/include/sfc_registry.h"
#include "src/pc_emulator/include/sfc/any_to_any.h"

using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

/*
 * Sets the Current result accumulator to the passed operand.
 */
void ADD_Insn::Execute(std::vector<PCVariable*>& Operands, bool isNegated) {
    auto Logger = __AssociatedResource->__configuration->PCLogger.get();

    auto CurrentResult = __AssociatedResource->__CurrentResult;
    std::vector<PCVariable *> modified_operands;
    modified_operands.push_back(CurrentResult);
    

    if (Utils::IsNumType(CurrentResult->__VariableDataType)) {
            if (Operands.size() == 0){
                    Logger->RaiseException("ADD: Not enough arguments for "
                        "NUM type");
            }
    
            for(int i = 0; i < Operands.size(); i++) {
                if (!Utils::IsNumType(Operands[i]->__VariableDataType)) {
                   Logger->RaiseException("ADD: Cannot ADD non-num type to a "
                        "num type");     
                }
                if (Operands[i]->__IsVariableContentTypeAPtr) {
                        modified_operands.push_back(
                                Operands[i]->GetPtrStoredAtField(""));
                } else {
                        modified_operands.push_back(Operands[i]);
                } 
            }

            auto DesiredDataType 
                = Utils::GetMostAppropriateTypeCast(CurrentResult,
                                modified_operands);
            if (!DesiredDataType) {
                    Logger->RaiseException("ADD: Cannot type cast operands");
            }
            assert(Utils::IsNumType(DesiredDataType));

            std::cout << "Desired DataType: " << DesiredDataType->__DataTypeName << std::endl;

            for (int i = 0; i < modified_operands.size(); i++) {
                if (modified_operands[i]->__IsTemporary
                        && modified_operands[i]->__VariableDataType
                        != DesiredDataType) {
                        string conv_sfc_name;
                        conv_sfc_name 
                        = modified_operands[i]->__VariableDataType->__DataTypeName
                          + "_TO_" + DesiredDataType->__DataTypeName;
                        ANY_TO_ANY * sfc = (ANY_TO_ANY*)(__AssociatedResource
                                ->__SFCRegistry->GetSFC(conv_sfc_name));

                        assert(sfc != nullptr);
                        modified_operands[i] 
                                = sfc->Execute(modified_operands[i]);
                        string ActualDataType 
                        = modified_operands[i]->__VariableDataType->__DataTypeName;
                        if (!modified_operands[i]) {
                                Logger->RaiseException(
                                "Type casting error: Actual DataType: "
                                        + ActualDataType + " Desired DataType: "
                                        + DesiredDataType->__DataTypeName);
                        } 
                }
            }

            for (int i = 0; i < modified_operands.size(); i++) {
                    if (i == 0)
                        *CurrentResult = *modified_operands[0];
                    else
                        *CurrentResult = *CurrentResult + *modified_operands[i];
            }

            return;
    }

    if (Operands.size() != 1) {
        Logger->RaiseException("ADD for non num types can take exactly one argument ! "
            " Actual number of arguments provided = " +
            std::to_string(Operands.size()));
        
    }

    PCVariable * Operand = Operands[0];
    assert(Operand != nullptr);
    if (Operand->__IsVariableContentTypeAPtr) {
        Operand = Operand->GetPtrStoredAtField("");
        assert(Operand != nullptr);
    }
    assert(Operand->__VariableDataType->__DataTypeCategory
            != DataTypeCategory::POU);

    assert(Operand->__VariableDataType->__DataTypeCategory
            != DataTypeCategory::DERIVED);

    assert(Operand->__VariableDataType->__DataTypeCategory
            != DataTypeCategory::ARRAY);   

    if (Operand->__VariableDataType->__DataTypeCategory != DataTypeCategory::TIME) {
            Logger->RaiseException("ADD: Only TIME is allowed as an operand!");
    }
    TimeType Timeval = Operand->GetValueStoredAtField<TimeType>("",
                DataTypeCategory::TIME);
    if (CurrentResult->__VariableDataType->__DataTypeCategory
        == DataTypeCategory::DATE_AND_TIME) {
        DateTODDataType DTval 
                = CurrentResult
                        ->GetValueStoredAtField<DateTODDataType>("",
                                DataTypeCategory::DATE_AND_TIME);
        DataTypeUtils::AddToDT(DTval, Timeval);
        CurrentResult->SetField("", &DTval, sizeof(DTval));

    } else if (CurrentResult->__VariableDataType->__DataTypeCategory
        == DataTypeCategory::TIME) {
        *CurrentResult = *CurrentResult + *Operand;

    } else if (CurrentResult->__VariableDataType->__DataTypeCategory
        == DataTypeCategory::TIME_OF_DAY) {
        TODDataType TODval 
                = CurrentResult
                        ->GetValueStoredAtField<TODDataType>("",
                                DataTypeCategory::TIME_OF_DAY);
        DataTypeUtils::AddToTOD(TODval, Timeval);
        CurrentResult->SetField("", &TODval, sizeof(TODval));

    } else {
            Logger->RaiseException("ADD: Current result data type is incompatible");
    }
}