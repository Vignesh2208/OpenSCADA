#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/include/sfc/any_to_any.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;


void ANY_TO_ANY::Execute(PCVariable * __CurrentResult,
    std::vector<PCVariable*>& Operands) {

    if (Operands.size()) {
        for(int i = 0; i < Operands.size(); i++) {
            auto returnVal = Execute(__CurrentResult, Operands[i]);
            if (returnVal == nullptr) {
                __AssociatedResource->__configuration->PCLogger->RaiseException(
                    "Type conversion error for: " 
                        + Operands[i]->__VariableDataType->__DataTypeName
                );
            }
            Operands[i] = returnVal;
        }
    } else {
        Execute(__CurrentResult, __CurrentResult);
    }
    
}

PCVariable * ANY_TO_ANY::Execute(PCVariable * __CurrentResult,
    PCVariable* Operand) {
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
            case DataTypeCategory::BOOL :  return Utils::BOOL_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                            
            case DataTypeCategory::BYTE :  return Utils::BYTE_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                             
            case DataTypeCategory::WORD :  return Utils::WORD_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                             
            case DataTypeCategory::DWORD :  return Utils::DWORD_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                        
            case DataTypeCategory::LWORD : return Utils::LWORD_TO_ANY(
                                                configuration,
                                                Op, __TargetDataType);
                                            
            case DataTypeCategory::CHAR :  return Utils::CHAR_TO_ANY(
                                                configuration,
                                                Op, __TargetDataType);
                                         
            case DataTypeCategory::INT :   return Utils::INT_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                             
            case DataTypeCategory::SINT :  return Utils::SINT_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                             
            case DataTypeCategory::DINT :  return Utils::DINT_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                             
            case DataTypeCategory::LINT :   return Utils::LINT_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                             
            case DataTypeCategory::UINT :  return Utils::UINT_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                             
            case DataTypeCategory::USINT :  return Utils::USINT_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                            
            case DataTypeCategory::UDINT :  return Utils::UDINT_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                            
            case DataTypeCategory::ULINT :  return Utils::ULINT_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                            
            case DataTypeCategory::REAL :   return Utils::REAL_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                            
            case DataTypeCategory::LREAL :  return Utils::LREAL_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);

            case DataTypeCategory::TIME :  
                                    return Utils::TIME_TO_ANY(
                                            configuration,
                                            Op, __TargetDataType);
                                            

            case DataTypeCategory::DATE_AND_TIME:
                        if(__TargetDataType->__DataTypeCategory
                            == DataTypeCategory::TIME_OF_DAY) {
                                return Utils::DT_TO_TOD(
                                configuration,
                                Op, __TargetDataType);
                        } else if (__TargetDataType->__DataTypeCategory
                            == DataTypeCategory::DATE){
                                return Utils::DT_TO_DATE(
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

    return nullptr;
}