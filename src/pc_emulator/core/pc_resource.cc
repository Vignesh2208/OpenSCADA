#include <assert.h>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <regex>
#include <vector>
#include <queue>
#include <fstream>
#include <cstdio>

#include "src/pc_emulator/include/pc_pou_code_container.h"
#include "src/pc_emulator/include/pc_datatype.h"
#include "src/pc_emulator/include/pc_variable.h"
#include "src/pc_emulator/include/pc_resource.h"
#include "src/pc_emulator/include/insn_registry.h"
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/include/task.h"
#include "src/pc_emulator/include/insn_registry.h"
#include "src/pc_emulator/include/sfc_registry.h"
#include "src/pc_emulator/include/sfb_registry.h"

using namespace std;
using namespace pc_emulator;
using namespace pc_specification;
using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldIntfType = pc_specification::FieldInterfaceType;
using SystemPOUs = pc_system_specification::SystemPOUs;


PCResourceImpl::PCResourceImpl(PCConfigurationImpl * configuration, 
    string ResourceName, int InputMemSize, int OutputMemSize) {
    __configuration = configuration;
    __ResourceName = ResourceName;
    __InputMemSize = InputMemSize;
    __OutputMemSize = OutputMemSize;

    assert(__InputMemSize > 0 && __OutputMemSize > 0);
    //__InputMemory.AllocateStaticMemory(__InputMemSize);
    //__OutputMemory.AllocateStaticMemory(__OutputMemSize);
    __InputMemory.AllocateSharedMemory(__InputMemSize,
                "/tmp/Output_" + ResourceName,
                ResourceName + "_IMemLock");
    __OutputMemory.AllocateSharedMemory(__OutputMemSize,
                "/tmp/Input_" + ResourceName,
                ResourceName + "_OMemLock");
    __CurrentResult = new PCVariable((PCConfiguration *)configuration,
                        (PCResource *) this, "__CurrentResult", "BOOL");
    __CurrentResult->AllocateAndInitialize();
    __InsnRegistry = new InsnRegistry(this);
    __SFCRegistry = new SFCRegistry(this);
    __SFBRegistry = new SFBRegistry(this);

    for (auto ins_spec: 
            configuration->__specification.machine_spec().ins_spec()) {

        if (ins_spec.mu_exec_time_ns() > 0 && ins_spec.sigma_exec_time_ns() >= 0)
        __ExecTimes.insert(std::make_pair(ins_spec.ins_name(),
            std::unique_ptr<std::normal_distribution<double>>(
                new std::normal_distribution<double>(
                        ins_spec.mu_exec_time_ns(), 
                        ins_spec.sigma_exec_time_ns()))));
   
    }

    for (auto sfc_spec: 
            configuration->__specification.machine_spec().sfc_spec()) {
        if (sfc_spec.mu_exec_time_ns() > 0 && sfc_spec.sigma_exec_time_ns() >= 0)
        __ExecTimes.insert(std::make_pair(sfc_spec.sfc_name(),
            std::unique_ptr<std::normal_distribution<double>>(
                new std::normal_distribution<double>(
                        sfc_spec.mu_exec_time_ns(), 
                        sfc_spec.sigma_exec_time_ns()))));
    }

    for (auto sfb_spec: 
            configuration->__specification.machine_spec().sfb_spec()) {
        if (sfb_spec.mu_exec_time_ns() > 0 && sfb_spec.sigma_exec_time_ns() >= 0)
        __ExecTimes.insert(std::make_pair(sfb_spec.sfb_name(),
            std::unique_ptr<std::normal_distribution<double>>(
                new std::normal_distribution<double>(
                        sfb_spec.mu_exec_time_ns(), 
                        sfb_spec.sigma_exec_time_ns()))));
    }

    InitializeClock();
}

void PCResourceImpl::RegisterPoUVariable(string VariableName,
                                std::unique_ptr<PCVariable> Var) {
    auto got = __ResourcePoUVars.find (VariableName);
    if (got != __ResourcePoUVars.end()) {
        
        __configuration->PCLogger->RaiseException("Variable already defined !");
    } else {   
        __configuration->PCLogger->LogMessage(LogLevels::LOG_INFO,
                                "Registered new resource pou variable!");
        __ResourcePoUVars.insert(std::make_pair(VariableName,
                                            std::move(Var)));
    }

}

