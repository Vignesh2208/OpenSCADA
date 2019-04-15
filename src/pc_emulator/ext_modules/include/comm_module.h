#ifndef __PC_EMULATOR_INCLUDE_PC_COMM_MODULE_H__
#define __PC_EMULATOR_INCLUDE_PC_COMM_MODULE_H__
#include "ext_module_intf.h"

using namespace std;
using namespace pc_specification;


namespace pc_emulator {

    // A generic communication module interface
    class CommModule: public ExtModule {
        public:

            //! Constructoe
            CommModule(string ConfigurationPath) 
                : ExtModule(ConfigurationPath) {};

            //! NOT IMPLEMENTED, Communication modules can only query access paths. 
            /*!
                Raises an exception.
            */
            std::unique_ptr<PCVariableContainer> GetVariableContainer(
                int RamByteOffset, int RamBitOffset,
                string VariableDataTypeName) {
                std::domain_error("NOT IMPLEMENTED !");
            };

            //! NOT IMPLEMENTED, Communication modules can only query access paths. 
            /*!
                Raises an exception.
            */
            std::unique_ptr<PCVariableContainer> GetVariableContainer(
                string ResourceName, int MemType, 
                int ByteOffset, int BitOffset, string VariableDataTypeName) {
                std::domain_error("NOT IMPLEMENTED !");
            };

            //! Returns a variable container for a given nested access path
            /*!
                \param NestedAccessPath May be a subfield of an access variable
                \return VariableContainer or nullptr if the NestedAccessPath
                    is not found!
            */
            std::unique_ptr<PCVariableContainer>
                GetVariableContainer(string NestedAccessPath);
    };
}

#endif