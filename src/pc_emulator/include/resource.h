#ifndef __PC_EMULATOR_INCLUDE_RESOURCE_H__
#define __PC_EMULATOR_INCLUDE_RESOURCE_H__

#include <iostream>
#include <vector>
#include <climits>
#include <cstdlib>
#include <unordered_map>
#include <queue>
#include "pc_variable.h"
#include "pc_mem_unit.h"
#include "synchronized_queue.h"
#include "src/pc_emulator/proto/configuration.pb.h"

using namespace std;
using namespace pc_specification;

namespace pc_emulator {

    //! Abstract class definition of a resource
    class PCResource {        
        public :   
            
            int __InputMemSize; /*!< Input memory size of resource in bytes */
            int __OutputMemSize;   /*!< Output memory size of resource in bytes */
            PCMemUnit __InputMemory;    /*!< InputMemory container object */
            PCMemUnit __OutputMemory;   /*!< OutputMemory container object */
            std::unordered_map<std::string,std::unique_ptr<PCVariable>>  
                __ResourcePoUVars; /*!< A hasmap of POUName to POU Variables */
            std::unordered_map<std::string, std::unique_ptr<PCVariable>> 
                __AccessedFields; /*!< A hasmap of FieldName to Variable */
            string __ResourceName; /*!< Name of the resource */
            SynchronizedQueue<string> FromResourceManager; /* For virtual time */
            SynchronizedQueue<string> ToResourceManager; /* For virtual time */

            //! Default constructor
            PCResource(void) {};

            //! Default destructor
            ~PCResource(void) {};

            //! Initialize all POU variables
            /*!
                Implemntation specific action. Intended to initialize all
                resource POU's and ACCESS paths
            */
            virtual void InitializeAllPoUVars() = 0;

            //! Called on resource startup
            virtual void OnStartup() = 0;

            //! Returns a variable defined as global
            /*!
                Searches all global/access paths and returns a PCVariable
                pointing to the given nested field name
            */
            virtual PCVariable * GetExternVariable(string NestedFieldName) = 0;
            
            //! Given location details, returns a PCVariable pointing to that location
            /*!
                \param MemType  Memory Type (INPUT or OUTPUT or RAM given in pc_specification::MemTypes)
                \param ByteOffset Byte offset within the specified memory
                \param BitOffset BitOffset within the specified memory. If 
                                 VariableDataTypeName is not BOOL, it is ignored
                \param VariableDataTypeName Initializes a variable of this datatype
                                            on the specified memory location
                \return The initialized variable is returned.
            */
            virtual PCVariable * GetVariablePointerToMem(int MemType,
                int ByteOffset, int BitOffset, string VariableDataTypeName) = 0;

            //! Cleanup operations
            virtual void Cleanup() = 0;
    };
}

#endif