PCVariable * PCResourceImpl::GetExternVariable(string NestedFieldName) {
    assert(!NestedFieldName.empty());
    DataTypeFieldAttributes FieldAttributes;
    auto got = __ResourcePoUVars.find(
            "__RESOURCE_" + __ResourceName + "_GLOBAL_VAR__");
    PCVariable * global_var;

    if (got == __ResourcePoUVars.end())
        global_var = nullptr;
    else {
        global_var = got->second.get();
    }
    
    if (global_var != nullptr && 
        global_var->__VariableDataType->IsFieldPresent(NestedFieldName)){

        global_var->GetFieldAttributes(NestedFieldName, FieldAttributes);

        if (!Utils::IsFieldTypePtr(FieldAttributes.FieldDetails.__FieldInterfaceType))
            return global_var->GetPtrToField(NestedFieldName);
        else
            return global_var->GetPtrStoredAtField(NestedFieldName);
    }
    else {
        // this may be referring to a PoU itself
        auto got = GetPOU(NestedFieldName);
        if (got == nullptr) {
            // This may be referring to a POUGlobalVariable
            return GetPOUGlobalVariable(NestedFieldName);
        } else {
            // we don't return POUs itself here

            return nullptr;
        }
    }
}

PCVariable * PCResourceImpl::GetPOU(string PoUName) {
    auto got =  __ResourcePoUVars.find(PoUName);
    if (got == __ResourcePoUVars.end()) {
        return nullptr;
    }
    return got->second.get();
}

// Returns a global variable or a directly represented variable declared
// by any of the POUs defined in this resource provided the NestedFieldName matches
PCVariable * PCResourceImpl::GetPOUGlobalVariable(string NestedFieldName) {
    assert(!NestedFieldName.empty());
    std::vector<string> results;
    boost::split(results, NestedFieldName, boost::is_any_of("."),
            boost::token_compress_on);

    PCVariable * AssociatedPOU = nullptr;
    if (results.size() > 1 
        && __ResourcePoUVars.find(results[0]) != __ResourcePoUVars.end()) {
        AssociatedPOU = __ResourcePoUVars.find(results[0])->second.get();
    } 
    
    if (AssociatedPOU == nullptr)
        return nullptr;
    

    std:: cout << "NOT NULL: " << AssociatedPOU->__VariableName << std::endl;
    string RemFieldName = NestedFieldName.substr(NestedFieldName.find('.') + 1,
                                string::npos);
    
    if (AssociatedPOU->__VariableDataType->IsFieldPresent(RemFieldName)) {
        DataTypeFieldAttributes FieldAttributes;
        AssociatedPOU->GetFieldAttributes(RemFieldName, 
                                FieldAttributes);
        if (FieldAttributes.FieldDetails.__FieldInterfaceType
            == FieldIntfType::VAR_GLOBAL)
            return AssociatedPOU->GetPtrToField(RemFieldName);

        

        if (FieldAttributes.FieldDetails.__FieldInterfaceType 
            == FieldIntfType::VAR_EXPLICIT_STORAGE) {
            //std::cout << "Returned: " 
            //<< FieldAttributes.FieldDetails.__StorageMemType
            //<< " Byte: " << FieldAttributes.FieldDetails.__StorageByteOffset
            //<< " Bit: " << FieldAttributes.FieldDetails.__StorageBitOffset
            //<< std::endl;
            return AssociatedPOU->GetPtrStoredAtField(RemFieldName);
        }
    }


    return nullptr;
}

