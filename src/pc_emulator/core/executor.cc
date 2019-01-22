
#include <iostream>
#include <cstdint>
#include <cstring>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>

#include "pc_emulator/include/pc_configuration.h"
#include "pc_emulator/include/pc_resource.h"
#include "pc_emulator/include/executor.h"


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

        if (CheckActiveInterrupts()) {
            SaveCPURegisters();
            ServeActiveInterrupts();
            RestoreCPURegisters();
        }

        if (idx < 0)
            break;

    }
}

// returns -1 to exit executor
int Executor::RunInsn(InsnContainer& insn_container) {
    // this might create and run other executors as well
    return -1;
}

bool Executor::CheckActiveInterrupts() {
    return false;
}

void Executor::ServeActiveInterrupts() {

}

void Executor::SaveCPURegisters() {

}

void Executor::RestoreCPURegisters() {

}

void Executor::ResetCPURegisters() {

}