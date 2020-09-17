#include <iostream>
#include <cstdint>
#include <cstring>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <fstream>
#include <cstdio>

#include <bits/stdc++.h> 
#include <iostream> 
#include <sys/stat.h> 
#include <sys/types.h> 

extern "C"
{
    #include <Kronos_functions.h>   
}

#include "src/pc_emulator/include/pc_variable.h"
#include "src/pc_emulator/include/pc_datatype.h"
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_resource.h"
#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/include/task.h"
#include "src/pc_emulator/include/kronos_api.h"



using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldIntfType = pc_specification::FieldInterfaceType;

using std::string;
using std::vector;
using std::unordered_map;
using std::to_string;


PCConfigurationImpl::PCConfigurationImpl(string ConfigurationPath,
        bool enable_kronos, long per_round_inc_ns) {


       Utils::GenerateFullSpecification(ConfigurationPath, __specification);

        __ConfigurationName = "Default_Configuration";
        __ConfigurationPath = ConfigurationPath;

        stop = false;
        this->enable_kronos = enable_kronos;
        this->per_round_inc_ns = per_round_inc_ns;
        
        int logLevel = LogLevels::LOG_INFO;
        string logFilePath = "";
        if (__specification.has_config_name())
            __ConfigurationName = __specification.config_name();
        
        
        if (__specification.has_log_level())
            logLevel = __specification.log_level();

        if (__specification.has_log_file_path())
            logFilePath = __specification.log_file_path();

        PCLogger = std::unique_ptr<Logger>(new Logger(
            this, logFilePath, logLevel));
        RegisteredDataTypes = std::unique_ptr<DataTypeRegistry>(new 
            DataTypeRegistry((PCConfiguration *)this));
        RegisteredResources = new ResourceRegistry((PCConfiguration *)this);
        __RAMmemSize = __specification.machine_spec().ram_mem_size_bytes();
        assert(__RAMmemSize > 0);
        
        std::cout << "Allocating Shared RAM Memory " << std::endl;
    	//assert (mkdir(("/tmp/OpenSCADA/" + __ConfigurationName).c_str(), 0777) != 0);
	mkdir("/tmp/OpenSCADA", 0777);
    	mkdir(("/tmp/OpenSCADA/" + __ConfigurationName).c_str(), 0777);

        __RAMMemory.AllocateSharedMemory(__RAMmemSize,
                "/tmp/OpenSCADA/" + __ConfigurationName + "/" + __ConfigurationName + "_RAM",
                __ConfigurationName + "_RamLock");
	/*__RAMMemory.AllocateSharedMemory(__RAMmemSize,
                "/tmp/" + __ConfigurationName + "_RAM",
                __ConfigurationName + "_RamLock");*/

        //__RAMMemory.AllocateStaticMemory(__RAMmemSize);
        __NumResources = __specification.machine_spec().num_cpus();
        assert(__NumResources >= 0);

        PCLogger->LogMessage(LogLevels::LOG_INFO, "Read Configuration !");
        RegisterAllElementaryDataTypes();
        RegisterAllResources();
        RegisterAllComplexDataTypes();
        InitializeAllPOUVariables();



};

 void PCConfigurationImpl::RegisterAllResources() {
    for (auto& resource_spec : __specification.machine_spec().resource_spec()) {
        auto new_resource = 
            std::unique_ptr<PCResourceImpl>(new PCResourceImpl(this,
                                        resource_spec.resource_name(),
                                        resource_spec.input_mem_size_bytes(),
                                        resource_spec.output_mem_size_bytes()));

        __ResourceManagers.insert(std::make_pair(
            resource_spec.resource_name(), 
            std::unique_ptr<ResourceManager>(new ResourceManager(
                new_resource.get()))));
        //new_resource->InitializeAllPoUVars();
        RegisteredResources->RegisterResource(
                resource_spec.resource_name(), std::move(new_resource));

        
        
    }
}

