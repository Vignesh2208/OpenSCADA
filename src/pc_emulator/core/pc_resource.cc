#include <assert.h>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <regex>
#include <vector>
#include <queue>

#include "pc_emulator/include/pc_pou_code_container.h"
#include "pc_emulator/include/pc_datatype.h"
#include "pc_emulator/include/pc_variable.h"
#include "pc_emulator/include/pc_resource.h"
#include "pc_emulator/include/pc_configuration.h"
#include "pc_emulator/include/utils.h"
#include "pc_emulator/include/task.h"

using namespace std;
using namespace pc_emulator;

void PCResource::RegisterPoUVariable(string VariableName, PCVariable * Var) {
    std::unordered_map<std::string, PCVariable*>::const_iterator got = 
                        __ResourcePoUVars.find (VariableName);
    if (got != __ResourcePoUVars.end()) {
        
        __configuration->PCLogger->RaiseException("Variable already defined !");
    } else {
        __ResourcePoUVars.insert(std::make_pair(VariableName, Var));
       
        __configuration->PCLogger->LogMessage(LOG_LEVELS::LOG_INFO,
                                        "Registered new resource pou variable!");
    }

}

PCVariable * PCResource::GetVariable(string NestedFieldName) {
    assert(!NestedFieldName.empty());
    std::vector<string> results;
    boost::split(results, NestedFieldName, [](char c){return c == '.';});

    if  (results.size() == 1) {
        //not . was found
        // this may belong to global variable
            PCVariable * global_var = __ResourcePoUVars.find(
                    "__RESOURCE_" + __ResourceName + "_GLOBAL__")->second;
            assert(global_var != nullptr);
            if (global_var->__VariableDataType->IsFieldPresent(NestedFieldName))
                return global_var->GetPCVariableToField(NestedFieldName);
            else
                return nullptr;
    } else {
        // dot was found; could be of the form resource_pou_var.field_name
        std::unordered_map<std::string, PCVariable*>::const_iterator got = 
                        __ResourcePoUVars.find(results[0]);
        if (got == __ResourcePoUVars.end()) {
            
            // this may belong to global variable
            PCVariable * global_var = __ResourcePoUVars.find(
                    "__RESOURCE_" + __ResourceName + "_GLOBAL__")->second;
            assert(global_var != nullptr);
            if (global_var->__VariableDataType->IsFieldPresent(NestedFieldName))
                return global_var->GetPCVariableToField(NestedFieldName);
            else
                return nullptr;

        } else {
            PCVariable * Base = got->second;
            assert(Base != nullptr);
            string Field = NestedFieldName.substr(
                    NestedFieldName.find('.') + 1, string::npos);
            if (Base->__VariableDataType->IsFieldPresent(Field))
                return Base->GetPCVariableToField(Field);
            else
                return nullptr;
        }
    } 
}

PCVariable * PCResource::GetPoUVariable(string PoUName) {
    auto got =  __ResourcePoUVars.find(PoUName);
    if (got == __ResourcePoUVars.end()) {
        return nullptr;
    }
    return got->second;
}

PCVariable * PCResource::GetGlobalVariable(string NestedFieldName) {
    assert(!NestedFieldName.empty());
    std::vector<string> results;
    boost::split(results, NestedFieldName, [](char c){return c == '.';});

    for (auto it = __ResourcePoUVars.begin(); 
                    it != __ResourcePoUVars.end(); it ++) {
        PCVariable * var = it->second;
        if (var->__VariableDataType->IsFieldPresent(NestedFieldName)) {
            DataTypeFieldAttributes FieldAttributes;
            var->GetFieldAttributes(NestedFieldName, 
                                    FieldAttributes);
            if (FieldAttributes.FieldInterfaceType 
                == FIELD_INTERFACE_TYPES::VAR_GLOBAL
                || FieldAttributes.FieldInterfaceType 
                == FIELD_INTERFACE_TYPES::VAR_EXPLICIT_STORAGE) {
                return var->GetPCVariableToField(NestedFieldName);
            }
        }
    }

    return nullptr;
}

