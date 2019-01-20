#ifndef __PC_EMULATOR_INCLUDE_PC_CONFIGURATION_H__
#define __PC_EMULATOR_INCLUDE_PC_CONFIGURATION_H__
#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "pc_logger.h"
#include "pc_mem_unit.h"
#include "pc_datatype_registry.h"
#include "pc_resource_registry.h"
#include "pc_emulator/proto/configuration.pb.h"

using namespace std;
using namespace pc_specification;

namespace pc_emulator {
    class PCVariable;
    
    class PCConfiguration {
        private :
            void RegisterAllResources();
            void RegisterAllElementaryDataTypes();
            void RegisterAllComplexDataTypes();
            PCVariable * GetVariable(string NestedFieldName);
            

        public:
            Logger * PCLogger;
            DataTypeRegistry RegisteredDataTypes;
            ResourceRegistry RegisteredResources;
            int __RAMmemSize;
            PCMemUnit __RAMMemory;
            string __ConfigurationPath;
            string __ConfigurationName;
            Specification __specification;
            PCVariable * __global_pou_var;
            PCVariable * __access_pou_var;
            int __NumResources;

            
            
            PCVariable * AccessVariable(string NestedFieldName);
            PCConfiguration(string ConfigurationPath);
            PCVariable * GetVariablePointerToMem(int MemType, int ByteOffset,
                        int BitOffset, string VariableDataTypeName); 
    };
}

#endif