void PCConfigurationImpl::RegisterAllElementaryDataTypes () {
    for (int Category = DataTypeCategory::BOOL ; 
                Category != DataTypeCategory::ARRAY; Category++) {

        
        string DataTypeName, InitValue;

        PCLogger->LogMessage(LogLevels::LOG_INFO, "Next Elementary Category "
            + std::to_string(Category));
        switch(Category) {
            case DataTypeCategory::BOOL :   
                            DataTypeName = "BOOL", InitValue = "0";
                            break;
            case DataTypeCategory::BYTE :     
                                DataTypeName = "BYTE", InitValue = "16#0";
                                break;
            case DataTypeCategory::WORD :     
                                DataTypeName = "WORD", InitValue = "16#0";
                                break;
            case DataTypeCategory::DWORD :     
                                DataTypeName = "DWORD", InitValue = "16#0";
                                break;
            case DataTypeCategory::LWORD :    
                                DataTypeName = "LWORD", InitValue = "16#0";
                                break;
            case DataTypeCategory::CHAR :    
                                DataTypeName = "CHAR", InitValue  = "";
                                break;
            case DataTypeCategory::INT :      
                                DataTypeName = "INT", InitValue = "0";
                                break;
            case DataTypeCategory::SINT :     
                                DataTypeName = "SINT", InitValue = "0";
                                break;
            case DataTypeCategory::DINT :     
                                DataTypeName = "DINT", InitValue = "0";
                                break;
            case DataTypeCategory::LINT :     
                                DataTypeName = "LINT", InitValue = "0";
                                break;
            case DataTypeCategory::UINT :     
                                DataTypeName = "UINT", InitValue = "0";
                                break;
            case DataTypeCategory::USINT :     
                                DataTypeName = "USINT", InitValue = "0";
                                break;
            case DataTypeCategory::UDINT :     
                                DataTypeName = "UDINT", InitValue = "0";
                                break;
            case DataTypeCategory::ULINT :     
                                DataTypeName = "ULINT", InitValue = "0";
                                break;
            case DataTypeCategory::REAL :     
                                DataTypeName = "REAL", InitValue = "0.0";
                                break;
            case DataTypeCategory::LREAL :     
                                DataTypeName = "LREAL", InitValue = "0.0";
                                break;
            case DataTypeCategory::TIME :     
                                DataTypeName = "TIME", InitValue = "t#0s";
                                break;
            case DataTypeCategory::DATE :     
                                DataTypeName = "DATE", InitValue = "d#0001-01-01";
                                break;
            case DataTypeCategory::TIME_OF_DAY :     
                                DataTypeName = "TOD", InitValue = "tod#00:00:00";
                                break;
            case DataTypeCategory::DATE_AND_TIME :     
                                DataTypeName = "DT";
                                InitValue = "dt#0001-01-01-00:00:00";
                                break;
            default:            DataTypeName = "NA";
                                break;
     
        }

        __DataTypeDefaultInitialValues.insert(std::make_pair(
                                                Category, InitValue));

        if(DataTypeName != "NA") {
            PCLogger->LogMessage(LogLevels::LOG_INFO, 
                    "Registering Elementary DataType: " + DataTypeName);
            auto newDataType = std::unique_ptr<PCDataType>(new PCDataType(
                (PCConfiguration *) this, DataTypeName, DataTypeName,
                static_cast<DataTypeCategory>(Category), InitValue));   
            RegisteredDataTypes->RegisterDataType(DataTypeName,
                                    std::move(newDataType));  
        } 
    }

    PCLogger->LogMessage(LogLevels::LOG_INFO, "Registering STRING DataType");
    // note that max string length is 1000 chars
    auto stringDataType = std::unique_ptr<PCDataType>(new PCDataType(
        (PCConfiguration *)this, "STRING", "CHAR", 1000,
        DataTypeCategory::ARRAY));
    RegisteredDataTypes->RegisterDataType(
            "STRING", std::move(stringDataType));

    __DataTypeDefaultInitialValues.insert(std::make_pair(
                                            DataTypeCategory::ARRAY, ""));
    __DataTypeDefaultInitialValues.insert(std::make_pair(
                                            DataTypeCategory::DERIVED, ""));
    __DataTypeDefaultInitialValues.insert(std::make_pair(
                                            DataTypeCategory::POU, ""));

    PCLogger->LogMessage(LogLevels::LOG_INFO, 
            "Registered all elementary datatypes!\n");
}

