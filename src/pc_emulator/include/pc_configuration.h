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
#include "src/pc_emulator/proto/configuration.pb.h"

using namespace std;
using namespace pc_specification;

namespace pc_emulator {
    class PCVariable;
    
    class PCConfigurationImpl: public PCConfiguration {
        private:
            int __NumResources;
            void RegisterAllResources();
            void RegisterAllElementaryDataTypes();
            void RegisterAllComplexDataTypes();
            void InitializeAllPOUVariables();
        public:
            PCVariable * GetExternVariable(string NestedFieldName);
            PCVariable * GetPOU(string PoUName);
            PCVariable * GetAccessPathVariable(string AccessPath);
            PCDataType * LookupDataType(string DataTypeName);
            PCConfigurationImpl(string ConfigurationPath);
            PCVariable * GetVariablePointerToMem(int ByteOffset, int BitOffset,
                        string VariableDataTypeName);
            PCVariable * GetVariablePointerToResourceMem(string ResourceName,
                        int MemType, int ByteOffset,
                        int BitOffset, string VariableDataTypeName);
            void Cleanup();

    };
}

#endif