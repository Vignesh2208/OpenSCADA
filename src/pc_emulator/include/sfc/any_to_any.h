#ifndef __PC_EMULATOR_INCLUDE_SFC_ANY_TO_ANY_H
#define __PC_EMULATOR_INCLUDE_SFC_ANY_TO_ANY_H


#include "src/pc_emulator/include/sfc/sfc.h"

namespace pc_emulator {

    //! ANY_TO_ANY sfc
    class ANY_TO_ANY: public SFC {
        private:
            PCDataType * __TargetDataType;
            PCDataType * __SrcDataType;
        public:
            ANY_TO_ANY(PCResourceImpl * AssociatedResource,
                        PCDataType* TargetDataType,
                        PCDataType* SrcDataType) {
                __AssociatedResource = AssociatedResource;
                __TargetDataType = TargetDataType;
                __SrcDataType = SrcDataType;

                string SrcDataTypeName = SrcDataType->__DataTypeName;

                switch(__TargetDataType->__DataTypeCategory){
                    case DataTypeCategory::BOOL :   
                        __SfcName = SrcDataTypeName + "_TO_BOOL";
                                                    break;
                    case DataTypeCategory::BYTE :  
                        __SfcName = SrcDataTypeName + "_TO_BYTE";
                                                    break; 
                    case DataTypeCategory::WORD :  
                        __SfcName = SrcDataTypeName + "_TO_WORD";
                                                    break; 
                    case DataTypeCategory::DWORD :  
                        __SfcName = SrcDataTypeName + "_TO_DWORD";
                                                    break;
                    case DataTypeCategory::LWORD :  
                        __SfcName = SrcDataTypeName + "_TO_LWORD";
                                                    break;
                    case DataTypeCategory::CHAR :  
                        __SfcName = SrcDataTypeName + "_TO_CHAR";
                                                    break; 
                    case DataTypeCategory::INT :   
                        __SfcName = SrcDataTypeName + "_TO_INT";
                                                    break; 
                    case DataTypeCategory::SINT :  
                        __SfcName = SrcDataTypeName + "_TO_SINT";
                                                    break; 
                    case DataTypeCategory::DINT :  
                        __SfcName = SrcDataTypeName + "_TO_DINT";
                                                    break; 
                    case DataTypeCategory::LINT :  
                        __SfcName = SrcDataTypeName + "_TO_LINT";
                                                    break; 
                    case DataTypeCategory::UINT :  
                        __SfcName = SrcDataTypeName + "_TO_UINT";
                                                    break; 
                    case DataTypeCategory::USINT :  
                        __SfcName = SrcDataTypeName + "_TO_USINT";
                                                    break;
                    case DataTypeCategory::UDINT :  
                        __SfcName = SrcDataTypeName + "_TO_UDINT";
                                                    break;
                    case DataTypeCategory::ULINT :  
                        __SfcName = SrcDataTypeName + "_TO_ULINT";
                                                    break;
                    case DataTypeCategory::REAL :   
                        __SfcName = SrcDataTypeName + "_TO_REAL";
                                                    break;
                    case DataTypeCategory::LREAL :   
                        __SfcName = SrcDataTypeName + "_TO_LREAL";
                                                    break;
                    case DataTypeCategory::TIME_OF_DAY: 
                        if (__SrcDataType->__DataTypeCategory
                            == DataTypeCategory::DATE_AND_TIME) {
                        __SfcName 
                            = SrcDataTypeName + "DATE_AND_TIME_TO_TIME_OF_DAY";
                        } else {
                            __AssociatedResource->__configuration
                                    ->PCLogger->RaiseException(
                                        "Cannot create a type "
                                        "conversion sfc for complex data types");
                        }
                        break;
                    case DataTypeCategory::DATE: 
                        if (__SrcDataType->__DataTypeCategory
                            == DataTypeCategory::DATE_AND_TIME) {
                        __SfcName 
                            = SrcDataTypeName + "DATE_AND_TIME_TO_DATE";
                        } else {
                            __AssociatedResource->__configuration
                                    ->PCLogger->RaiseException(
                                        "Cannot create a type "
                                        "conversion sfc for complex data types");
                        }
                        break;
                    default:    __AssociatedResource->__configuration
                                    ->PCLogger->RaiseException(
                                        "Cannot create a type "
                                        "conversion sfc for complex data types");                   
                }
            };

            //! Called to execute the sfc
            /*!
                \param Operands     Operands to the sfc
            */
            void Execute(PCVariable *CurrentResult,
                std::vector<PCVariable*>& Operands);

            //! Called to execute the sfc
            /*!
                \param Operand     Operand to the sfc
            */
            PCVariable* Execute(PCVariable * CurrentResult,
                PCVariable* Operand);
    };
}

#endif