void PCConfigurationImpl::RegisterAllComplexDataTypes() {

    for (auto & datatype_decl : __specification.datatype_declaration()) {
        if (datatype_decl.datatype_category() != DataTypeCategory::POU
        && datatype_decl.datatype_category() != DataTypeCategory::ARRAY
        && datatype_decl.datatype_category() != DataTypeCategory::NOT_ASSIGNED) {
            if (datatype_decl.datatype_field_size() > 0
                && datatype_decl.has_datatype_spec()) {
                    PCLogger->RaiseException("Incorrect proto specification "
                     "cannot have array_spec and datatype_fields specified "
                     "at the same time!");
            }

            std::unique_ptr<PCDataType> new_data_type;
            PCLogger->LogMessage(LogLevels::LOG_INFO, 
                "Registering New DataType: " + datatype_decl.name());

            if (datatype_decl.has_datatype_spec()) {
                assert (datatype_decl.datatype_category() 
                    != DataTypeCategory::DERIVED);

                string elementary_datatype_name 
                    = Utils::GetElementaryDataTypeName(
                        datatype_decl.datatype_category());
                string InitValue 
                    = datatype_decl.datatype_spec().has_initial_value() ? 
                      datatype_decl.datatype_spec().initial_value(): "" ;
                s64 range_min =  datatype_decl.datatype_spec().has_range_min() ? 
                      datatype_decl.datatype_spec().range_min() : LLONG_MIN;
                s64 range_max = datatype_decl.datatype_spec().has_range_max() ? 
                      datatype_decl.datatype_spec().range_max(): LLONG_MAX;
                

                if (datatype_decl.datatype_spec().has_dimension_1()) {
                    if (!datatype_decl.datatype_spec().has_dimension_2()) {
                        assert(datatype_decl.datatype_spec().dimension_1() > 0);
                        
                        new_data_type = std::unique_ptr<PCDataType>(
                            new PCDataType((PCConfiguration *)this, 
                            datatype_decl.name(), elementary_datatype_name,
                            datatype_decl.datatype_spec().dimension_1(),
                            datatype_decl.datatype_category(), InitValue,
                            range_min, range_max));

                    } else {// 2-d array datatype
                        assert(datatype_decl.datatype_spec().dimension_1() > 0
                            && datatype_decl.datatype_spec().dimension_2() > 0);
                        new_data_type = std::unique_ptr<PCDataType>(
                                new PCDataType((PCConfiguration *)this, 
                                datatype_decl.name(), elementary_datatype_name,
                                datatype_decl.datatype_spec().dimension_1(),
                                datatype_decl.datatype_spec().dimension_2(),
                                datatype_decl.datatype_category(), InitValue,
                                range_min, range_max));
                    }
                } else {
                    new_data_type = std::unique_ptr<PCDataType>(
                        new PCDataType((PCConfiguration *)this,
                        datatype_decl.name(), 
                        elementary_datatype_name,
                        datatype_decl.datatype_category(),
                        InitValue, range_min, range_max));
                }

                


            } else {
                assert(datatype_decl.datatype_category() 
                    == DataTypeCategory::DERIVED 
                    && datatype_decl.datatype_field_size() > 0);
                new_data_type = std::unique_ptr<PCDataType>(
                    new PCDataType((PCConfiguration *)this,
                    datatype_decl.name(),
                    datatype_decl.name(), DataTypeCategory::DERIVED));
                Utils::InitializeDataType(this, new_data_type.get(),
                                        datatype_decl);
            }

            
            assert(new_data_type.get() != nullptr);
            RegisteredDataTypes->RegisterDataType(datatype_decl.name(), 
                                        std::move(new_data_type));
        }

    }
    
    // Now register global and access variables
    if (!__specification.has_config_global_pou_var())
        __global_pou_var = nullptr;
    else {

        auto global_var_type = std::unique_ptr<PCDataType>(
                    new PCDataType((PCConfiguration *)this,
                    "__CONFIG_GLOBAL__",
                    "__CONFIG_GLOBAL__", DataTypeCategory::POU));

        Utils::InitializeDataType(this, global_var_type.get(),
             __specification.config_global_pou_var());
        
        RegisteredDataTypes->RegisterDataType("__CONFIG_GLOBAL__", 
                                std::move(global_var_type));

        __global_pou_var = std::unique_ptr<PCVariable>
                (new PCVariable((PCConfiguration *)this, nullptr,
                                "__CONFIG_GLOBAL_VAR__", "__CONFIG_GLOBAL__"));
    }
    
    PCLogger->LogMessage(LogLevels::LOG_INFO,
                "Registered all Complex DataTypes!");
}

