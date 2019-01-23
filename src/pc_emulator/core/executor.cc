
#include <iostream>
#include <cstdint>
#include <cstring>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>

#include "pc_emulator/include/pc_configuration.h"
#include "pc_emulator/include/pc_resource.h"
#include "pc_emulator/include/executor.h"
#include "pc_emulator/include/task.h"
#include "pc_emulator/include/utils.h"


using namespace std;
using namespace pc_emulator;

void Executor::SetExecPoUVariable(PCVariable* ExecPoUVariable) {
    __ExecPoUVariable = ExecPoUVariable;
    __CodeContainer 
            = __AssociatedResource->GetCodeContainer(
                ExecPoUVariable->__VariableDataType->__DataTypeName);
    if (!__CodeContainer) {
        __configuration->PCLogger->RaiseException("Error initializing executor"
                " cannot get required code container!");
    }

    if (!__ExecPoUVariable) {
        __configuration->PCLogger->RaiseException("Error initializing executor"
                " Exec PoU variable is null!");
    }
    __Initialized = true;
};

void Executor::Run() {
    assert(__Initialized);

    __ExecPoUVariable->OnExecutorStartup();
    auto nInsns = __CodeContainer->GetTotalNumberInsns();
    int idx = 0;

    while (true) {
        auto insn_container = __CodeContainer->GetInsn(idx);
        idx = RunInsn(*insn_container);
        SaveCPURegisters();
        if (__AssociatedTask->type == TaskType::INTERRUPT) {
            Task * EligibleTask = nullptr;
            do {
                EligibleTask = __AssociatedResource->GetInterruptTaskToExecute();
                if (EligibleTask != nullptr
                    && EligibleTask->__priority < __AssociatedTask->__priority) {
                    EligibleTask->Execute();
                }

            } while(EligibleTask != nullptr);
            
            
        } else {
            Task * EligibleTask = nullptr;
            do {
                EligibleTask = __AssociatedResource->GetInterruptTaskToExecute();
                if (EligibleTask != nullptr)
                    EligibleTask->Execute();

            } while(EligibleTask != nullptr);

            do {
                EligibleTask 
                    = __AssociatedResource->GetIntervalTaskToExecuteAt(
                        __AssociatedResource->clock->GetCurrentTime());
                if (EligibleTask != nullptr) {
                    EligibleTask->Execute();
                    EligibleTask->__nxt_schedule_time_ms 
                                += EligibleTask->__interval_ms;
                    __AssociatedResource->QueueTask(EligibleTask); //requeue   
                }

            } while(EligibleTask != nullptr);

        }

        RestoreCPURegisters();
        if (idx < 0)
            break;

    }
}

// returns -1 to exit executor
int Executor::RunInsn(InsnContainer& insn_container) {
    // this might create and run other executors as well

    // update clock here as well.
    //__AssociatedResource->clock->UpdateCurrentTime(1);
    return -1;
}


void Executor::SaveCPURegisters() {

}

void Executor::RestoreCPURegisters() {

}

void Executor::ResetCPURegisters() {

}