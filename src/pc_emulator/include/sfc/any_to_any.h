#ifndef __PC_EMULATOR_INCLUDE_SFC_ANY_TO_ANY_H
#define __PC_EMULATOR_INCLUDE_SFC_ANY_TO_ANY_H


#include "src/pc_emulator/include/sfc/sfc.h"

namespace pc_emulator {

    //! ANY_TO_ANY sfc
    class ANY_TO_ANY: public SFC {
        private:
            PCDataType * __TargetDataType;
        public:
            ANY_TO_ANY(PCResourceImpl * AssociatedResource,
                        PCDataType* TargetDataType) {
                __AssociatedResource = AssociatedResource;
                __TargetDataType = TargetDataType;

                switch(__TargetDataType->__DataTypeCategory){
                    case DataTypeCategory::BOOL :   __SfcName = "ANY_TO_BOOL";
                                                    break;
                    case DataTypeCategory::BYTE :  __SfcName = "ANY_TO_BYTE";
                                                    break; 
                    case DataTypeCategory::WORD :  __SfcName = "ANY_TO_WORD";
                                                    break; 
                    case DataTypeCategory::DWORD :  __SfcName = "ANY_TO_DWORD";
                                                    break;
                    case DataTypeCategory::LWORD :  __SfcName = "ANY_TO_LWORD";
                                                    break;
                    case DataTypeCategory::CHAR :  __SfcName = "ANY_TO_CHAR";
                                                    break; 
                    case DataTypeCategory::INT :   __SfcName = "ANY_TO_INT";
                                                    break; 
                    case DataTypeCategory::SINT :  __SfcName = "ANY_TO_SINT";
                                                    break; 
                    case DataTypeCategory::DINT :  __SfcName = "ANY_TO_DINT";
                                                    break; 
                    case DataTypeCategory::LINT :  __SfcName = "ANY_TO_LINT";
                                                    break; 
                    case DataTypeCategory::UINT :  __SfcName = "ANY_TO_UINT";
                                                    break; 
                    case DataTypeCategory::USINT :  __SfcName = "ANY_TO_USINT";
                                                    break;
                    case DataTypeCategory::UDINT :  __SfcName = "ANY_TO_UDINT";
                                                    break;
                    case DataTypeCategory::ULINT :  __SfcName = "ANY_TO_ULINT";
                                                    break;
                    case DataTypeCategory::REAL :   __SfcName = "ANY_TO_REAL";
                                                    break;
                    case DataTypeCategory::LREAL :   __SfcName = "ANY_TO_LREAL";
                                                    break;
                    case DataTypeCategory::DATE_AND_TIME: 
                                                    __SfcName = "ANY_TO_DT";
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
            void Execute(std::vector<PCVariable*>& Operands);
    };
}

#endif