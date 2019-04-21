
#include <iostream>
#include <cstdint>
#include <cstring>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_resource.h"
#include "src/pc_emulator/include/executor.h"
#include "src/pc_emulator/include/task.h"
#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/include/sfb_registry.h"
#include "src/pc_emulator/include/functions_registry.h"

using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldInterfaceType = pc_specification::FieldInterfaceType;


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

    __AssociatedTask->__Executing = true;
    __ExecPoUVariable->OnExecutorStartup();
    int idx = 0;
    while (true) {
        auto insn_container = __CodeContainer->GetInsn(idx);
        if (!insn_container || insn_container->InsnName 
            == __ExecPoUVariable->__VariableDataType->__DataTypeName) {
            // This is a SFB without a code body
            assert(__ExecPoUVariable->__VariableDataType->__PoUType
                == PoUType::FB);

            auto SFB = __AssociatedResource->__SFBRegistry->GetSFB(
                insn_container->InsnName);

            if (SFB == nullptr) {
                __AssociatedResource->__configuration->PCLogger->RaiseException(
                    "SFB: " + insn_container->InsnName + " not found!");
            }

            SFB->Execute(__ExecPoUVariable);
            idx = -1;
        } else {
            idx = RunInsn(*insn_container);
        }

        Task * EligibleTask = nullptr;
        do {
            EligibleTask = __AssociatedResource->GetInterruptTaskToExecute();
            if (EligibleTask != nullptr
                && EligibleTask->__priority < __AssociatedTask->__priority) {
                if (!EligibleTask->__Executing) {
                    //__AssociatedTask->__configuration->PCLogger->RaiseException(
                    //    "Attempting to re-execute an already executing task!"
                    //);
                
                    SaveCPURegisters();
                    EligibleTask->Execute();
                    RestoreCPURegisters();
                }
            }

        } while(EligibleTask != nullptr);

        if (__AssociatedTask->type == TaskType::INTERRUPT) {
            EligibleTask = __AssociatedResource->GetIntervalTaskToExecuteAt(
                __AssociatedResource->clock->GetCurrentTime());

            if (EligibleTask != nullptr && !EligibleTask->__Executing
                && EligibleTask->__priority < __AssociatedTask->__priority) {
                SaveCPURegisters();
                EligibleTask->Execute();
                RestoreCPURegisters();    
            }
        }

        if (idx < 0 || idx >= __CodeContainer->__InsnCount);
            break;
    }
    __AssociatedTask->__Executing = false;
}

void Executor::CleanUp() {
}

