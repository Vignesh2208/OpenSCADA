#include "src/pc_emulator/include/insns/ld_insn.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

/*
 * Sets the Current result accumulator to the passed operand.
 */
void LD_Insn::Execute(PCVariable * __CurrentResult,
    std::vector<PCVariable*>& Operands) {
    auto Logger = __AssociatedResource->__configuration->PCLogger.get();

    if (Operands.size() != 1) {
        Logger->RaiseException("LD can take exactly one argument ! "
            " Actual number of arguments provided = " +
            std::to_string(Operands.size()));
        
    }

    PCVariable * Operand = Operands[0];
    assert(Operand != nullptr);
   if (Operand->__IsVariableContentTypeAPtr) {
        Operand = Operand->GetPtrStoredAtField("");
        assert(Operand != nullptr);
    }
 

    if (IsNegated) {
        assert(Operand->__VariableDataType->__DataTypeCategory
            != DataTypeCategory::POU);

        auto tmp = Operand->GetCopy();
        *__CurrentResult = !(*tmp.get());
    } else {
        auto CurrentResult = __CurrentResult;
        *CurrentResult = *Operand;
    }
}