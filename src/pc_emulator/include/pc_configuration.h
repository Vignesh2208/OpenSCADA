#ifndef __PC_EMULATOR_INCLUDE_PC_CONFIGURATION_H__
#define __PC_EMULATOR_INCLUDE_PC_CONFIGURATION_H__
#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <unordered_map>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "pc_logger.h"
#include "pc_mem_unit.h"
#include "pc_datatype_registry.h"
#include "pc_resource_registry.h"
#include "task.h"
#include "configuration.h"
#include "src/pc_emulator/include/resource_manager.h"
#include "src/pc_emulator/proto/configuration.pb.h"
#include "src/pc_emulator/proto/system_specification.pb.h"

using namespace std;
using namespace pc_specification;
using namespace pc_system_specification;


namespace pc_emulator {
    class PCVariable;
    
    //! Specific implementation of the PCConfiguration abstract class
    class PCConfigurationImpl: public PCConfiguration {
        private:
            int __NumResources; /*!< Number of CPUs on this PLC */

            //! Register all resources
            void RegisterAllResources(); 

            //! Register all elementary data types
            void RegisterAllElementaryDataTypes();

            //! Initialize all global variables in the configuration
            void RegisterAllComplexDataTypes();

            //! Initialize all access paths and each resource specific POUs
            void InitializeAllPOUVariables();

            unordered_map<std::string, std::unique_ptr<ResourceManager>>
                __ResourceManagers;

            std::vector <std::thread> LaunchedResources;

        public:

            bool stop;

            bool enable_kronos;

            float rel_cpu_speed;

            long  per_round_inc_ns;

            //! Returns a variable pointing to the subfield of a global/directly rep field
            PCVariable * GetExternVariable(string NestedFieldName);

            //! Returns a POU defined in one of the resources with the specified name
            /*!
                \param PoUName  Name of the POU defined in one of the resources
                \return POU variable or nullptr if POU is not found in any of
                        the registered resources
            */            
            PCVariable * GetPOU(string PoUName);

            //! Returns a variable referred by an AccessPath
            /*!
                \param AccessPath   Name of the AccessPath specified in the 
                    ACCESS definition
                \return A variable pointing to the content which can be accessed
                    or nullptr if the AccessPath is not defined
            */
            PCVariable * GetAccessPathVariable(string AccessPath);

            //! Returns a registered data type with the specified data type name
            /*!
                \param DataTypeName Name of the data type
                \return A pointer to the data type of null if not found!
            */
            PCDataType * LookupDataType(string DataTypeName);

            //! Constructor
            /*!
                \param ConfigurationPath    Path to txt file specified in
                    configuration.proto format
                \param enable_kronos    Whether this PLC is run under virtual time
                    or not
            */
            PCConfigurationImpl(string ConfigurationPath, 
                bool enable_kronos,
                long per_round_inc_ns=1000000);


            //! Given RAM memory location details, returns a PCVariable pointing to that location
            /*!
                \param ByteOffset Byte offset within the specified memory
                \param BitOffset BitOffset within the specified memory. If 
                                 VariableDataTypeName is not BOOL, it is ignored
                \param VariableDataTypeName Initializes a variable of this datatype
                                            on the specified memory location
                \return The initialized variable is returned.
            */
            PCVariable * GetVariablePointerToMem(int ByteOffset, int BitOffset,
                        string VariableDataTypeName);


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
                        int MemType, int ByteOffset,
                        int BitOffset, string VariableDataTypeName);

            
            //! Launches a PLC and waits for completion
            void RunPLC();

            //! Launch a PLC
            void LaunchPLC();

            //! Wait for the specified run time to elapse
            void WaitForCompletion();

            // Schedule all resources to stop gracefully !
            void StopAllResources();
            //! Cleans up all resources and frees allocated memory
            void Cleanup();

    };
}

#endif