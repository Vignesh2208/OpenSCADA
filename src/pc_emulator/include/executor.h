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

    // Holds a PoU Variable and executes its instructions
    class Executor {
        public :
            PCConfigurationImpl * __configuration; /*!< Associated configuration */
            PCVariable * __ExecPoUVariable; /*!< PoU to be executed */
            PCResourceImpl* __AssociatedResource;   /*!< Associated resource */
            PoUCodeContainer * __CodeContainer; /*!< Instructions of the PoU */
            bool __Initialized; /*!< Set to true after a PoU is assigned to this
                executor */
            Task* __AssociatedTask; /*!< Task associated with this executor */

            //!Constructor
            Executor(PCConfigurationImpl* configuration,
                    PCResourceImpl* AssociatedResource,
                    Task * AssociatedTask):
                __configuration(configuration),
                __ExecPoUVariable(nullptr), 
                __AssociatedResource(AssociatedResource),
                __CodeContainer(nullptr),
                __Initialized(false),
                __AssociatedTask(AssociatedTask) {
                };

            //! Sets a PoU to this executor
            /*!
                \param ExecPoUVariable PoU to be assigned to this executor
            */
            void SetExecPoUVariable(PCVariable* ExecPoUVariable);

            //! Runs the executor. Starts running each instruction
            void Run();

            //! Cleanup and do other book keeping
            void CleanUp();

            //! Runs the current instruction and returns idx of next instruction to execute
            /*!
                \param insn_container Current instruction to execute
                \return idx of next instruction to execute
            */
            int RunInsn(InsnContainer& insn_container);

            //! Returns true if the CurrentResult register of the associated task is TRUE
            bool IsCRSet();
            
            //! Saves the current result register of the resource
            void SaveCPURegisters();

            //! Restores the current result register of the resource
            void RestoreCPURegisters();

            //! Resets the current result register of the resource
            void ResetCPURegisters();
            
            
    };
}

#endif