void PCResource::InitializeAllPoUVars() {

    
    for (auto & resource_spec : 
            __configuration->__specification.machine_spec().resource_spec()) {
        if (resource_spec.resource_name == __ResourceName) {
            if (resource_spec.has_resource_global_var()) {
                PCDataType * global_var_type = new PCDataType(
                    __configuration, 
                    "__RESOURCE_" + __ResourceName + "_GLOBAL__",
                    "__RESOURCE_" + __ResourceName + "_GLOBAL__",
                    DataTypeCategories::POU);

                __configuration->RegisteredDataTypes.RegisterDataType(
                "__RESOURCE_" + __ResourceName + "_GLOBAL__", global_var_type);

                Utils::InitializeDataType(__configuration, global_var_type,
                        resource_spec.resource_global_var());
                
                PCVariable * __global_pou_var = new PCVariable(__configuration,
                        this, "__RESOURCE_" + __ResourceName + "_GLOBAL_VAR__",
                        "__RESOURCE_" + __ResourceName + "_GLOBAL__");

                RegisterPoUVariable(
                    "__RESOURCE_" + __ResourceName + "_GLOBAL_VAR__",
                    __global_pou_var);

            }

            for (auto& pou_var : resource_spec.pou_var()) {

                assert(pou_var.datatype_category() == DataTypeCategory::POU);
                assert(pou_var.has_pou_type()
                    && (pou_var.pou_type() == PoUType::FC || 
                        pou_var.pou_type() == PoUType::FB ||
                        pou_var.pou_type() == PoUType::PROGRAM));
                         
                PCDataType * new_var_type = new PCDataType(
                    __configuration, 
                    pou_var.name(),
                    pou_var.name(),
                    DataTypeCategories::POU);

                __configuration->RegisteredDataTypes.RegisterDataType(
                                            pou_var.name(), new_var_type);

                Utils::InitializeDataType(__configuration, new_var_type,
                                        pou_var);

                if (pou_var.pou_type() != PoUType::FC) {
                    PCVariable * new_pou_var = new PCVariable(
                        __configuration,
                        this, pou_var.name(), pou_var.name());
                    

                    RegisterPoUVariable(pou_var.name(), new_pou_var);
                }
            }

            for(auto it = __ResourcePoUVars.begin();
                    it != __ResourcePoUVars.end(); it ++) {
                PCVariable * pou_var = it->second;
                pou_var->AllocateAndInitialize();
            }

            for(auto it = __ResourcePoUVars.begin();
                    it != __ResourcePoUVars.end(); it ++) {
                PCVariable * pou_var = it->second;
                pou_var->ResolveAllExternalFields();
            }

            break;
        }
    }

}

PCVariable * PCResource::GetVariablePointerToMem(int MemType, int ByteOffset,
                        int BitOffset, string VariableDataTypeName) {

    assert(ByteOffset > 0 && BitOffset >= 0 && BitOffset < 8);
    assert(MemType == MEM_TYPE::INPUT_MEM || MemType == MEM_TYPE::OUTPUT_MEM);
    string VariableName = __ResourceName + std::to_string(MemType)
                            + "." + std::to_string(ByteOffset)
                            + "." + std::to_string(BitOffset);

    // need to track and delete this variable later on
    auto got = __AccessedFields.find(VariableName);

    if(got == __AccessedFields.end()) {
        PCVariable* V = new PCVariable(__configuration, this, VariableName,
                                    VariableDataTypeName);
        assert(V != nullptr);

        if(MemType == MEM_TYPE::INPUT_MEM)
            V->__MemoryLocation.SetMemUnitLocation(&__InputMemory);
        else 
            V->__MemoryLocation.SetMemUnitLocation(&__OutputMemory);

        V->__ByteOffset = ByteOffset;
        V->__BitOffset = BitOffset;
        V->__IsDirectlyRepresented = true;
        V->__MemAllocated = true;
        V->AllocateAndInitialize();
        __AccessedFields.insert(std::make_pair(VariableName, V));
    } else {
        return got->second;
    }
   
}

