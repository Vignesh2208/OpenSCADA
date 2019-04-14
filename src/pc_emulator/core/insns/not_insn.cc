#include "src/pc_emulator/include/insns/not_insn.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

/*
 * Sets the Current result accumulator to the passed operand.
 */
void NOT_Insn::Execute(std::vector<PCVariable*>& Operands, bool isNegated) {
    auto Logger = __AssociatedResource->__configuration->PCLogger.get();

    if (Operands.size() != 0) {
        Logger->RaiseException("NOT can take exactly 0 arguments ! "
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
    auto CurrentResult = __AssociatedResource->__CurrentResult;
    if (Operand->__IsVariableContentTypeAPtr) {
        Operand = Operand->GetPtrStoredAtField("");
        assert(Operand != nullptr);
    }
    *CurrentResult = !(*CurrentResult);
}