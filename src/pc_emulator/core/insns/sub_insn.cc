#include "src/pc_emulator/include/insns/sub_insn.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

/*
 * Sets the Current result accumulator to the passed operand.
 */
void SUB_Insn::Execute(PCVariable * __CurrentResult,
                std::vector<PCVariable*>& Operands) {
    auto Logger = __AssociatedResource->__configuration->PCLogger.get();

    if (Operands.size() != 1) {
        Logger->RaiseException("SUB can take exactly one argument ! "
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
        == DataTypeCategory::TIME_OF_DAY
        && Operand->__VariableDataType->__DataTypeCategory
        == DataTypeCategory::TIME) {
        auto CRval = CurrentResult->GetValueStoredAtField<TODDataType>("",
                DataTypeCategory::TIME_OF_DAY);
        auto Opval = Operand->GetValueStoredAtField<TimeType>("",
                DataTypeCategory::TIME);

        DataTypeUtils::SubFromTOD(CRval, Opval);
        CurrentResult->SetField("", &CRval, sizeof(CRval));

    } else if (CurrentResult->__VariableDataType->__DataTypeCategory 
        == DataTypeCategory::TIME_OF_DAY
        && Operand->__VariableDataType->__DataTypeCategory
        == DataTypeCategory::TIME_OF_DAY) {
        auto CRval = CurrentResult->GetValueStoredAtField<TODDataType>("",
                DataTypeCategory::TIME_OF_DAY);
        auto Opval = Operand->GetValueStoredAtField<TODDataType>("",
                DataTypeCategory::TIME_OF_DAY);

        auto Res = DataTypeUtils::SubTODs(CRval, Opval);
        *CurrentResult = *__AssociatedResource->GetTmpVariable("TIME",
                "t#" + std::to_string(Res.SecsElapsed) + "s");

    } else if (CurrentResult->__VariableDataType->__DataTypeCategory 
        == DataTypeCategory::DATE_AND_TIME
        && Operand->__VariableDataType->__DataTypeCategory
        == DataTypeCategory::TIME) {
        auto CRval = CurrentResult->GetValueStoredAtField<DateTODDataType>("",
                DataTypeCategory::DATE_AND_TIME);
        auto Opval = Operand->GetValueStoredAtField<TimeType>("",
                DataTypeCategory::TIME);

        DataTypeUtils::SubFromDT(CRval, Opval);
        CurrentResult->SetField("", &CRval, sizeof(CRval));

    } else if (CurrentResult->__VariableDataType->__DataTypeCategory 
        == DataTypeCategory::DATE_AND_TIME
        && Operand->__VariableDataType->__DataTypeCategory
        == DataTypeCategory::DATE_AND_TIME) {
        auto CRval = CurrentResult->GetValueStoredAtField<DateTODDataType>("",
                DataTypeCategory::DATE_AND_TIME);
        auto Opval = Operand->GetValueStoredAtField<DateTODDataType>("",
                DataTypeCategory::DATE_AND_TIME);

        auto Res = DataTypeUtils::SubDTs(CRval, Opval);
        *CurrentResult = *__AssociatedResource->GetTmpVariable("TIME",
                "t#" + std::to_string(Res.SecsElapsed) + "s");

    } else if (CurrentResult->__VariableDataType->__DataTypeCategory 
        == DataTypeCategory::DATE
        && Operand->__VariableDataType->__DataTypeCategory
        == DataTypeCategory::DATE) {
        auto CRval = CurrentResult->GetValueStoredAtField<DateType>("",
                DataTypeCategory::DATE);
        auto Opval = Operand->GetValueStoredAtField<DateType>("",
                DataTypeCategory::DATE);

        auto Res = DataTypeUtils::SubDATEs(CRval, Opval);
        *CurrentResult = *__AssociatedResource->GetTmpVariable("TIME",
                "t#" + std::to_string(Res.SecsElapsed) + "s");
    } else {
        *CurrentResult = *CurrentResult - *Operand;
    }
}