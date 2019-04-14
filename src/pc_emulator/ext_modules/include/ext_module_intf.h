#ifndef __PC_EMULATOR_INCLUDE_PC_EXT_MODULE_H__
#define __PC_EMULATOR_INCLUDE_PC_EXT_MODULE_H__
#include "pc_config_interface.h"
#define SUCCESS 1
#define PERM_DENIED -1
#define ERROR_NO_MEM -2
using namespace std;
using namespace pc_specification;


namespace pc_emulator {
    class PCVariableContainer {
        private:
            PCVariable * __AssociatedVariable;
        public:
            PCVariableContainer(PCVariable * AssociatedVariable):
                __AssociatedVariable(AssociatedVariable){};
            template <typename T> T GetValueStoredAtField(string NestedFieldName,
                                            int CategoryOfDataType) {
                return __AssociatedVariable->GetValueStoredAtField<T>(
                        NestedFieldName, CategoryOfDataType);
            }
            int SetField(string NestedFieldName, void * value,
                            int CopySizeBytes) {
                if (__AssociatedVariable->__VariableAttributes.
                    FieldDetails.__FieldQualifier 
                        == pc_specification::FieldQualifiers::READ_ONLY) {
                    return PERM_DENIED;
                }

                if (!value || (CopySizeBytes 
                    > __AssociatedVariable->__TotalSizeInBits /8)) {
                    return ERROR_NO_MEM;
                }
                
                __AssociatedVariable->SetField(NestedFieldName, value,
                        CopySizeBytes);
                return SUCCESS;
            }
    };

    class ExtModule {
        protected:
            PCConfigurationInterface __ConfigInterface;

        public:
            ExtModule(string ConfigurationPath)
                : __ConfigInterface(ConfigurationPath) {};
            
            PCDataType * GetDataType(string DataTypeName) {
                return __ConfigInterface.LookupDataType(DataTypeName);
            };
            virtual std::unique_ptr<PCVariableContainer>
                        GetVariableContainer(string AccessPath) =0;
            virtual std::unique_ptr<PCVariableContainer> 
                        GetVariableContainer(int RamByteOffset,
                            int RamBitOffset, string VariableDataTypeName) = 0;
            virtual std::unique_ptr<PCVariableContainer>
                 GetVariableContainer(string ResourceName, int MemType, 
                int ByteOffset, int BitOffset, string VariableDataTypeName) = 0;

            void Cleanup() { __ConfigInterface.Cleanup();};
    };
}

#endif