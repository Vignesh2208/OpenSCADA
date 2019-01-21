#ifndef __PC_EMULATOR_INCLUDE_EXECUTOR_H__
#define __PC_EMULATOR_INCLUDE_EXECUTOR_H__

using namespace std;

#include <iostream>
#include <vector>
#include <climits>
#include <cstdlib>
#include <unordered_map>
#include "pc_variable.h"

namespace pc_emulator {
    class PCConfiguration;

    class Executor {
        public :
            PCConfiguration * __configuration;
            PCVariable * __ExecPoUVariable;
            string __ResourceName;
            bool __Initialized;

            Executor(PCConfiguration* configuration, string ResourceName):
                __configuration(configuration), __ResourceName(ResourceName),
                __ExecPoUVariable(nullptr), __Initialized(false) {};

            void SetExecPoUVariable(PCVariable* ExecPoUVariable) {
                __ExecPoUVariable = ExecPoUVariable;
                __Initialized = true;
            };


            
            
    };
}

#endif