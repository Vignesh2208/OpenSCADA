#include "src/pc_emulator/include/insns/not_insn.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

/*
 * Sets the Current result accumulator to the passed operand.
 */
void NOT_Insn::Execute(PCVariable * __CurrentResult,
    std::vector<PCVariable*>& Operands) {
    auto Logger = __AssociatedResource->__configuration->PCLogger.get();

    if (Operands.size() > 1) {
        Logger->RaiseException("NOT can take exactly 0 or 1 arguments ! "
            " Actual number of arguments provided = " +
            std::to_string(Operands.size()));
        
    }

    if (Operands.size() == 1) {
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
        if (Operand->__IsVariableContentTypeAPtr) {
            Operand = Operand->GetPtrStoredAtField("");
            assert(Operand != nullptr);
        }

        auto tmp = Operand->GetCopy();
        *CurrentResult = !(*tmp);
    } else {
        auto CurrentResult = __CurrentResult;
        assert(CurrentResult->__VariableDataType->__DataTypeCategory
            == DataTypeCategory::BOOL);
        *CurrentResult = !(*CurrentResult);
    }
}