void PCResource::Cleanup() {
    for ( auto it = __AccessedFields.begin(); it != __AccessedFields.end(); 
            ++it ) {
            PCVariable * __AccessedVariable = it->second;
            __AccessedVariable->Cleanup();
            delete __AccessedVariable;
    }

    for ( auto it = __ResourcePoUVars.begin(); it != __ResourcePoUVars.end(); 
            ++it ) {
            PCVariable * __AccessedVariable = it->second;
            __AccessedVariable->Cleanup();
            delete __AccessedVariable;
    }
}

PoUCodeContainer * PCResource::CreateNewCodeContainer(string PoUDataTypeName) {
    PCDataType * PoUDataType 
        = __configuration->RegisteredDataTypes.GetDataType(PoUDataTypeName);
    if (!PoUDataType)
        return nullptr;
    
    if (__CodeContainers.find(PoUDataTypeName) != __CodeContainers.end()) {
        __configuration->PCLogger->RaiseException("Cannot define two code "
                                        "bodies for same POU");
    }
    PoUCodeContainer * new_container = new PoUCodeContainer(__configuration,
                                                this);
    new_container->SetPoUDataType(PoUDataType);

    __CodeContainers.insert(std::make_pair(PoUDataTypeName, new_container));

    return new_container;

    
}
PoUCodeContainer * PCResource::GetCodeContainer(string PoUDataTypeName) {

    auto got = __CodeContainers.find(PoUDataTypeName);
    if (got == __CodeContainers.end())
        return nullptr;
    
    return got->second;
}


void PCResource::AddTask(Task * Tsk) {

    if (Tsk && __Tasks.find(Tsk->__TaskName) == __Tasks.end()) {
        __Tasks.insert(std::make_pair(Tsk->__TaskName, Tsk));
        return;
    }

    __configuration->PCLogger->RaiseException("Couldn't add tsk to resource !");
}

Task * PCResource::GetTask(string TaskName) {
    auto got = __Tasks.find(TaskName);
    if (got != __Tasks.end())
        return got->second;
    return nullptr;
}

void PCResource::QueueTask (Task * Tsk) {

    if (Tsk == nullptr)
        __configuration->PCLogger->RaiseException("Cannot queue null task!");

    auto got = __IntervalTasksByPriority.find(Tsk->__priority);
    if (got == __IntervalTasksByPriority.end() 
                    && Tsk->type == TaskType::INTERVAL) {
        
        CompactTaskDescription tskDescription(Tsk->__TaskName,
                                            Tsk->__nxt_schedule_time_ms);
        __IntervalTasksByPriority.insert(std::make_pair(Tsk->__priority,
                    priority_queue<CompactTaskDescription>()));
        __IntervalTasksByPriority.find(Tsk->__priority)->second.push(
                            tskDescription);
    } else if (Tsk->type == TaskType::INTERVAL) {
        CompactTaskDescription tskDescription(Tsk->__TaskName,
                                            Tsk->__nxt_schedule_time_ms);
        got->second.push(tskDescription);
    } else {
        auto got = __InterruptTasks.find(Tsk->__trigger_variable_name);
        if (got == __InterruptTasks.end()) {
            
            __InterruptTasks.insert(std::make_pair(Tsk->__trigger_variable_name,
                                    vector<Task*>()));
            __InterruptTasks.find(
                    Tsk->__trigger_variable_name)->second.push_back(Tsk);
        } else {
            got->second.push_back(Tsk);
        }
    }
}

