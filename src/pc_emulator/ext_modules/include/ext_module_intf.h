#ifndef __PC_EMULATOR_INCLUDE_PC_EXT_MODULE_H__
#define __PC_EMULATOR_INCLUDE_PC_EXT_MODULE_H__
#include "pc_config_interface.h"
#define SUCCESS 1
#define PERM_DENIED -1
#define ERROR_NO_MEM -2
using namespace std;
using namespace pc_specification;


namespace pc_emulator {

    // A helper class which restricts variable API
    class PCVariableContainer {
        private:
            PCVariable * __AssociatedVariable; /*!< Associated Variable */
        public:

            //!Constructor
            PCVariableContainer(PCVariable * AssociatedVariable):
                __AssociatedVariable(AssociatedVariable){};

            //! Returns the data type of the nested field
            /*!
                \param NestedFieldName  The field whose data type is to be returned
                \return PCDataType  A pointer to the data type of the field
            */
            PCDataType * GetDataTypeOfField(string NestedFieldName) {
                DataTypeFieldAttributes FieldAttributes;
                __AssociatedVariable->GetFieldAttributes(NestedFieldName, 
                            FieldAttributes);
                return FieldAttributes.FieldDetails.__FieldTypePtr;
            }

            
            //! Returns the current value stored at a nested field
            /*!
                Can only be used when the data type of the nested field is one
                of the elementary ones.
                \param NestedFieldName: The field whose content is to be returned
                \param CategoryOfDataType: One of pc_specification::DataTypeCategory
            */
            template <typename T> T GetValueStoredAtField(string NestedFieldName,
                                            int CategoryOfDataType) {
                return __AssociatedVariable->GetValueStoredAtField<T>(
                        NestedFieldName, CategoryOfDataType);
            }

            //! Sets the value to the specified nested field
            /*!
                Copies bytes from value into the memory location corresponding
                to the nested field. If the nested field is a pointer, then
                the "value" itself is copied to the memory location

                \param NestedFieldName: A nested field name which needs to be set
                \param value: Buffer containing data to be copied over
                \param CopySizeBytes: Size of data to be copied
                \return PERM_DENIED if the variable is READ_ONLY
                        ERROR_NO_MEM if CopySizeBytes exceeds variable size
                        SUCCESS
            */
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

            //! Sets  value to the specified nested field
            /*!
                Accepts value as a string and sets it to the specified field.
                Can only be called when the datatype of the nested field is an
                elementary data type. 

                \param value A string which is in a specific format for each
                            elementary data type
                \param NestedFieldName: A nested field name whose value needs
                            to be set. Raises an exception of the data type of
                            the nested field is not elementary
                \return PERM_DENIED if the variable is READ_ONLY
                        or SUCCESS
            */
            int SetField(string NestedFieldName, string value) {
                if (__AssociatedVariable->__VariableAttributes.
                    FieldDetails.__FieldQualifier 
                        == pc_specification::FieldQualifiers::READ_ONLY) {
                    return PERM_DENIED;
                }
                
                __AssociatedVariable->SetField(NestedFieldName, value);
                return SUCCESS;
            }
    };

    //! External module interface
    class ExtModule {
        protected:

            PCConfigurationInterface __ConfigInterface; /*!< Associated Config
                Interface */

        public:

            //! Constructor
            ExtModule(string ConfigurationPath)
                : __ConfigInterface(ConfigurationPath) {};
            
            //! Returns datatype with the specified name
            /*!
                \param DataTypeName Name of the data type
                \return PCDataType object or nullptr if the data type is not found !
            */
            PCDataType * GetDataType(string DataTypeName) {
                return __ConfigInterface.LookupDataType(DataTypeName);
            };

            //!Should returns a VariableContainer for the specified access path
            /*!
                \param AccessPath   AccessPath name (Cannot be a nested field)
                \return VariableContainer or nullptr if AccessPath is not found! */
            virtual std::unique_ptr<PCVariableContainer>
                        GetVariableContainer(string AccessPath) =0;

            
            //! Should return a VariableContainer to the specified RAM location
            /*!
                \param RamByteOffset    Byte of the RAM
                \param RamBitOffset     Bit offset within the byte
                \param  VariableDataTypeName    Initializes the Variable Container
                    to hold a variable of this type
                \return A variable container pointing to the specified memory location
            */
            virtual std::unique_ptr<PCVariableContainer> 
                        GetVariableContainer(int RamByteOffset,
                            int RamBitOffset, string VariableDataTypeName) = 0;

            //! Should return a VariableContainer to the specified location
            /*!
                \param ResourceName Name of the resource
                \param MemType  INPUT or OUTPUT. If specified as RAM, then ResourceName is ignored
                \param ByteOffset    Byte of the memory
                \param BitOffset     Bit offset within the byte
                \param  VariableDataTypeName    Initializes the Variable Container
                    to hold a variable of this type
                \return A variable container pointing to the specified memory location
            */
            virtual std::unique_ptr<PCVariableContainer>
                 GetVariableContainer(string ResourceName, int MemType, 
                int ByteOffset, int BitOffset, string VariableDataTypeName) = 0;

            //!Cleans up the configuration interface object
            void Cleanup() { __ConfigInterface.Cleanup();};

    };
}

#endif