void PCResourceImpl::InitializeAllSFBVars() {
    string sfb_spec_file = Utils::GetInstallationDirectory() 
        + "/src/pc_emulator/proto/system_pous.prototxt";
    SystemPOUs system_pous;
    int fileDescriptor = open(sfb_spec_file.c_str(),
                            O_RDONLY);

    if( fileDescriptor < 0 ) {
        std::cerr << " Error opening the specification file " 
                    << std::endl;
        exit(-1);
    }

    google::protobuf::io::FileInputStream 
                                    fileInput(fileDescriptor);
    fileInput.SetCloseOnDelete( true );

    if (!google::protobuf::TextFormat::Parse(&fileInput,
                                    &system_pous)) {
        std::cerr << std::endl << "Failed to parse system spec file!" 
        << std::endl;
        exit(-1);
    }
    for (auto& pou_var : system_pous.system_pou()) {

        assert(pou_var.datatype_category() == DataTypeCategory::POU);
        assert(pou_var.has_pou_type()
            && (pou_var.pou_type() == PoUType::FC || 
                pou_var.pou_type() == PoUType::FB ||
                pou_var.pou_type() == PoUType::PROGRAM));

        if (__configuration->RegisteredDataTypes->GetDataType(
            pou_var.name()) == nullptr) { 
            auto new_var_type = std::unique_ptr<PCDataType>(
                new PCDataType((PCConfiguration *)__configuration, 
                    pou_var.name(), pou_var.name(), DataTypeCategory::POU));

            Utils::InitializeDataType(__configuration, new_var_type.get(),
                                    pou_var);

            

            __configuration->RegisteredDataTypes->RegisterDataType(
                                        pou_var.name(),
                                        std::move(new_var_type));
        }

        auto registered_type = __configuration->RegisteredDataTypes->GetDataType(
                pou_var.name()); 
        auto code_container = CreateNewCodeContainer(registered_type);
        assert(code_container != nullptr);

        
        auto new_pou_var = std::unique_ptr<PCVariable>(new PCVariable(
            (PCConfiguration *)__configuration,
            (PCResource *) this, pou_var.name(), pou_var.name()));
        

        if (pou_var.pou_type() == PoUType::FC)
            new_pou_var->__VariableDataType->__PoUType = PoUType::FC;
        else if (pou_var.pou_type() == PoUType::FB)
            new_pou_var->__VariableDataType->__PoUType = PoUType::FB;
        else
            new_pou_var->__VariableDataType->__PoUType 
                                                    = PoUType::PROGRAM;

        RegisterPoUVariable(pou_var.name(), std::move(new_pou_var));
        
        for (auto& insn : pou_var.code_body().insn()) {
            code_container->AddInstruction(insn);
        }
    }

    for(auto it = __ResourcePoUVars.begin();
            it != __ResourcePoUVars.end(); it ++) {
        PCVariable * pou_var = it->second.get();
        pou_var->AllocateAndInitialize("/tmp/" + __ResourceName
            + "_" + pou_var->__VariableName);
    
        Utils::ValidatePOUDefinition(pou_var, __configuration);
        if (pou_var->__VariableDataType->__PoUType 
            == pc_specification::PoUType::PROGRAM) {
                if (pou_var->__VariableDataType
                    ->__FieldsByInterfaceType[
                        FieldIntfType::VAR_ACCESS].size())
                __configuration->PCLogger->RaiseException(
                    "A PROGRAM: " + pou_var->__VariableName
                    + " cannot define ACCESS variables!");
        }
    }

}

