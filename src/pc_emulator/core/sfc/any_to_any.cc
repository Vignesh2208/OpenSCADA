#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/include/sfc/any_to_any.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;


void ANY_TO_ANY::Execute(std::vector<PCVariable*>& Operands) {
    for(int i = 0; i < Operands.size(); i++) {
        Execute(Operands[i]);
    }
    
}

void ANY_TO_ANY::Execute(PCVariable* Operand) {
    auto configuration = __AssociatedResource->__configuration;
    auto Op = Operand;
    
    if (!Op->__IsTemporary &&  Op->__VariableDataType->__DataTypeCategory 
        != __TargetDataType->__DataTypeCategory) {
        __AssociatedResource->__configuration->PCLogger->RaiseException(
            "Operand: " + Op->__VariableName + " is not Temporary and of an"
            " incompatible type");
    }


    if (Op->__IsTemporary) {
        switch(Op->__VariableDataType->__DataTypeCategory) {
            case DataTypeCategory::BOOL :  Utils::BOOL_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                            break;
            case DataTypeCategory::BYTE :  Utils::BYTE_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                            break; 
            case DataTypeCategory::WORD :  Utils::WORD_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                            break; 
            case DataTypeCategory::DWORD :  Utils::DWORD_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                            break;
            case DataTypeCategory::LWORD : Utils::LWORD_TO_ANY(
                                                configuration,
                                                Op, __TargetDataType);
                                            break;
            case DataTypeCategory::CHAR :  Utils::CHAR_TO_ANY(
                                                configuration,
                                                Op, __TargetDataType);
                                            break; 
            case DataTypeCategory::INT :  Utils::INT_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                            break; 
            case DataTypeCategory::SINT :  Utils::SINT_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                            break; 
            case DataTypeCategory::DINT :  Utils::DINT_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                            break; 
            case DataTypeCategory::LINT :  Utils::LINT_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                            break; 
            case DataTypeCategory::UINT :  Utils::UINT_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                            break; 
            case DataTypeCategory::USINT :  Utils::USINT_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                            break;
            case DataTypeCategory::UDINT :  Utils::UDINT_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                            break;
            case DataTypeCategory::ULINT :  Utils::ULINT_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                            break;
            case DataTypeCategory::REAL :   Utils::REAL_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                            break;
            case DataTypeCategory::LREAL :  Utils::LREAL_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                            break;

            case DataTypeCategory::DATE_AND_TIME:
                        if(__TargetDataType->__DataTypeCategory
                            == DataTypeCategory::TIME_OF_DAY) {
                                Utils::DT_TO_TOD(
                                configuration,
                                Op, __TargetDataType);
                        } else if (__TargetDataType->__DataTypeCategory
                            == DataTypeCategory::DATE){
                                Utils::DT_TO_DATE(
                                configuration,
                                Op, __TargetDataType);                                       
                        } else {
                            configuration->PCLogger->RaiseException(
                                "DT data type can only be converted to "
                            "TOD or DATE");
                        }
                        break;
        
            default: configuration->PCLogger->RaiseException(
                                "Cannot cast this data type");
                    break;
                    
        }
    }

    
}