Task * PCResource::GetInterruptTaskToExecute() {
    Task * EligibleTask = nullptr;
    int highest_priority = 10000;

    for (auto it = __InterruptTasks.begin(); it != __InterruptTasks.end(); it++) {
        PCVariable * trigger;
        trigger = __configuration->GetVariable(it->first);
        if(!trigger) {
            trigger = GetGlobalVariable(it->first);
            if(!trigger)
                __configuration->PCLogger->RaiseException("Invalid trigger: "
                        + it->first);

        }
        assert(trigger->__VariableDataType->__DataTypeCategory 
                    == DataTypeCategories::BOOL); //trigger must be a boolean
        auto curr_value = trigger->GetFieldValue<bool>("",
                                        DataTypeCategories::BOOL);
        
        
        for (int itt = 0; itt < it->second.size(); itt++) {
            auto prev_value = it->second[itt]->__trigger_variable_previous_value;
            if (prev_value == false && curr_value == true) {
                if (it->second[itt]->__priority < highest_priority) {
                    highest_priority = it->second[itt]->__priority;
                    EligibleTask = it->second[itt];
                }

                it->second[itt]->__IsReady = true;
                it->second[itt]->__trigger_variable_previous_value = true;
            } else if (it->second[itt]->__IsReady == true) {
                if (it->second[itt]->__priority < highest_priority) {
                    highest_priority = it->second[itt]->__priority;
                    EligibleTask = it->second[itt];
                }
            }
        }
    }

    if (EligibleTask != nullptr)
        return EligibleTask;
    return nullptr;
}

Task * PCResource::GetIntervalTaskToExecuteAt(double schedule_time) {

    

    // Else we need to get highest priority expired interval task
    
    Task * EligibleTask = nullptr;
    int highest_priority = 10000;

    for(auto it = __IntervalTasksByPriority.begin(); 
            it != __IntervalTasksByPriority.end(); it++ ) {
        
        auto compact_tsk_description = it->second.top();

        if (it->first < highest_priority 
            && compact_tsk_description.__nxt_schedule_time_ms <= schedule_time) {
            highest_priority = it->first;
            EligibleTask = GetTask(compact_tsk_description.__TaskName);
        }
    }

    if (EligibleTask != nullptr) {
        __IntervalTasksByPriority.find(highest_priority)->second.pop();
        return EligibleTask;
    }

    return nullptr;

}

void PCResource::InitializeAllTasks() {
    for (auto resource_spec : 
           __configuration->__specification.machine_spec().resource_spec()) {
        if(__ResourceName == resource_spec.resource_name()) {
            for (auto task_spec : resource_spec.tasks()) {
                    Task * new_task = new Task(__configuration, this, task_spec);

                    if (new_task->type == TaskType::INTERVAL) {
                        new_task->SetNextScheduleTime(clock->GetCurrentTime()
                            + (float)new_task->__interval_ms);
                    }

                    AddTask(new_task);
                    QueueTask(new_task);
            }   

            for (auto program_spec : resource_spec.programs()) {
                Task * associated_task 
                            = GetTask(program_spec.task_name());
                if (associated_task != nullptr) {
                    associated_task->AddProgramToTask(program_spec);
                    
                }
                
            }
            break;
        }
    }
}

 void PCResource::InitializeClock() {
     if (__configuration->__specification.has_enable_kronos() && 
            __configuration->__specification.enable_kronos())
        clock = new Clock(true, this);
    else {
        clock = new Clock(false, this);
    }

}

void PCResource::OnStartup() {
    InitializeClock();
    InitializeAllTasks();
}


bool CompactTaskDescription::operator==(const CompactTaskDescription& a) {
    return a.__nxt_schedule_time_ms == __nxt_schedule_time_ms;
}

bool CompactTaskDescription::operator>(const CompactTaskDescription& a) {
    return a.__nxt_schedule_time_ms > __nxt_schedule_time_ms;
}

bool CompactTaskDescription::operator<(const CompactTaskDescription& a) {
    return a.__nxt_schedule_time_ms < __nxt_schedule_time_ms;
}

bool CompactTaskDescription::operator<=(const CompactTaskDescription& a) {
    return a.__nxt_schedule_time_ms <= __nxt_schedule_time_ms;
}

bool CompactTaskDescription::operator>=(const CompactTaskDescription& a) {
    return a.__nxt_schedule_time_ms >= __nxt_schedule_time_ms;
}