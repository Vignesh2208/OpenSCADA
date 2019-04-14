#ifndef __PC_EMULATOR_INCLUDE_PC_CONFIG_INTERFACE_H__
#define __PC_EMULATOR_INCLUDE_PC_CONFIG_INTERFACE_H__
#include <iostream>
#include <memory>
#include <fcntl.h>
#include <fstream>
#include <unordered_map>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "src/pc_emulator/include/pc_logger.h"
#include "src/pc_emulator/include/pc_mem_unit.h"
#include "src/pc_emulator/include/pc_datatype.h"
#include "src/pc_emulator/include/pc_variable.h"
#include "src/pc_emulator/include/configuration.h"
#include "src/pc_emulator/proto/configuration.pb.h"

using namespace std;
using namespace pc_specification;

namespace pc_emulator {

    class PCConfigurationInterface: public PCConfiguration {
        private:
            int __NumResources;
            unordered_map<string, int> __AccessPathFieldQualifier;
            void RegisterAllElementaryDataTypes();
            void RegisterAllComplexDataTypes();
            void RegisterAllResources();
        public: 
            PCConfigurationInterface(std::string ConfigurationPath);
            PCDataType * LookupDataType(string DataTypeName);
            PCVariable * GetExternVariable(string NestedFieldName);
            PCVariable * GetVariablePointerToMem(int ByteOffset,
                int BitOffset, string VariableDataTypeName);
            PCVariable * GetVariablePointerToResourceMem(string ResourceName,
                int MemType, int ByteOffset, int BitOffset,
                string VariableDataTypeName);
            PCVariable * GetPOU(string PoUName) {
                std::domain_error("NOT IMPLEMENTED !");
            };
            void Cleanup();    


    };        
}

#endif