void PCResourceImpl::InitializeAllPoUVars() {

    
    InitializeAllSFBVars();
    
    for (auto & resource_spec : 
            __configuration->__specification.machine_spec().resource_spec()) {
        if (resource_spec.resource_name() == __ResourceName) {
            if (resource_spec.has_resource_global_var()) {
                auto global_var_type = std::unique_ptr<PCDataType>
                ( new PCDataType(
                    (PCConfiguration *)__configuration, 
                    "__RESOURCE_" + __ResourceName + "_GLOBAL__",
                    "__RESOURCE_" + __ResourceName + "_GLOBAL__",
                    DataTypeCategory::POU));

                Utils::InitializeDataType(__configuration, global_var_type.get(),
                        resource_spec.resource_global_var());

                __configuration->RegisteredDataTypes->RegisterDataType(
                                "__RESOURCE_" + __ResourceName + "_GLOBAL__",
                                std::move(global_var_type));

                
                
                auto __global_pou_var = std::unique_ptr<PCVariable>(
                    new PCVariable((PCConfiguration *) __configuration,
                        (PCResource *)this, 
                        "__RESOURCE_" + __ResourceName + "_GLOBAL_VAR__",
                        "__RESOURCE_" + __ResourceName + "_GLOBAL__"));

                __global_pou_var->__VariableDataType->__PoUType 
                = pc_specification::PoUType::PROGRAM;

                RegisterPoUVariable(
                    "__RESOURCE_" + __ResourceName + "_GLOBAL_VAR__",
                    std::move(__global_pou_var));


            }

            for (auto& pou_var : resource_spec.pou_var()) {

                assert(pou_var.datatype_category() == DataTypeCategory::POU);
                assert(pou_var.has_pou_type()
                    && (pou_var.pou_type() == PoUType::FC || 
                        pou_var.pou_type() == PoUType::FB ||
                        pou_var.pou_type() == PoUType::PROGRAM));
                         
                auto new_var_type = std::unique_ptr<PCDataType>(
                    new PCDataType((PCConfiguration *)__configuration, 
                        pou_var.name(), pou_var.name(), DataTypeCategory::POU));

                Utils::InitializeDataType(__configuration, new_var_type.get(),
                                        pou_var);

                auto code_container = CreateNewCodeContainer(new_var_type.get());
                assert(code_container != nullptr);
       
                __configuration->RegisteredDataTypes->RegisterDataType(
                                            pou_var.name(),
                                            std::move(new_var_type));

                
                auto new_pou_var = std::unique_ptr<PCVariable>(new PCVariable(
                    (PCConfiguration *)__configuration,
                    (PCResource *) this, pou_var.name(), pou_var.name()));
                

                if (pou_var.pou_type() == PoUType::FC)
                    new_pou_var->__VariableDataType->__PoUType = PoUType::FC;
                else if (pou_var.pou_type() == PoUType::FB)
                    new_pou_var->__VariableDataType->__PoUType = PoUType::FB;
                else
                    new_pou_var->__VariableDataType->__PoUType 
                                                            = PoUType::PROGRAM;

                RegisterPoUVariable(pou_var.name(), std::move(new_pou_var));

                
                for (auto& insn : pou_var.code_body().insn()) {
                    code_container->AddInstruction(insn);
                }
            }

            for(auto it = __ResourcePoUVars.begin();
                    it != __ResourcePoUVars.end(); it ++) {
                PCVariable * pou_var = it->second.get();
                pou_var->AllocateAndInitialize("/tmp/" + __ResourceName
                    + "_" + pou_var->__VariableName);
            
                Utils::ValidatePOUDefinition(pou_var, __configuration);
                if (pou_var->__VariableDataType->__PoUType 
                    == pc_specification::PoUType::PROGRAM) {
                        if (pou_var->__VariableDataType
                            ->__FieldsByInterfaceType[
                                FieldIntfType::VAR_ACCESS].size())
                        __configuration->PCLogger->RaiseException(
                            "A PROGRAM: " + pou_var->__VariableName
                            + " cannot define ACCESS variables!");
                }
            }

            
            // No need to do this for FCs because they wouldn't have any external fields

            break;
        }
    }


}

void PCResourceImpl::ResolveAllExternalFields() {
    for(auto it = __ResourcePoUVars.begin();
            it != __ResourcePoUVars.end(); it ++) {
        PCVariable * pou_var = it->second.get();

        if (pou_var->__VariableDataType->__PoUType 
            == pc_specification::PoUType::PROGRAM)
            pou_var->ResolveAllExternalFields(); 
            // First resolve external fields in all programs
    }

    for(auto it = __ResourcePoUVars.begin();
            it != __ResourcePoUVars.end(); it ++) {
        PCVariable * pou_var = it->second.get();

        if (pou_var->__VariableDataType->__PoUType 
            == pc_specification::PoUType::FB)
            pou_var->ResolveAllExternalFields(); 
            //  Next resolve external fields in all Function Blocks
    }
}

