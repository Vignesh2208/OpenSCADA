#ifndef __PC_EMULATOR_INCLUDE_PC_SENSOR_MODULE_H__
#define __PC_EMULATOR_INCLUDE_PC_SENSOR_MODULE_H__
#include "ext_module_intf.h"

using namespace std;
using namespace pc_specification;


namespace pc_emulator {

    // A generic sensor interface
    class SensorModule: public ExtModule {
        public:

            //! Constructor
            SensorModule(string ConfigurationPath) 
                : ExtModule(ConfigurationPath) {};


            //! NOT IMPLEMENTED, Sensor modules cannot query RAM memory. 
            /*!
                Raises an exception.
            */
            std::unique_ptr<PCVariableContainer> GetVariableContainer(
                int RamByteOffset, int RamBitOffset,
                string VariableDataTypeName) {
                std::domain_error("NOT IMPLEMENTED !");
                return nullptr;
            };
            
            //! NOT IMPLEMENTED, Sensor modules cannot query access paths. 
            /*!
                Raises an exception.
            */
            std::unique_ptr<PCVariableContainer>
                GetVariableContainer(string AccessPath) {
                std::domain_error("NOT IMPLEMENTED !");
                return nullptr;
            };


            //! Returns a variable container for the specified resource mem location
            /*!
                Raises an exception if MemType is not INPUT_MEM
            */
            std::unique_ptr<PCVariableContainer> GetVariableContainer(
                string ResourceName, int MemType, int ByteOffset, int BitOffset,
                string VariableDataTypeName);

            
            

    };

    
}

#endif