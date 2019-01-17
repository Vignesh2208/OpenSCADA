#ifndef __PC_EMULATOR_INCLUDE_PC_CONFIGURATION_H__
#define __PC_EMULATOR_INCLUDE_PC_CONFIGURATION_H__
#include <iostream>
#include "pc_logger.h"
#include "pc_mem_unit.h"
#include "pc_datatype_registry.h"
#include "pc_resource_registry.h"

using namespace std;

namespace pc_emulator {
    class PCConfiguration {
        public:
            Logger * PCLogger;
            DataTypeRegistry RegisteredDataTypes;
            ResourceRegistry RegisteredResources;
            int __RAMmemSize;
            PCMemUnit __RAMMemory;
            string __ConfigurationPath;
            string __ConfigurationName;

            PCConfiguration(string ConfigurationName, string ConfigurationPath) :
                __ConfigurationPath(ConfigurationPath) {
                    PCLogger = new Logger(this, ExtractLogFile(),
                                        ExtractLogLevel());
                    __RAMmemSize = ExtractRAMSize();
                    assert(__RAMmemSize > 0);
                    __RAMMemory.AllocateStaticMemory(__RAMmemSize);
                    __ConfigurationName(ConfigurationName);
            };

            string ExtractLogFile();
            int ExtractLogLevel();
            int ExtractRAMSize();

            PCVariable * GetVariablePointerToMem(int MemType, int ByteOffset,
                        int BitOffset, string VariableDataTypeName); 
    };
}

#endif