PCVariable * PCResourceImpl::GetVariablePointerToMem(int memType, int ByteOffset,
                        int BitOffset, string VariableDataTypeName) {

    assert(ByteOffset > 0 && BitOffset >= 0 && BitOffset < 8);
    assert(memType == MemType::INPUT_MEM || memType == MemType::OUTPUT_MEM);

    if (Utils::ResolveAliasName(VariableDataTypeName,
        __configuration) != "BOOL")
        BitOffset = 0;

    string VariableName = __ResourceName 
                            + "." + VariableDataTypeName
                            + "." + std::to_string(memType)
                            + "." + std::to_string(ByteOffset)
                            + "." + std::to_string(BitOffset);

    // need to track and delete this variable later on
    auto got = __AccessedFields.find(VariableName);

    if(got == __AccessedFields.end()) {

        __AccessedFields.insert(std::make_pair(VariableName,
                    std::unique_ptr<PCVariable>(
                        new PCVariable((PCConfiguration *)__configuration,
                            (PCResource *) this, VariableName,
                                    VariableDataTypeName))));
        PCVariable* V = __AccessedFields.find(VariableName)->second.get();
        assert(V != nullptr);

        if(memType == MemType::INPUT_MEM)
            V->__MemoryLocation.SetMemUnitLocation(&__InputMemory);
        else 
            V->__MemoryLocation.SetMemUnitLocation(&__OutputMemory);

        V->__ByteOffset = ByteOffset;
        V->__BitOffset = BitOffset;
        V->__IsDirectlyRepresented = true;
        V->__MemAllocated = true;
        V->AllocateAndInitialize();
        
        return V;
    } else {
        return got->second.get();
    }
   
}

void PCResourceImpl::Cleanup() {
    for ( auto it = __AccessedFields.begin(); it != __AccessedFields.end(); 
            ++it ) {
            PCVariable * __AccessedVariable = it->second.get();
            __AccessedVariable->Cleanup();
    }

    for ( auto it = __ResourcePoUVars.begin(); it != __ResourcePoUVars.end(); 
            ++it ) {
            PCVariable * __AccessedVariable = it->second.get();
            __AccessedVariable->Cleanup();
    }

    for ( auto it = __Tasks.begin(); it != __Tasks.end(); 
            ++it ) {
            auto __AccessedTask = it->second.get();
            __AccessedTask->Cleanup();
    }

    std::cout << "Removing MMap files: " << __ResourceName << std::endl;
    __InputMemory.Cleanup();
    __OutputMemory.Cleanup();
    //std::remove(("/tmp/Output_" + __ResourceName).c_str());
    //std::remove(("/tmp/Input_" + __ResourceName).c_str());

    __CurrentResult->Cleanup();
    delete __CurrentResult;
    delete __InsnRegistry;
    delete __SFCRegistry;
    delete __SFBRegistry;
    std:cout << "Resource: " << __ResourceName << " cleaned up ..." << std::endl;
}

PoUCodeContainer * PCResourceImpl::CreateNewCodeContainer(
                                        PCDataType* PoUDataType) {

    assert(PoUDataType != nullptr);
    if (__CodeContainers.find(PoUDataType->__DataTypeName) 
                != __CodeContainers.end()) {
        __configuration->PCLogger->RaiseException("Cannot define two code "
                                        "bodies for same POU");
    }
    auto new_container = 
        std::unique_ptr<PoUCodeContainer>(new PoUCodeContainer(__configuration,
                                                this));
    new_container->SetPoUDataType(PoUDataType);

    PoUCodeContainer * return_val = new_container.get();

    __CodeContainers.insert(std::make_pair(PoUDataType->__DataTypeName,
                        std::move(new_container)));

    return return_val;

    
}
PoUCodeContainer * PCResourceImpl::GetCodeContainer(string PoUDataTypeName) {

    auto got = __CodeContainers.find(PoUDataTypeName);
    if (got == __CodeContainers.end())
        return nullptr;
    
    return got->second.get();
}


