#include "src/pc_emulator/include/insns/mul_insn.h"
#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/include/sfc_registry.h"
#include "src/pc_emulator/include/sfc/any_to_any.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

/*
 * Sets the Current result accumulator to the passed operand.
 */
void MUL_Insn::Execute(std::vector<PCVariable*>& Operands, bool isNegated) {
    auto Logger = __AssociatedResource->__configuration->PCLogger.get();
    auto CurrentResult = __AssociatedResource->__CurrentResult;
    std::vector<PCVariable *> modified_operands;
    modified_operands.push_back(CurrentResult);
    

    if (Utils::IsNumType(CurrentResult->__VariableDataType)) {
        if (Operands.size() == 0){
                Logger->RaiseException("MUL: Not enough arguments for "
                "NUM type");
        }

        for(int i = 0; i < Operands.size(); i++) {
        if (!Utils::IsNumType(Operands[i]->__VariableDataType)) {
                Logger->RaiseException("MUL: Cannot ADD non-num type to a "
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
                Logger->RaiseException("MUL: Cannot type cast operands");
        }
        assert(Utils::IsNumType(DesiredDataType));

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

        for (int i = 1; i < modified_operands.size(); i++) {
                *CurrentResult = (*CurrentResult)*(*modified_operands[i]);
        }

        return;
    }

    if (Operands.size() != 1) {
        Logger->RaiseException("MUL: for non num types can take exactly one argument ! "
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

    if (CurrentResult->__VariableDataType->__DataTypeCategory
        == DataTypeCategory::TIME) {

        if (!Utils::IsNumType(Operand->__VariableDataType)) {
            Logger->RaiseException("MUL: Only Integer types are allowed "
                " for multiplication with TIME!");
        }

        TimeType Timeval = CurrentResult->GetValueStoredAtField<TimeType>("",
                DataTypeCategory::TIME);
        switch(Operand->__VariableDataType->__DataTypeCategory) {
                case DataTypeCategory::INT:
                        Timeval.SecsElapsed *= 
                        Operand->GetValueStoredAtField<int16_t>("",
                                DataTypeCategory::INT);
                        break;
                case DataTypeCategory::SINT:
                        Timeval.SecsElapsed *= 
                        Operand->GetValueStoredAtField<int8_t>("",
                                DataTypeCategory::SINT);
                        break;
                case DataTypeCategory::DINT:
                        Timeval.SecsElapsed *= 
                        Operand->GetValueStoredAtField<int32_t>("",
                                DataTypeCategory::DINT);
                        break;
                case DataTypeCategory::LINT:
                        Timeval.SecsElapsed *= 
                        Operand->GetValueStoredAtField<int64_t>("",
                                DataTypeCategory::LINT);
                        break;

                case DataTypeCategory::UINT:
                        Timeval.SecsElapsed *= 
                        Operand->GetValueStoredAtField<uint16_t>("",
                                DataTypeCategory::UINT);
                        break;
                case DataTypeCategory::USINT:
                        Timeval.SecsElapsed *= 
                        Operand->GetValueStoredAtField<uint8_t>("",
                                DataTypeCategory::USINT);
                        break;
                case DataTypeCategory::UDINT:
                        Timeval.SecsElapsed *= 
                        Operand->GetValueStoredAtField<uint32_t>("",
                                DataTypeCategory::UDINT);
                        break;
                case DataTypeCategory::ULINT:
                        Timeval.SecsElapsed *= 
                        Operand->GetValueStoredAtField<uint64_t>("",
                                DataTypeCategory::ULINT);
                        break;
                default: Logger->RaiseException(
                        "MUL: Only Integer types are allowed "
                        " for multiplication with TIME!");
        }

        CurrentResult->SetField("", &Timeval, sizeof(Timeval));
        
    } else {
            Logger->RaiseException("MUL: Current result data type is "
                "incompatible");
    }
}