#ifndef __PC_EMULATOR_INCLUDE_PC_EXT_INTERFACE_GRPC_H__
#define __PC_EMULATOR_INCLUDE_PC_EXT_INTERFACE_GRPC_H__
#include "src/pc_emulator/ext_modules/include/ext_module_intf.h"

using namespace std;
using namespace pc_specification;


namespace pc_emulator {

    class ExtInterfaceAPI {
        public:

            PCConfigurationInterface __ConfigInterface; /*!< Associated Config
                Interface */

            //! Constructor
            ExtInterfaceAPI(string ConfigurationPath) 
                : __ConfigInterface(ConfigurationPath) {};
                
            void SetSensorInput(
                string ResourceName, int MemType, int ByteOffset, int BitOffset,
                string VariableDataTypeName, string Value);
                
            string GetActuatorOutput(
                string ResourceName, int MemType, int ByteOffset, int BitOffset,
                string VariableDataTypeName);
    };

}

#endif