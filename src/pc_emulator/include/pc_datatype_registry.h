#ifndef __PC_EMULATOR_INCLUDE_PC_DATATYPE_REGISTRY_H__
#define __PC_EMULATOR_INCLUDE_PC_DATATYPE_REGISTRY_H__

#include <iostream>
#include <unordered_map>


#include "pc_datatype.h"

using namespace std;


namespace pc_emulator {
    class PCDataType;
    class PCConfiguration;

    //! Registers and tracks data types
    class DataTypeRegistry {
        private:
            PCConfiguration * __configuration; /*!< Associated configuration */
            std::unordered_map<std::string, std::unique_ptr<PCDataType>> 
                __Registry; /*!< A hashmap of resource name, data type obj */
        public:
            //!Constructor
            DataTypeRegistry(PCConfiguration* configuration) : 
                    __configuration(configuration) {};

            //!Register new data type
            void RegisterDataType(string DataTypeName, 
                                std::unique_ptr<PCDataType> DataType);

            //!Retrieves data type with the specified name
            PCDataType * GetDataType(string DataTypeName);

            //! Clean's up all registered data types
            void Cleanup();
    };
}

#endif