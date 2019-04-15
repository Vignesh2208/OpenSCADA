#ifndef __PC_EMULATOR_INCLUDE_CONFIGURATION_H__
#define __PC_EMULATOR_INCLUDE_CONFIGURATION_H__
#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <unordered_map>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <stdexcept>

#include "pc_logger.h"
#include "pc_mem_unit.h"
#include "pc_datatype_registry.h"
#include "pc_resource_registry.h"
#include "src/pc_emulator/proto/configuration.pb.h"

using namespace std;
using namespace pc_specification;

namespace pc_emulator {
    class PCVariable;
    class ResourceRegistry;

    //! Abstract programmable controller config specification
    class PCConfiguration {

        private:

            //! Should register all elementary data types
            virtual void RegisterAllElementaryDataTypes() = 0;

            //! Should initialize all global variables in the configuration
            virtual void RegisterAllComplexDataTypes() = 0;

            //! Should initialize all access paths and each resource specific POUs
            virtual void RegisterAllResources() = 0;
        
        public:
            std::unique_ptr<Logger> PCLogger; /*!< For logging messages */
            std::unique_ptr<DataTypeRegistry> 
                RegisteredDataTypes; /*!< Stores registered data types */
            ResourceRegistry * RegisteredResources; /*!< Stores registered resources */
            int __RAMmemSize; /*!< Ram memory size in bytes */
            PCMemUnit __RAMMemory; /*!< Ram memory unit */
            string __ConfigurationPath; /*!< Path containing configuration specification
                prototxt */
            string __ConfigurationName; /*!< Name of the configuration */
            Specification __specification;  /*!< Specification proto object */
            std::unique_ptr<PCVariable> 
                __global_pou_var; /*!< Configuration global variable */
            std::unique_ptr<PCVariable> 
                __access_pou_var; /*!< Configuration access variable */          
            std::unordered_map<int, string> 
                __DataTypeDefaultInitialValues; /*!< Default initial values of
                all elementary data types */
            std::unordered_map<string, std::unique_ptr<PCVariable>> 
                 __AccessedFields; /*!< For book keeping */
            
            //! Constructor
            PCConfiguration(void) {};

            //! Destructor
            ~PCConfiguration(void) {};
 

            //! Should return a registered data type with the specified data type name
            /*!
                \param DataTypeName Name of the data type
                \return A pointer to the data type of null if not found!
            */
            virtual PCDataType * LookupDataType(string DataTypeName) = 0;

            //! Should return a variable pointing to the subfield of a global/directly rep field
            virtual PCVariable * GetExternVariable(
                        string NestedFieldName)  = 0;

            //! Should return a POU defined in one of the resources with the specified name
            /*!
                \param PoUName  Name of the POU defined in one of the resources
                \return POU variable or nullptr if POU is not found in any of
                        the registered resources
            */  
            virtual PCVariable * GetPOU(string PoUName) = 0;

            //! Given RAM memory location details, should return a PCVariable pointing to that location
            /*!
                \param ByteOffset Byte offset within the specified memory
                \param BitOffset BitOffset within the specified memory. If 
                                 VariableDataTypeName is not BOOL, it is ignored
                \param VariableDataTypeName Initializes a variable of this datatype
                                            on the specified memory location
                \return The initialized variable is should be returned.
            */
            virtual PCVariable * GetVariablePointerToMem(int ByteOffset,
                int BitOffset, string VariableDataTypeName) = 0;


            //! Should clean up all resources and free allocated memory
            virtual void Cleanup() = 0;

    };
}

#endif