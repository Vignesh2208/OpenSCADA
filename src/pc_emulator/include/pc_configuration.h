#ifndef __PC_EMULATOR_INCLUDE_PC_CONFIGURATION_H__
#define __PC_EMULATOR_INCLUDE_PC_CONFIGURATION_H__
#include <iostream>
#include "pc_logger.h"
#include "pc_datatype_registry.h"

using namespace std;

namespace pc_emulator {
    class PCConfiguration {
        public:
            Logger * PCLogger;
            DataTypeRegistry RegisteredDataTypes;
    };
}

#endif