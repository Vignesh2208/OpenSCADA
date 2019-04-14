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

    class PCConfiguration {

        private:
            virtual void RegisterAllElementaryDataTypes() = 0;
            virtual void RegisterAllComplexDataTypes() = 0;
            virtual void RegisterAllResources() = 0;
        
        public:
            std::unique_ptr<Logger> PCLogger;
            std::unique_ptr<DataTypeRegistry> RegisteredDataTypes;
            ResourceRegistry * RegisteredResources;
            int __RAMmemSize;
            PCMemUnit __RAMMemory;
            string __ConfigurationPath;
            string __ConfigurationName;
            Specification __specification; 
            std::unique_ptr<PCVariable> __global_pou_var;
            std::unique_ptr<PCVariable> __access_pou_var;           
            std::unordered_map<int, string> __DataTypeDefaultInitialValues;
            std::unordered_map<string, std::unique_ptr<PCVariable>> 
                                                    __AccessedFields;
            
            PCConfiguration(void) {};
            ~PCConfiguration(void) {};
 
            virtual PCDataType * LookupDataType(string DataTypeName) = 0;
            virtual PCVariable * GetExternVariable(
                        string NestedFieldName)  = 0;
            virtual PCVariable * GetPOU(string PoUName) = 0;
            virtual PCVariable * GetVariablePointerToMem(int ByteOffset,
                int BitOffset, string VariableDataTypeName) = 0;
            virtual void Cleanup() = 0;

    };
}

#endif