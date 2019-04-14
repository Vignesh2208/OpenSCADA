#ifndef __PC_EMULATOR_INCLUDE_PC_SENSOR_MODULE_H__
#define __PC_EMULATOR_INCLUDE_PC_SENSOR_MODULE_H__
#include "ext_module_intf.h"

using namespace std;
using namespace pc_specification;


namespace pc_emulator {

    class SensorModule: public ExtModule {
        public:
            SensorModule(string ConfigurationPath) 
                : ExtModule(ConfigurationPath) {};

            std::unique_ptr<PCVariableContainer> GetVariableContainer(
                int RamByteOffset, int RamBitOffset,
                string VariableDataTypeName) {
                std::domain_error("NOT IMPLEMENTED !");
            };
            
            std::unique_ptr<PCVariableContainer>
                GetVariableContainer(string AccessPath) {
                std::domain_error("NOT IMPLEMENTED !");
            };

            std::unique_ptr<PCVariableContainer> GetVariableContainer(
                string ResourceName, int MemType, int ByteOffset, int BitOffset,
                string VariableDataTypeName);

    };
}

#endif