void PCConfigurationImpl::InitializeAllPOUVariables() {
    if (__global_pou_var) {
        PCLogger->LogMessage(LogLevels::LOG_INFO,
            "Initializing Configuration GLOBAL variables ****");

        __global_pou_var->AllocateAndInitialize("/tmp/OpenSCADA/" + __ConfigurationName + "/" + __ConfigurationName
                + "_" + "__CONFIG_GLOBAL__");
        
        __global_pou_var->__VariableDataType->__PoUType 
                = pc_specification::PoUType::PROGRAM;
        Utils::ValidatePOUDefinition(__global_pou_var.get(), this);
        PCLogger->LogMessage(LogLevels::LOG_INFO, 
        "Finished Initializing Configuration GLOBAL Variables *****");
    }

    for (auto& resource_spec 
            : __specification.machine_spec().resource_spec()) {
        auto new_resource = (PCResourceImpl *)
            RegisteredResources->GetResource(
                resource_spec.resource_name());            
        new_resource->InitializeAllPoUVars();
    }

    if (!__specification.has_config_access_pou_var())
        __access_pou_var = nullptr;
    else {
        auto access_var_type = std::unique_ptr<PCDataType>(
                new PCDataType((PCConfiguration *)this,
                "__CONFIG_ACCESS__", "__CONFIG_ACCESS__",
                    DataTypeCategory::POU));

        Utils::InitializeAccessDataType(this, access_var_type.get(),
             __specification.config_access_pou_var());

        RegisteredDataTypes->RegisterDataType("__CONFIG_ACCESS__", 
                                std::move(access_var_type));
        
        __access_pou_var = std::unique_ptr<PCVariable>
                    (new PCVariable((PCConfiguration *)this, nullptr,
                                "__CONFIG_ACCESS_VAR__", "__CONFIG_ACCESS__"));
        
    }

    if (__access_pou_var) {

        PCLogger->LogMessage(LogLevels::LOG_INFO,
         "Initializing ACCESS Paths ****");
        __access_pou_var->AllocateAndInitialize("/tmp/OpenSCADA/" + __ConfigurationName + "/" + __ConfigurationName
            + "_" + "__CONFIG_ACCESS__");
        __access_pou_var->__VariableDataType->__PoUType 
                        = pc_specification::PoUType::PROGRAM;
        Utils::ValidatePOUDefinition(__access_pou_var.get(), this);
        // now we need to set pointers to some fields of this variable

 

        for (auto& field : 
                    __specification.config_access_pou_var().datatype_field()) {
            
            if (field.field_storage_spec().has_full_storage_spec()) {
                //extract memtype, byte and bit offsets from string specification
                int mem_type, ByteOffset, BitOffset;
                string CandidateResourceName;
                if (!Utils::ExtractFromAccessStorageSpec(
                        this,
                        field.field_storage_spec().full_storage_spec(),
                        &mem_type, &ByteOffset, &BitOffset,
                        CandidateResourceName)) {
                    // these are the fields which are selected
                    string StorageSpec 
                        = field.field_storage_spec().full_storage_spec();

                    //std::cout << "Getting Ptr for: " << StorageSpec << std::endl;

                    PCVariable * desired_ptr = Utils::GetVariable(
                        StorageSpec, this);
                    //std::cout << "Got Ptr for: " << StorageSpec << std::endl;
                    if(!desired_ptr) {
                        PCLogger->RaiseException("Error in storage spec"
                                " of access variable: " + StorageSpec);
                    }

                    if (desired_ptr->__IsVariableContentTypeAPtr) {
                        throw std::domain_error("ACCESS path: " 
                            + field.field_name()
                            + " points to another pointer ! ACCESS paths "
                            " can only be set for VAR_GLOBAL, VAR_INPUT,"
                            "VAR_OUTPUT or directly represented variables");
                    }

                    //set this as a ptr to the field of acess variable
                    __access_pou_var->SetField(field.field_name(),
                                    desired_ptr);

                }
            }
        }

        
    } else {

        /*for (auto& resource_spec : __specification.machine_spec().resource_spec()) {
            auto new_resource = (PCResourceImpl *)RegisteredResources->GetResource(
                    resource_spec.resource_name());
            
            new_resource->ResolveAllExternalFields();
        
        }*/
    }

    for (auto& resource_spec : __specification.machine_spec().resource_spec()) {
            auto new_resource = (PCResourceImpl *)
                    RegisteredResources->GetResource(
                            resource_spec.resource_name());            
            new_resource->ResolveAllExternalFields();
    }
    PCLogger->LogMessage(LogLevels::LOG_INFO,
        "Finished Initializing ACCESS Variables *****");
    PCLogger->LogMessage(LogLevels::LOG_INFO,
        "Finished Initializing Resource POU Variables *****");
}

