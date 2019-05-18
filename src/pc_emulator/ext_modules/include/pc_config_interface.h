#ifndef __PC_EMULATOR_INCLUDE_PC_CONFIG_INTERFACE_H__
#define __PC_EMULATOR_INCLUDE_PC_CONFIG_INTERFACE_H__
#include <iostream>
#include <memory>
#include <fcntl.h>
#include <fstream>
#include <unordered_map>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "src/pc_emulator/include/pc_logger.h"
#include "src/pc_emulator/include/pc_mem_unit.h"
#include "src/pc_emulator/include/pc_datatype.h"
#include "src/pc_emulator/include/pc_variable.h"
#include "src/pc_emulator/include/configuration.h"
#include "src/pc_emulator/proto/configuration.pb.h"

using namespace std;
using namespace pc_specification;

namespace pc_emulator {

    class PCConfigurationInterface: public PCConfiguration {
        private:
            int __NumResources; /*!< Num resource specified in the configuration */
            unordered_map<string, int>
                __AccessPathFieldQualifier; /*!< Hashmap of access path name
                    and READ_ONLY/READ_WRITE qualifiers */

            //! Register all elementary data types
            void RegisterAllElementaryDataTypes();

            //! Initialize all global variables in the configuration
            void RegisterAllComplexDataTypes();

            //! Register all resources
            void RegisterAllResources();
        public: 

            //!Constructor
            PCConfigurationInterface(std::string ConfigurationPath);


            //! Returns a registered data type with the specified data type name
            /*!
                \param DataTypeName Name of the data type
                \return A pointer to the data type of null if not found!
            */
            PCDataType * LookupDataType(string DataTypeName);

            //! Returns a variable pointing to the subfield of a global/directly rep field
            PCVariable * GetExternVariable(string NestedFieldName);

            //! Given RAM memory location details, returns a PCVariable pointing to that location
            /*!
                \param ByteOffset Byte offset within the specified memory
                \param BitOffset BitOffset within the specified memory. If 
                                 VariableDataTypeName is not BOOL, it is ignored
                \param VariableDataTypeName Initializes a variable of this datatype
                                            on the specified memory location
                \return The initialized variable is returned.
            */
            PCVariable * GetVariablePointerToMem(int ByteOffset,
                int BitOffset, string VariableDataTypeName);

            
            //! Given location details, returns a PCVariable pointing to that location
            /*!
                \param ResourceName Name of the resource to check. If MemType is RAM,
                        then it is ignored.
                \param MemType  Memory Type (INPUT or OUTPUT or RAM given in pc_specification::MemTypes)
                \param ByteOffset Byte offset within the specified memory
                \param BitOffset BitOffset within the specified memory. If 
                                 VariableDataTypeName is not BOOL, it is ignored
                \param VariableDataTypeName Initializes a variable of this datatype
                                            on the specified memory location
                \return The initialized variable is returned.
            */
            PCVariable * GetVariablePointerToResourceMem(string ResourceName,
                int MemType, int ByteOffset, int BitOffset,
                string VariableDataTypeName);

            //! GetPOU is not implemented for this config_interface
            PCVariable * GetPOU(string PoUName) {
                std::domain_error("NOT IMPLEMENTED !");
            };

            //! Cleans up all resources and frees allocated memory
            void Cleanup();    

            ~PCConfigurationInterface() {};

    };        
}

#endif