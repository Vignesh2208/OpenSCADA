#ifndef __PC_EMULATOR_INCLUDE_PC_RESOURCE_H__
#define __PC_EMULATOR_INCLUDE_PC_RESOURCE_H__

#include <iostream>
#include <vector>
#include <climits>
#include <cstdlib>
#include <unordered_map>
#include "pc_variable.h"
#include "pc_mem_unit.h"
#include "pc_emulator/proto/configuration.pb.h"

using namespace std;

namespace pc_emulator {
    class PCConfiguration;

    class PCResource {
        private:
            PCConfiguration * __configuration;
            string __ResourceName;
            int __InputMemSize;
            int __OutputMemSize;
            PCMemUnit __InputMemory;
            PCMemUnit __OutputMemory;
            std::unordered_map<std::string,  PCVariable*> __ResourcePoUVars;
            
            void InitializeAllPoUVars();

        public :
            PCResource(PCConfiguration * configuration, 
                string ResourceName, int InputMemSize, int OutputMemSize):
                __configuration(configuration), __ResourceName(ResourceName),
                __InputMemSize(InputMemSize), __OutputMemSize(OutputMemSize) {

                    assert(__InputMemSize > 0 && __OutputMemSize > 0);
                    __InputMemory.AllocateStaticMemory(__InputMemSize);
                    __OutputMemory.AllocateStaticMemory(__OutputMemSize);

            }

            void RegisterPoUVariable(string VariableName, PCVariable * Var);
            PCVariable * GetVariable(string NestedFieldName);
            PCVariable * GetVariablePointerToMem(int MemType, int ByteOffset,
                                int BitOffset, string VariableDataTypeName);
    };

}

#endif