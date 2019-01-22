#ifndef __PC_EMULATOR_INCLUDE_EXECUTOR_H__
#define __PC_EMULATOR_INCLUDE_EXECUTOR_H__

using namespace std;

#include <iostream>
#include <vector>
#include <climits>
#include <cstdlib>
#include <unordered_map>
#include "pc_variable.h"
#include "pc_pou_code_container.h"

namespace pc_emulator {
    class PCConfiguration;
    class PCResource;

    class Executor {
        public :
            PCConfiguration * __configuration;
            PCVariable * __ExecPoUVariable;
            PCResource* __AssociatedResource;
            PoUCodeContainer * __CodeContainer;
            bool __Initialized;

            Executor(PCConfiguration* configuration,
                                PCResource * AssociatedResource):
                __configuration(configuration), 
                __AssociatedResource(AssociatedResource),
                __ExecPoUVariable(nullptr),
                __CodeContainer(nullptr), __Initialized(false) {};

            void SetExecPoUVariable(PCVariable* ExecPoUVariable);
            void Run();

            // returns idx of next instruction to execute;
            int RunInsn(InsnContainer& insn_container);
            bool CheckActiveInterrupts();
            void ServeActiveInterrupts();
            void SaveCPURegisters();
            void RestoreCPURegisters();
            void ResetCPURegisters();
            
            
    };
}

#endif