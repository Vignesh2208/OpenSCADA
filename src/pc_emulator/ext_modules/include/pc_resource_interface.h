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
            PCVariable * GetPoUVariable(string PoUName);
            PCVariable * GetPOUGlobalVariable(string NestedFieldName);
            void RegisterPoUVariable(string VariableName,
                            std::unique_ptr<PCVariable> Var);
        public :
            string __ResourceName;
            PCConfigurationInterface * __configuration;
            PCResourceInterface(PCConfigurationInterface * configuration,
                        string ResourceName, int InputMemSize,
                        int OutputMemSize);

            void InitializeAllPoUVars();
            void OnStartup();
            PCVariable * GetExternVariable(string NestedFieldName);    
            PCVariable * GetPoUFieldVariable(string NestedPoUFieldName);       
            PCVariable * GetVariablePointerToMem(int MemType,
                int ByteOffset, int BitOffset, string VariableDataTypeName);
            void Cleanup();
    };

}

#endif