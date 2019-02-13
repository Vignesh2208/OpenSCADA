#ifndef __PC_EMULATOR_INCLUDE_PC_DATATYPE_REGISTRY_H__
#define __PC_EMULATOR_INCLUDE_PC_DATATYPE_REGISTRY_H__

#include <iostream>
#include <unordered_map>


#include "pc_datatype.h"

using namespace std;


namespace pc_emulator {
    class PCDataType;
    class PCConfiguration;

    class DataTypeRegistry {
        private:
            PCConfiguration * __configuration;
            std::unordered_map<std::string,
                    std::unique_ptr<PCDataType>> __Registry;
        public:
            DataTypeRegistry(PCConfiguration* configuration) : 
                    __configuration(configuration) {};
            void RegisterDataType(string DataTypeName, 
                                std::unique_ptr<PCDataType> DataType);
            PCDataType * GetDataType(string DataTypeName);

            void Cleanup();
    };
}

#endif