PCVariable * PCConfigurationImpl::GetPOU(string NestedPoUName) {
    assert(!NestedPoUName.empty());
    std::vector<string> results;

    boost::split(results, NestedPoUName,
                boost::is_any_of("."), boost::token_compress_on);
    DataTypeFieldAttributes FieldAttributes;
    PCResourceImpl * resource = (PCResourceImpl *)
                RegisteredResources->GetResource(results[0]);
    if (resource != nullptr && results.size() == 2) {
        return resource->GetPOU(results[1]);
    }
    return nullptr;
}

PCVariable * PCConfigurationImpl::GetVariablePointerToMem(int ByteOffset,
        int BitOffset,string VariableDataTypeName) {

    int memType = MemType::RAM_MEM;
    assert(ByteOffset > 0 && BitOffset >= 0 && BitOffset < 8);
    assert(memType == MemType::RAM_MEM);

    if (Utils::ResolveAliasName(VariableDataTypeName, this) != "BOOL")
        BitOffset = 0;

    string VariableName = __ConfigurationName 
                            + "." + VariableDataTypeName
                            + "." + std::to_string(memType)
                            + "." + std::to_string(ByteOffset)
                            + "." + std::to_string(BitOffset);
    // need to track and delete this variable later on
    auto got = __AccessedFields.find(VariableName);
    if(got == __AccessedFields.end()) {
        __AccessedFields.insert(std::make_pair(VariableName,
            std::unique_ptr<PCVariable>(new PCVariable((PCConfiguration *)this,
                    nullptr, VariableName, VariableDataTypeName))));
        auto V = __AccessedFields.find(VariableName)->second.get();
        assert(V != nullptr);

        
        V->__MemoryLocation.SetMemUnitLocation(&__RAMMemory);
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

PCVariable * PCConfigurationImpl::GetVariablePointerToResourceMem(
                        string ResourceName, int memType, int ByteOffset,
                        int BitOffset, string VariableDataTypeName){

    if (memType == MemType::RAM_MEM)
        return GetVariablePointerToMem(ByteOffset, BitOffset,
                        VariableDataTypeName);
                        
    PCResource * desired_resource = RegisteredResources->GetResource(
                                        ResourceName);
    if (!desired_resource)
        return nullptr;
    
    return desired_resource->GetVariablePointerToMem(memType, ByteOffset,
                                    BitOffset, VariableDataTypeName);

}

PCVariable * PCConfigurationImpl::GetExternVariable(string NestedFieldName) {
    assert(!NestedFieldName.empty());
    std::vector<string> results;

    boost::split(results, NestedFieldName,
                boost::is_any_of("."), boost::token_compress_on);
    DataTypeFieldAttributes FieldAttributes;
    PCResource * resource = RegisteredResources->GetResource(results[0]);
    if (resource == nullptr) {
        if (__global_pou_var != nullptr
        && __global_pou_var->__VariableDataType->IsFieldPresent(
                                                NestedFieldName)){
            __global_pou_var->GetFieldAttributes(NestedFieldName, FieldAttributes);
            if (!Utils::IsFieldTypePtr(
                FieldAttributes.FieldDetails.__FieldInterfaceType))
                return __global_pou_var->GetPtrToField(NestedFieldName);
            else
                return __global_pou_var->GetPtrStoredAtField(NestedFieldName);
        }
        // Note: ACCESS Paths are not checked here because they are only
        // intended for inter configuration communication. POUs within one
        // configuration may not access these ACCESS paths.
        return nullptr;
        
    } else {
        
        if (results.size() > 1) {
            string ResourceVariableName = NestedFieldName.substr(
                    NestedFieldName.find('.') + 1, string::npos);
            return resource->GetExternVariable(ResourceVariableName);
        }
        return nullptr;
    }

}

PCVariable * PCConfigurationImpl::GetAccessPathVariable(string AccessPath) {

    DataTypeFieldAttributes FieldAttributes;

    if (!__access_pou_var || AccessPath.empty())
        return nullptr;
    
    if (__access_pou_var->__VariableDataType->IsFieldPresent(AccessPath)) {
        __access_pou_var->GetFieldAttributes(AccessPath,
                                            FieldAttributes);                                       
        if (!Utils::IsFieldTypePtr(
            FieldAttributes.FieldDetails.__FieldInterfaceType))
            return __access_pou_var->GetPtrToField(AccessPath);
        else
            return __access_pou_var->GetPtrStoredAtField(AccessPath);
    
    } else
        return nullptr;
    
}

PCDataType * PCConfigurationImpl::LookupDataType(string DataTypeName) {
    return RegisteredDataTypes->GetDataType(DataTypeName);
}

void PCConfigurationImpl::RunPLC() {

    
    LaunchPLC();
    WaitForCompletion();
}

void PCConfigurationImpl::LaunchPLC() {
    for (auto it = __ResourceManagers.begin();
        it != __ResourceManagers.end(); it++) {

        PCLogger->LogMessage(LogLevels::LOG_INFO, "Launching Resource: "
            + it->first);

        auto ResourceManager = it->second.get();
        assert(ResourceManager != nullptr);

        LaunchedResources.push_back(ResourceManager->LaunchResourceManager());
    }

}

void PCConfigurationImpl::WaitForCompletion() {
    PCLogger->LogMessage(LogLevels::LOG_INFO, 
        "Waiting for all resources to finish");

    for (auto th = LaunchedResources.begin(); th != LaunchedResources.end();
        th++) {
        th->join();
    }

    PCLogger->LogMessage(LogLevels::LOG_INFO, 
        "Finished executing all programs");
}

void PCConfigurationImpl::StopAllResources() {
    for (auto it = __ResourceManagers.begin();
        it != __ResourceManagers.end(); it++) {

        PCLogger->LogMessage(LogLevels::LOG_INFO, "Stopping Resource: "
            + it->first);

        auto ResourceManager = it->second.get();
        ResourceManager->__AssociatedResource->clock->__stop = true;
    }
}

void PCConfigurationImpl::Cleanup() {
    for ( auto it = __AccessedFields.begin(); it != __AccessedFields.end(); 
            ++it ) {
            PCVariable * __AccessedVariable = it->second.get();
            __AccessedVariable->Cleanup();
    }

    if (__global_pou_var != nullptr) {
        __global_pou_var->Cleanup();
    }

    if (__access_pou_var != nullptr) {
        __access_pou_var->Cleanup();
    }

    std::cout << "Configuration: Removing MMap Files" << std::endl;
    __RAMMemory.Cleanup();
    std::remove(("/tmp/OpenSCADA/" + __ConfigurationName + "/" + __ConfigurationName).c_str());
    RegisteredResources->Cleanup();
    RegisteredDataTypes->Cleanup();

    delete RegisteredResources;

    std::cout << "Configuration: Cleaned up" << std::endl;
}
