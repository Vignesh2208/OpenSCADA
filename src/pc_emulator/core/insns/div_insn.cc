#include "src/pc_emulator/include/insns/div_insn.h"
#include "src/pc_emulator/include/utils.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

/*
 * Sets the Current result accumulator to the passed operand.
 */
void DIV_Insn::Execute(PCVariable * __CurrentResult,
        std::vector<PCVariable*>& Operands) {
    auto Logger = __AssociatedResource->__configuration->PCLogger.get();

    if (Operands.size() != 1) {
        Logger->RaiseException("DIV can take exactly one argument ! "
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
             
    auto CurrentResult = __CurrentResult;

    

    if (CurrentResult->__VariableDataType->__DataTypeCategory
        == DataTypeCategory::TIME) {

        if (!Utils::IsNumType(Operand->__VariableDataType)) {
            Logger->RaiseException("DIV: Only Integer types are allowed "
                " for division with TIME!");
        }

        TimeType Timeval = CurrentResult->GetValueStoredAtField<TimeType>("",
                DataTypeCategory::TIME);
        switch(Operand->__VariableDataType->__DataTypeCategory) {
                case DataTypeCategory::INT:
                        Timeval.SecsElapsed /= (double)
                        Operand->GetValueStoredAtField<int16_t>("",
                                DataTypeCategory::INT);
                        break;
                case DataTypeCategory::SINT:
                        Timeval.SecsElapsed /= (double)
                        Operand->GetValueStoredAtField<int8_t>("",
                                DataTypeCategory::SINT);
                        break;
                case DataTypeCategory::DINT:
                        Timeval.SecsElapsed /= (double)
                        Operand->GetValueStoredAtField<int32_t>("",
                                DataTypeCategory::DINT);
                        break;
                case DataTypeCategory::LINT:
                        Timeval.SecsElapsed /= (double)
                        Operand->GetValueStoredAtField<int64_t>("",
                                DataTypeCategory::LINT);
                        break;

                case DataTypeCategory::UINT:
                        Timeval.SecsElapsed /= (double)
                        Operand->GetValueStoredAtField<uint16_t>("",
                                DataTypeCategory::UINT);
                        break;
                case DataTypeCategory::USINT:
                        Timeval.SecsElapsed /= (double)
                        Operand->GetValueStoredAtField<uint8_t>("",
                                DataTypeCategory::USINT);
                        break;
                case DataTypeCategory::UDINT:
                        Timeval.SecsElapsed /= (double)
                        Operand->GetValueStoredAtField<uint32_t>("",
                                DataTypeCategory::UDINT);
                        break;
                case DataTypeCategory::ULINT:
                        Timeval.SecsElapsed /= (double)
                        Operand->GetValueStoredAtField<uint64_t>("",
                                DataTypeCategory::ULINT);
                        break;
                default: Logger->RaiseException(
                        "MUL: Only Integer types are allowed "
                        " for multiplication with TIME!");
        }

        CurrentResult->SetField("", &Timeval, sizeof(Timeval));
        
    } else {
            *CurrentResult = *CurrentResult / *Operand;
    }
}