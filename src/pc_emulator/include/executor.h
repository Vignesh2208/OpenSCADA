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
    class PCResourceImpl;
    class Task;

    class Executor {
        public :
            PCConfigurationImpl * __configuration;
            PCVariable * __ExecPoUVariable;
            PCResourceImpl* __AssociatedResource;
            PoUCodeContainer * __CodeContainer;
            bool __Initialized;
            Task* __AssociatedTask;
            PCVariable * __CR; // local copy of the current result register of
                               // the associated resource

            Executor(PCConfigurationImpl* configuration,
                    PCResourceImpl* AssociatedResource,
                    Task * AssociatedTask):
                __configuration(configuration),
                __ExecPoUVariable(nullptr), 
                __AssociatedResource(AssociatedResource),
                __CodeContainer(nullptr),
                __Initialized(false),
                __AssociatedTask(AssociatedTask) {
                    __CR = new PCVariable((PCConfiguration *)configuration,
                                (PCResource *) AssociatedResource,
                                "__CR", "BOOL");
                    __CR->AllocateAndInitialize();
                };

            void SetExecPoUVariable(PCVariable* ExecPoUVariable);
            void Run();
            void CleanUp();

            // returns idx of next instruction to execute;
            int RunInsn(InsnContainer& insn_container);
            void SaveCPURegisters();
            void RestoreCPURegisters();
            void ResetCPURegisters();
            
            
    };
}

#endif