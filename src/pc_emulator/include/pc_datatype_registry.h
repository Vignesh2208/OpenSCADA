#ifndef __PC_EMULATOR_INCLUDE_PC_DATATYPE_REGISTRY_H__
#define __PC_EMULATOR_INCLUDE_PC_DATATYPE_REGISTRY_H__

#include <iostream>
#include <unordered_map>

using namespace std;


namespace pc_emulator {
    class PCDataType;
    class PCConfiguration;

    class DataTypeRegistry {
        private:
            PCConfiguration * __configuration;
            std::unordered_map<std::string,  PCDataType*> __Registry;
        public:
            DataTypeRegistry(PCConfiguration* configuration) : 
                    __configuration(configuration) {};
            void RegisterDataType(string DataTypeName, PCDataType* DataType);
            PCDataType * GetDataType(string DataTypeName);
    };
}

#endif