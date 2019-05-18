
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
#include "src/pc_emulator/include/insn_registry.h"
#include "src/pc_emulator/include/sfc_registry.h"
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
        if (!insn_container) {
            string SFBName 
            = __ExecPoUVariable->__VariableDataType->__DataTypeName;
            std::cout << "Executing SFB: " << SFBName << std::endl;
            // This is a SFB without a code body
            assert(__ExecPoUVariable->__VariableDataType->__PoUType
                == PoUType::FB);

            auto SFBExecutor = __AssociatedResource->__SFBRegistry->GetSFB(
                SFBName);

            if (SFBExecutor == nullptr) {
                __AssociatedResource->__configuration->PCLogger->RaiseException(
                    "Defnition of SFB: " + SFBName + " not found!");
            }

            SFBExecutor->Execute(__AssociatedTask->__CR, __ExecPoUVariable);
            __AssociatedResource->clock->UpdateCurrentTime(SFBName);
            if (__AssociatedResource->clock->__stop) {
                __AssociatedTask->__Executing = false;
                return;
            }
            idx = -1;
        } else {
            //std::cout << "Executing Insn: " << insn_container->InsnName << std::endl;
            idx = RunInsn(*insn_container);
            __AssociatedResource->clock->UpdateCurrentTime(
                (*insn_container).InsnName);
            
            if (__AssociatedResource->clock->__stop) {
                __AssociatedTask->__Executing = false;
                return;
            }
        }

        
        Task * EligibleTask = nullptr;
        do {
            EligibleTask = __AssociatedResource->GetInterruptTaskToExecute();
            if (EligibleTask != nullptr
                && EligibleTask->__priority < __AssociatedTask->__priority) {
                if (!EligibleTask->__Executing) {

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
        if (__AssociatedResource->clock->__stop) {
            __AssociatedTask->__Executing = false;
            return;
        }
        
        if (idx < 0 || idx >= __CodeContainer->__InsnCount)
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

        //std:: cout << "PoUName: " << PoUName 
        //    << " Operands: " << insn_container.OperandList[1] << std::endl;
        auto PoU = __ExecPoUVariable->GetPtrToField(PoUName);

        assert(PoU != nullptr);
        if(PoU->__IsVariableContentTypeAPtr)
            PoU = __ExecPoUVariable->GetPtrStoredAtField(PoUName);

        for (auto it = VarsToSet.begin(); it != VarsToSet.end(); it++) {
            PCVariable * VarToSet;
            //std::cout << "Var To Set: " 
            //<< it->second << " to: " << it->first << std::endl;
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
                if (VarToSet->__IsVariableContentTypeAPtr) {
                    //std::cout << "Here ..." << std::endl;
                    VarToSet = __ExecPoUVariable->GetPtrStoredAtField(it->second);
                    assert(VarToSet != nullptr);
                }

                assert(Utils::WriteAccessCheck(
                        __AssociatedResource->__configuration,
                        __ExecPoUVariable->__VariableDataType->__DataTypeName,
                        PoUName + "." + it->first));
            }

            PoU->SetField(it->first, VarToSet);
        }
        Executor new_executor(__AssociatedResource->__configuration,
                    __AssociatedResource, __AssociatedTask);

        //std::cout << "Executing PoU: " 
        //<< PoU->__VariableDataType->__DataTypeName << std::endl;
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

        return insn_container.InsnPosition + 1;
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
        //std::cout << "Executing: " << insn_container.InsnName << " "
        //        << JmpLabel << std::endl;
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

        /*
        if (insn_container.OperandList.size() > 0) {
                std::cout << "Executing Insn: " << insn_container.InsnName 
                << " " << insn_container.OperandList[0] << std::endl;
        } else {
                std::cout << "Executing Insn: " << insn_container.InsnName 
                 << std::endl;
        }*/
        std::vector <PCVariable *> Ops;
        for (int i = 0; i < insn_container.OperandList.size(); i++) {
            if(Utils::IsOperandImmediate(insn_container.OperandList[i])) {
                Ops.push_back(
                    __AssociatedResource->GetVariableForImmediateOperand(
                    insn_container.OperandList[i]));
            } else {

                auto field = __ExecPoUVariable->GetPtrToField(
                    insn_container.OperandList[i]);
                Ops.push_back(field);
                
                /*if (!field->__IsVariableContentTypeAPtr)
                    Ops.push_back(field);
                else {
                    field = __ExecPoUVariable->GetPtrStoredAtField(
                        insn_container.OperandList[i]);
                    assert(field != nullptr);
                    Ops.push_back(field);
                }*/  
            }
        }


        if (Fn == nullptr) {
            // Need to perform access checks here too for LD and ST instructions

            
            auto InsnObj = __AssociatedResource->__InsnRegistry->GetInsn(
                insn_container.InsnName);

            if (InsnObj != nullptr) {
                InsnObj->Execute(__AssociatedTask->__CR, Ops);
            } else {

                auto SFCObj = __AssociatedResource->__SFCRegistry->GetSFC(
                    insn_container.InsnName);

                //std::cout << "SFC Name: " << insn_container.InsnName << std::endl;

                if (SFCObj != nullptr) {
                    SFCObj->Execute(__AssociatedTask->__CR, Ops);
                } else {
                    __AssociatedResource
                    ->__configuration->PCLogger->RaiseException("Opcode: "
                        + insn_container.InsnName + " not recognized "
                        " as a valid IL Instruction or SFC!");
                }
            }
            
            //std::cout << "Finished Insn" << std::endl;
            return insn_container.InsnPosition + 1;
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
            
            }
            Executor new_executor(__AssociatedResource->__configuration,
                __AssociatedResource, __AssociatedTask);

            new_executor.SetExecPoUVariable(Fn);
            new_executor.Run();
            new_executor.CleanUp();
            return insn_container.InsnPosition + 1;
        }
    }

    return -1;
}

bool Executor::IsCRSet() {
    if (__AssociatedTask
        ->__CR->__VariableDataType->__DataTypeCategory 
        != DataTypeCategory::BOOL) {
        __AssociatedResource->__configuration->PCLogger->RaiseException(
            "CR type error before executing a conditional control flow instruction!");
    }

    bool Value = __AssociatedTask->__CR->GetValueStoredAtField<bool>("",
                        DataTypeCategory::BOOL);

    return Value;
}

void Executor::SaveCPURegisters() {
    //*__AssociatedTask->__CR = *__AssociatedResource->__CurrentResult;
}

void Executor::RestoreCPURegisters() {
    //*__AssociatedResource->__CurrentResult = *__AssociatedTask->__CR;
}

void Executor::ResetCPURegisters() {
    /*
    *__AssociatedResource->__CurrentResult 
        = *__AssociatedResource->GetTmpVariable("BOOL", "0");
    *__AssociatedTask->__CR = *__AssociatedResource->__CurrentResult;*/
}