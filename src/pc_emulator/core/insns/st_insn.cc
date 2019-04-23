#include "src/pc_emulator/include/insns/st_insn.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

/*
 * Sets the Current result accumulator to the passed operand.
 */
void ST_Insn::Execute(PCVariable * __CurrentResult,
        std::vector<PCVariable*>& Operands) {
    auto Logger = __AssociatedResource->__configuration->PCLogger.get();

    if (Operands.size() != 1) {
        Logger->RaiseException("ST can take exactly one argument ! "
            " Actual number of arguments provided = " +
            std::to_string(Operands.size()));
        
    }

    PCVariable * Operand = Operands[0];
    assert(Operand != nullptr);
    if (Operand->__IsVariableContentTypeAPtr) {
        Operand = Operand->GetPtrStoredAtField("");
        assert(Operand != nullptr);
    }

    

    if (__CurrentResult
        ->__VariableDataType->__DataTypeName !=
        Operand->__VariableDataType->__DataTypeName) {
        Logger->RaiseException("Variable DataTypes of Curr Result and Operand "
            " do not match !");
    }

    if (IsNegated) {
        assert(Operand->__VariableDataType->__DataTypeCategory
            != DataTypeCategory::POU);
        std:: cout << "ST Insn Getting Copy\n";
        auto tmp = __CurrentResult->GetCopy();
        *tmp = !(*tmp);
        Operand->SetField("", tmp.get());
    } else {
        auto CurrentResult = __CurrentResult;
        Operand->SetField("", CurrentResult);
    }
}