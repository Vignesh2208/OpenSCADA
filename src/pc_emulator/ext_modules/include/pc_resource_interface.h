#ifndef __PC_EMULATOR_INCLUDE_PC_RESOURCE_INTF_H__
#define __PC_EMULATOR_INCLUDE_PC_RESOURCE_INTF_H__

#include <iostream>
#include <vector>
#include <climits>
#include <cstdlib>
#include <unordered_map>
#include <queue>
#include "src/pc_emulator/include/pc_variable.h"
#include "src/pc_emulator/include/pc_mem_unit.h"
#include "src/pc_emulator/proto/configuration.pb.h"

using namespace std;
using namespace pc_specification;
using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldInterfaceType = pc_specification::FieldInterfaceType;

namespace pc_emulator {
    class PCConfigurationInterface;
    
    class PCResourceInterface: public PCResource {
        private:

            //! Returns a POU defined in the resource with the specified name
            /*!
                \param PoUName  Name of the POU defined in the resource
                \return POU variable or nullptr if POU is not found 
            */
            PCVariable * GetPoUVariable(string PoUName);
            //! Gets a global variable or a directly represented defined in a POU
            /*!
                \param NestedFieldName: May point to a subfield of a global variable or 
                    a directly represented variable defined inside one of the POUs
                    associated with this resource
                \return A variable pointing to this sub field
            */
            PCVariable * GetPOUGlobalVariable(string NestedFieldName);

            //! Registers a POU for internal book keeping
            void RegisterPoUVariable(string VariableName,
                            std::unique_ptr<PCVariable> Var);

            void InitializeAllSFBVars();
            
        public :
            string __ResourceName; /*!< Name of the resource*/
            PCConfigurationInterface * __configuration; /*!< Associated config
                interface */

            //! Constructor
            /*!
                \param configuration    Associated configuration object
                \param ResourceName Name of the resource
                \param InputMemSize Input memory size in bytes
                \param OutputMemSize    Output memory size in bytes
            */
            PCResourceInterface(PCConfigurationInterface * configuration,
                        string ResourceName, int InputMemSize,
                        int OutputMemSize);

            //! Register and initialize all POU variables defined within the resource
            void InitializeAllPoUVars();

            //! Called on resource start-up
            void OnStartup();

            //! Returns a variable pointing to the subfield of a global/directly rep field
            PCVariable * GetExternVariable(string NestedFieldName);


            PCVariable * GetPoUFieldVariable(string NestedPoUFieldName);

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
            PCVariable * GetVariablePointerToMem(int MemType,
                int ByteOffset, int BitOffset, string VariableDataTypeName);

            //! Cleans up allocated memory and temporay variables
            void Cleanup();

            ~PCResourceInterface() {};
    };

}

#endif