// returns -1 to exit executor
int Executor::RunInsn(InsnContainer& insn_container) {
    // this might create and run other executors as well

    // update clock here as well.
    //__AssociatedResource->clock->UpdateCurrentTime(1);
    if (insn_container.InsnName == "CAL" || insn_container.InsnName == "CALC"
        || insn_container.InsnName == "CALCN") {

        if (insn_container.InsnName == "CALC" && !IsCRSet())
            return insn_container.InsnPosition + 1;
        else if (insn_container.InsnName == "CALCN" && IsCRSet())
            return insn_container.InsnPosition + 1;

        
        if (insn_container.OperandList.size() != 2) {
            __AssociatedResource->__configuration->PCLogger->RaiseException(
            "Incorrect number of operands for CAL instruction!"); 
        }

        std::unordered_map<string, string> VarsToSet;
        std::unordered_map<string, string> VarsToGet;

        Utils::ExtractCallInterfaceMapping(VarsToSet, VarsToGet,
            insn_container.OperandList[1]);

        string PoUName = insn_container.OperandList[0];
        auto PoU = __ExecPoUVariable->GetPtrToField(PoUName);

        assert(PoU != nullptr);
        if(PoU->__IsVariableContentTypeAPtr)
            PoU = PoU->GetPtrStoredAtField("");

        for (auto it = VarsToSet.begin(); it != VarsToSet.end(); it++) {
            PCVariable * VarToSet;
            if (Utils::IsOperandImmediate(it->second)) {
                VarToSet 
                = __AssociatedResource->GetVariableForImmediateOperand(
                    it->second);

                if(VarToSet == nullptr)
                    __AssociatedResource->__configuration->PCLogger->RaiseException(
                        "Error processing immediate operand: " + it->second);

                
            } else {
                VarToSet = __ExecPoUVariable->GetPtrToField(it->second);

                if(VarToSet == nullptr)
                    __AssociatedResource->__configuration->PCLogger->RaiseException(
                        "Error processing operand: " + it->second);
                if (VarToSet->__IsVariableContentTypeAPtr)
                    VarToSet = VarToSet->GetPtrStoredAtField("");

                assert(Utils::WriteAccessCheck(
                        __AssociatedResource->__configuration,
                        __ExecPoUVariable->__VariableDataType->__DataTypeName,
                        PoUName + "." + it->first));
            }

            PoU->SetField(it->first, VarToSet);
        }
        Executor new_executor(__AssociatedResource->__configuration,
                    __AssociatedResource, __AssociatedTask);

        new_executor.SetExecPoUVariable(PoU);
        new_executor.Run();
        new_executor.CleanUp();


        for (auto it = VarsToGet.begin(); it != VarsToGet.end(); it++) {
            PCVariable * VarToGet;

            if (Utils::IsOperandImmediate(it->first))
                __AssociatedResource->__configuration->PCLogger->RaiseException(
                    "Error: cannot get from an immediate operand: " + it->first);
            
            VarToGet = PoU->GetPtrToField(it->first);

            assert(Utils::ReadAccessCheck(
                __AssociatedResource->__configuration,
                __ExecPoUVariable->__VariableDataType->__DataTypeName,
                PoUName + "." + it->first));

            if(VarToGet == nullptr)
                __AssociatedResource->__configuration->PCLogger->RaiseException(
                    "Error processing output operand: " + it->first);
            assert(VarToGet->__IsVariableContentTypeAPtr == false);

            if (Utils::IsOperandImmediate(it->second))
                __AssociatedResource->__configuration->PCLogger->RaiseException(
                    "Error: cannot set to an immediate operand: " + it->second);
            __ExecPoUVariable->SetField(it->second, VarToGet);
        }

        // Operand should be of the form "(Var1:=Value1, Var2:=Value2, ...)"
    } else if (insn_container.InsnName == "JMP" 
        || insn_container.InsnName == "JMPC"
        || insn_container.InsnName == "JMPCN") {

        if (insn_container.InsnName == "JMPC" && !IsCRSet())
            return insn_container.InsnPosition + 1;
        else if (insn_container.InsnName == "JMPCN" && IsCRSet())
            return insn_container.InsnPosition + 1;

        if (insn_container.OperandList.size() != 1) {
            __AssociatedResource->__configuration->PCLogger->RaiseException(
            "Incorrect number of operands for Jump instruction!"); 
        }

        auto JmpLabel = insn_container.OperandList[0];
        auto nxt_insn_container = __CodeContainer->GetInsnAT(JmpLabel);

        if (!nxt_insn_container) {
             __AssociatedResource->__configuration->PCLogger->RaiseException(
            "Jmp Label: " + JmpLabel + " not found!"); 
        }
        if (insn_container.InsnPosition == nxt_insn_container->InsnPosition) {
            __AssociatedResource->__configuration->PCLogger->RaiseException(
            "Jmp Label: " + JmpLabel + " jumping to same line!"); 
        }
        return nxt_insn_container->InsnPosition;

    } else if (insn_container.InsnName == "RET" 
        || insn_container.InsnName == "RETC"
        || insn_container.InsnName == "RETCN") {

        if (insn_container.InsnName == "RETC" && !IsCRSet())
            return insn_container.InsnPosition + 1;
        else if (insn_container.InsnName == "RETCN" && IsCRSet())
            return insn_container.InsnPosition + 1;
        return -1;

    } else { // All the other instructions/SFCs

        auto Fn = __AssociatedTask->FCRegistry->GetFunction(
                insn_container.InsnName);

        std::vector <PCVariable *> Ops;
        for (int i = 0; i < insn_container.OperandList.size(); i++) {
            if(Utils::IsOperandImmediate(insn_container.OperandList[i])) {
                Ops.push_back(
                    __AssociatedResource->GetVariableForImmediateOperand(
                    insn_container.OperandList[i]));
            } else {
                Ops.push_back(__ExecPoUVariable->GetPtrToField(
                    insn_container.OperandList[i]));   
            }
        }


        if (Fn == nullptr) {
            // Need to perform access checks here too for LD and ST instructions
            __AssociatedResource->ExecuteInsn(insn_container.InsnName, Ops);
        } else {
            auto FnDataType = Fn->__VariableDataType;
            int i = 0;
            
            for(auto& DefinedField: 
                FnDataType->__FieldsByInterfaceType[
                    FieldInterfaceType::VAR_INPUT]) {

                if (i >= Ops.size()) {
                    __AssociatedResource->__configuration
                        ->PCLogger->RaiseException(
                            "All arguments not specified for Function: " 
                            + FnDataType->__DataTypeName); 
                }
                Fn->SetField(DefinedField.__FieldName, Ops[i]);
                Executor new_executor(__AssociatedResource->__configuration,
                    __AssociatedResource, __AssociatedTask);

                new_executor.SetExecPoUVariable(Fn);
                new_executor.Run();
                new_executor.CleanUp();

            }
        }
    }

    return -1;
}

bool Executor::IsCRSet() {
    if (__AssociatedResource
        ->__CurrentResult->__VariableDataType->__DataTypeCategory 
        != DataTypeCategory::BOOL) {
        __AssociatedResource->__configuration->PCLogger->RaiseException(
            "CR type error before executing a conditional control flow instruction!");
    }

    bool Value = __AssociatedResource
                    ->__CurrentResult->GetValueStoredAtField<bool>("",
                        DataTypeCategory::BOOL);

    return Value;
}

void Executor::SaveCPURegisters() {
    *__AssociatedTask->__CR = *__AssociatedResource->__CurrentResult;
}

void Executor::RestoreCPURegisters() {
    *__AssociatedResource->__CurrentResult = *__AssociatedTask->__CR;
}

void Executor::ResetCPURegisters() {
    *__AssociatedResource->__CurrentResult 
        = *__AssociatedResource->GetTmpVariable("BOOL", "0");
    *__AssociatedTask->__CR = *__AssociatedResource->__CurrentResult;
}