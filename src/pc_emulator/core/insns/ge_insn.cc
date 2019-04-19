#include "src/pc_emulator/include/insns/ge_insn.h"
#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/include/sfc_registry.h"
#include "src/pc_emulator/include/sfc/any_to_any.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

/*
 * Sets the Current result accumulator to the passed operand.
 */
void GE_Insn::Execute(std::vector<PCVariable*>& Operands, bool isNegated) {
    auto Logger = __AssociatedResource->__configuration->PCLogger.get();

    if (Operands.size() != 1) {
        Logger->RaiseException("GE can take exactly one argument ! "
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

       if (Operand->__VariableDataType != CurrentResult->__VariableDataType) {
        std::vector<PCVariable *> modified_operands;
        modified_operands.push_back(Operand);

        PCDataType * DesiredDataType 
            = Utils::GetMostAppropriateTypeCast(CurrentResult,
                modified_operands);
        if(DesiredDataType == nullptr) {
            Logger->RaiseException("GE: Typecasting error!");
        }


        if (CurrentResult->__VariableDataType != DesiredDataType) {
            string conv_sfc_name 
                    = CurrentResult->__VariableDataType->__DataTypeName
                            + "_TO_" + DesiredDataType->__DataTypeName;
            ANY_TO_ANY * sfc = (ANY_TO_ANY*)(__AssociatedResource
                    ->__SFCRegistry->GetSFC(conv_sfc_name));

            assert(sfc != nullptr);
            assert(sfc->Execute(CurrentResult) == CurrentResult);   
        } else if(Operand->__IsTemporary 
            && Operand->__VariableDataType != DesiredDataType) {
            string conv_sfc_name 
                    = Operand->__VariableDataType->__DataTypeName
                            + "_TO_" + DesiredDataType->__DataTypeName;
            ANY_TO_ANY * sfc = (ANY_TO_ANY*)(__AssociatedResource
                    ->__SFCRegistry->GetSFC(conv_sfc_name));

            assert(sfc != nullptr);
            Operand = sfc->Execute(Operand); 

            if (!Operand) {
                Logger->RaiseException("Type casting error: " + conv_sfc_name);
            }   
        }

        assert (Operand->__VariableDataType 
            == CurrentResult->__VariableDataType);
    }
        

    if(*CurrentResult  >= *Operand) {
        *CurrentResult = * __AssociatedResource->GetTmpVariable("BOOL", "1");
    } else {
       *CurrentResult = * __AssociatedResource->GetTmpVariable("BOOL", "0"); 
    }
}