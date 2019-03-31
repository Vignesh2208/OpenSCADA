#include "src/pc_emulator/include/insns/st_insn.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

/*
 * Sets the Current result accumulator to the passed operand.
 */
void ST_Insn::Execute(std::vector<PCVariable*>& Operands, bool isNegated) {
    auto Logger = __AssociatedResource->__configuration->PCLogger.get();

    if (Operands.size() != 1) {
        Logger->RaiseException("ST can take exactly one argument ! "
            " Actual number of arguments provided = " +
            std::to_string(Operands.size()));
        
    }

    PCVariable * Operand = Operands[0];
    assert(Operand != nullptr);
    assert(Operand->__VariableDataType->__DataTypeCategory
            != DataTypeCategory::POU);

    if (__AssociatedResource->__CurrentResult
        ->__VariableDataType->__DataTypeName !=
        Operand->__VariableDataType->__DataTypeName) {
        Logger->RaiseException("Variable DataTypes of Curr Result and Operand "
            " do not match !");
    }

    if (isNegated) {
        auto tmp = __AssociatedResource->__CurrentResult->GetCopy();
        *tmp = !(*tmp);
        Operand->SetField("", tmp.get());
    } else {
        auto CurrentResult = __AssociatedResource->__CurrentResult;
        Operand->SetField("", CurrentResult);
    }
}