void PCResourceImpl::AddTask(std::unique_ptr<Task> Tsk) {

    if (Tsk && __Tasks.find(Tsk->__TaskName) == __Tasks.end()) {
        __Tasks.insert(std::make_pair(Tsk->__TaskName, std::move(Tsk)));
        return;
    }

    __configuration->PCLogger->RaiseException("Couldn't add tsk to resource !");
}

Task * PCResourceImpl::GetTask(string TaskName) {
    auto got = __Tasks.find(TaskName);
    if (got != __Tasks.end())
        return got->second.get();
    return nullptr;
}

void PCResourceImpl::QueueTask (Task * Tsk) {

    if (Tsk == nullptr)
        __configuration->PCLogger->RaiseException("Cannot queue null task!");

    
    if (Tsk->type == TaskType::INTERVAL) {
        __IntervalTask = Tsk;
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

Task * PCResourceImpl::GetInterruptTaskToExecute() {
    Task * EligibleTask = nullptr;
    int highest_priority = 10000;

    for (auto it = __InterruptTasks.begin(); 
                        it != __InterruptTasks.end(); it++) {
        PCVariable * trigger;
        trigger = __configuration->GetExternVariable(it->first);
        if(!trigger) {
            trigger = GetPOUGlobalVariable(it->first);
            if(!trigger)
                __configuration->PCLogger->RaiseException("Invalid trigger: "
                        + it->first);

        }
        assert(trigger->__VariableDataType->__DataTypeCategory 
                    == DataTypeCategory::BOOL); //trigger must be a boolean
        auto curr_value = trigger->GetValueStoredAtField<bool>("",
                                        DataTypeCategory::BOOL);
        
        
        for (int itt = 0; itt < (int)it->second.size(); itt++) {
            auto prev_value = it->second[itt]->__trigger_variable_previous_value;
            if (prev_value == false && curr_value == true) {
                if (it->second[itt]->__priority < highest_priority) {
                    highest_priority = it->second[itt]->__priority;
                    EligibleTask = it->second[itt];
                }

                it->second[itt]->__IsReady = true;
                it->second[itt]->__trigger_variable_previous_value = curr_value;
            } else if (it->second[itt]->__IsReady == true) {
                if (it->second[itt]->__priority < highest_priority) {
                    highest_priority = it->second[itt]->__priority;
                    EligibleTask = it->second[itt];
                }

                it->second[itt]->__trigger_variable_previous_value = curr_value;
            }
        }
    }

    if (EligibleTask != nullptr)
        return EligibleTask;
    return nullptr;
}

Task * PCResourceImpl::GetIntervalTaskToExecuteAt(double schedule_time) {

    if (__IntervalTask && __IntervalTask->__nxt_schedule_time_ms
        <= schedule_time)
        return __IntervalTask;


    return nullptr;

}

void PCResourceImpl::InitializeAllTasks() {
    for (auto resource_spec : 
           __configuration->__specification.machine_spec().resource_spec()) {
        if(__ResourceName == resource_spec.resource_name()) {

            auto interval_task = std::unique_ptr<Task>(
                                new Task(__configuration, this,
                                    resource_spec.interval_task()));
            auto interval_task_ptr = interval_task.get();       
            interval_task->SetNextScheduleTime(clock->GetCurrentTime()*1000.0
                                    + (float)interval_task->__interval_ms);
            AddTask(std::move(interval_task));
            QueueTask(interval_task_ptr);

            for (auto task_spec : resource_spec.interrupt_tasks()) {
                    auto new_task = 
                        std::unique_ptr<Task>(
                            new Task(__configuration, this, task_spec));
                    auto new_task_ptr = new_task.get();

                    if (new_task->type == TaskType::INTERVAL) {
                        new_task->SetNextScheduleTime(clock->GetCurrentTime()*1000.0
                            + (float)new_task->__interval_ms);
                    }

                    AddTask(std::move(new_task));
                    QueueTask(new_task_ptr);
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

 void PCResourceImpl::InitializeClock() {
     if (__configuration->enable_kronos)
        clock = std::unique_ptr<Clock>(new Clock(true, this));
    else {
        clock = std::unique_ptr<Clock>(new Clock(false, this));
    }

}

void PCResourceImpl::ExecuteInsn(string InsnName,
        std::vector<PCVariable*>& Ops){

    auto InsnObj = __InsnRegistry->GetInsn(InsnName);

    if (InsnObj != nullptr) {
        InsnObj->Execute(__CurrentResult, Ops);
    }

    auto SFCObj = __SFCRegistry->GetSFC(InsnName);

    if (SFCObj != nullptr) {
        SFCObj->Execute(__CurrentResult, Ops);
    }
}

PCVariable * PCResourceImpl::GetTmpVariable(string VariableDataTypeName,
                                string InitialValue) {

    // need to track and delete this variable later on
    string VariableName = VariableDataTypeName + "_" + InitialValue;
    auto got = __AccessedFields.find(VariableName);
    PCDataType * VarDataType 
            = __configuration->LookupDataType(VariableDataTypeName);
    assert(VarDataType != nullptr);
    PCVariable* V;

    if(got == __AccessedFields.end()) {

        __AccessedFields.insert(std::make_pair(VariableName,
                    std::unique_ptr<PCVariable>(
                        new PCVariable((PCConfiguration *)__configuration,
                                    (PCResource *) this, VariableName,
                                    VariableDataTypeName))));
        V = __AccessedFields.find(VariableName)->second.get();
        assert(V != nullptr);
        V->AllocateAndInitialize(); 
        V->__IsTemporary = true;   
    } else {
        V =  got->second.get();
    }

    if (VarDataType->__DataTypeCategory != DataTypeCategory::DERIVED
        && VarDataType->__DataTypeCategory != DataTypeCategory::POU) {
        if (VariableDataTypeName == "STRING") {
            string Init = "{";
            for (int i = 0; i < InitialValue.length(); i++) {
                Init += InitialValue[i];
                Init += ",";
            }
            Init += "}";
            V->SetField("",Init);
        } else {
            V->SetField("", InitialValue);
        }
    }

    return V;
}

/*
 * Convert an immediate operand value string of form: <DataTypeName>Value into
 * a PCVariable object
 */
PCVariable * PCResourceImpl::GetVariableForImmediateOperand(string OperandValue) {
    string VariableTypeName, InitValue;
    InitValue = OperandValue;
    if (OperandValue == "TRUE" || OperandValue == "true"
        || OperandValue == "True") {
        VariableTypeName = "BOOL", InitValue = "1";
    } else if (OperandValue == "FALSE" || OperandValue == "false"
        || OperandValue == "False") {
        VariableTypeName = "BOOL", InitValue = "0";
    } else if(boost::starts_with(OperandValue, "16#")) {
        
        if (OperandValue.length() <= 5) {
            VariableTypeName = "BYTE";
        } else if (OperandValue.length() <= 7) {
            VariableTypeName = "WORD";
        } else if (OperandValue.length() <= 11) {
            VariableTypeName = "DWORD";
        } else {
            VariableTypeName = "LWORD";
        }

    } else if (boost::starts_with(OperandValue, "t#")) {
        VariableTypeName = "TIME";
    } else if (boost::starts_with(OperandValue, "tod#")) {
        VariableTypeName = "TOD";
    } else if (boost::starts_with(OperandValue, "d#")) {
        VariableTypeName = "DATE";
    } else if (boost::starts_with(OperandValue, "dt#")) {
        VariableTypeName = "DT";
    } else if (OperandValue[0] == '"' && OperandValue.length() == 3) {
        VariableTypeName = "CHAR", InitValue = OperandValue[1];
    } else if (OperandValue[0] == '"') {
        VariableTypeName = "STRING";
        InitValue = OperandValue.substr(1, OperandValue.length() - 2);
    } else if (OperandValue.find(".") != string::npos) {
        VariableTypeName = "REAL";
    } else {
        VariableTypeName = "INT";
    }

    return GetTmpVariable(VariableTypeName, InitValue);
}

void PCResourceImpl::OnStartup() {

    InitializeAllTasks();
}


