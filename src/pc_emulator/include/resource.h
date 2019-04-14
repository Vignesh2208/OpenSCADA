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
#include "src/pc_emulator/proto/configuration.pb.h"

using namespace std;
using namespace pc_specification;

namespace pc_emulator {

    class PCResource {        
        public :   
            
            int __InputMemSize;
            int __OutputMemSize;
            PCMemUnit __InputMemory;
            PCMemUnit __OutputMemory;
            std::unordered_map<std::string,  std::unique_ptr<PCVariable>> 
                                                __ResourcePoUVars;
            std::unordered_map<std::string, std::unique_ptr<PCVariable>> 
                                                __AccessedFields;
            string __ResourceName;

            PCResource(void) {};
            ~PCResource(void) {};
            virtual void InitializeAllPoUVars() = 0;
            virtual void OnStartup() = 0;
            virtual PCVariable * GetExternVariable(string NestedFieldName) = 0;
            
            virtual PCVariable * GetVariablePointerToMem(int MemType,
                int ByteOffset, int BitOffset, string VariableDataTypeName) = 0;
            virtual void Cleanup() = 0;
    };
}

#endif