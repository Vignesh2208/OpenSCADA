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


#include "src/pc_emulator/include/pc_variable.h"
#include "src/pc_emulator/include/pc_datatype.h"
#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/include/task.h"
#include "include/pc_config_interface.h"
#include "include/pc_resource_interface.h"

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


PCConfigurationInterface::PCConfigurationInterface(string ConfigurationPath) {

    Utils::GenerateFullSpecification(ConfigurationPath, __specification);
    __ConfigurationName = "Default_Configuration";
    __ConfigurationPath = ConfigurationPath;
    
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
    
    
    mkdir("/tmp/OpenSCADA", 0777);
    mkdir(("/tmp/OpenSCADA/" + __ConfigurationName).c_str(), 0777);
    
    std::cout << "Allocating Shared Memory " << std::endl;

    __RAMMemory.AllocateSharedMemory(__RAMmemSize,
            "/tmp/OpenSCADA/" + __ConfigurationName + "/" + __ConfigurationName + "_RAM",
            __ConfigurationName + "_RamLock");

    /*__RAMMemory.AllocateSharedMemory(__RAMmemSize,
            "/tmp/" + __ConfigurationName + "_RAM",
            __ConfigurationName + "_RamLock");*/

    std::cout << "Allocated Shared Memory " << std::endl;
    __NumResources = __specification.machine_spec().num_cpus();
    assert(__NumResources >= 0);

    PCLogger->LogMessage(LogLevels::LOG_INFO, "Read Configuration !");
    RegisterAllElementaryDataTypes();
    RegisterAllComplexDataTypes();
    RegisterAllResources();

};

 void PCConfigurationInterface::RegisterAllResources() {
    for (auto& resource_spec : __specification.machine_spec().resource_spec()) {
        auto new_resource = 
            std::unique_ptr<PCResourceInterface>(new PCResourceInterface(this,
                                        resource_spec.resource_name(),
                                        resource_spec.input_mem_size_bytes(),
                                        resource_spec.output_mem_size_bytes()));
        new_resource->InitializeAllPoUVars();
        RegisteredResources->RegisterResource(
                resource_spec.resource_name(), std::move(new_resource));
        
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
        __access_pou_var->AllocateStorage("/tmp/OpenSCADA/" + __ConfigurationName + "/" + __ConfigurationName 
            + "_" + "__CONFIG_ACCESS__");
        __access_pou_var->__VariableDataType->__PoUType 
                        = pc_specification::PoUType::PROGRAM;
        Utils::ValidatePOUDefinition(__access_pou_var.get(), this);


        for (auto& field : 
            __specification.config_access_pou_var().datatype_field()) {
            int field_qualifier = field.has_field_qualifier() 
                ? field.field_qualifier()
                : pc_specification::FieldQualifiers::READ_ONLY;
            __AccessPathFieldQualifier.insert(std::make_pair(
                    field.field_name(), field_qualifier));
        }
    }
    std::cout << "Registered all resources ! " << std::endl;
}

void PCConfigurationInterface::RegisterAllElementaryDataTypes () {
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

void PCConfigurationInterface::RegisterAllComplexDataTypes() {

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
        std::cout << "Initializing GLOBAL VAR ****\n";
        __global_pou_var->AllocateStorage("/tmp/OpenSCADA/" + __ConfigurationName + "/" + __ConfigurationName
                + "_" + "__CONFIG_GLOBAL__");
        
        __global_pou_var->__VariableDataType->__PoUType 
                = pc_specification::PoUType::PROGRAM;
        Utils::ValidatePOUDefinition(__global_pou_var.get(), this);
        std::cout << "Finished Initializing Global VAR *****\n";
        
    }
    
    
    PCLogger->LogMessage(LogLevels::LOG_INFO,
                "Registered all Complex DataTypes!");
}

PCDataType * PCConfigurationInterface::LookupDataType(string DataTypeName) {
    return RegisteredDataTypes->GetDataType(DataTypeName);
}

void PCConfigurationInterface::Cleanup() {
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
}



PCVariable * PCConfigurationInterface::GetVariablePointerToMem(int ByteOffset,
        int BitOffset,string VariableDataTypeName) {

    int memType = MemType::RAM_MEM;
    assert(ByteOffset > 0 && BitOffset >= 0 && BitOffset < 8);
    assert(memType == MemType::RAM_MEM);

    if (Utils::ResolveAliasName(VariableDataTypeName, this) != "BOOL")
        BitOffset = 0;

    string VariableName =   "%%M" + std::to_string(ByteOffset)
                            + "." + std::to_string(BitOffset);
    // need to track and delete this variable later on
    auto got = __AccessedFields.find(VariableName
                    +"_" + VariableDataTypeName);
    if(got == __AccessedFields.end()) {
        __AccessedFields.insert(std::make_pair(VariableName + "_" 
                    + VariableDataTypeName,
            std::unique_ptr<PCVariable>(new PCVariable((PCConfiguration *)this,
                    nullptr, VariableName, VariableDataTypeName))));
        auto V = __AccessedFields.find(VariableName + "_" 
                    + VariableDataTypeName)->second.get();
        assert(V != nullptr);

        
        V->__MemoryLocation.SetMemUnitLocation(&__RAMMemory);
        V->__ByteOffset = ByteOffset;
        V->__BitOffset = BitOffset;
        V->__IsDirectlyRepresented = true;
        V->__MemAllocated = true;
        V->__VariableAttributes.FieldDetails.__FieldQualifier
                = pc_specification::FieldQualifiers::READ_WRITE;
        return V;
    } else {
        return got->second.get();
    }
}

PCVariable * PCConfigurationInterface::GetVariablePointerToResourceMem(
                        string ResourceName, int memType, int ByteOffset,
                        int BitOffset, string VariableDataTypeName){

    if (memType == MemType::RAM_MEM)
        return GetVariablePointerToMem(ByteOffset, BitOffset,
                        VariableDataTypeName);

    if (Utils::ResolveAliasName(VariableDataTypeName, this) != "BOOL")
        BitOffset = 0;

                        
    PCResourceInterface * desired_resource = (PCResourceInterface *)
            RegisteredResources->GetResource(ResourceName);
    if (!desired_resource)
        return nullptr;
    
    return desired_resource->GetVariablePointerToMem(memType, ByteOffset,
                                    BitOffset, VariableDataTypeName);

}

PCVariable * PCConfigurationInterface::GetExternVariable(string AccessPath) {
    
    DataTypeFieldAttributes FieldAttributes;
    PCVariable * return_var;

    if (!__access_pou_var) {
        PCLogger->LogMessage(LogLevels::LOG_INFO,
            "No ACCESS variables defined !");
        return nullptr;
    }

    if(__AccessPathFieldQualifier.find(AccessPath) == 
        __AccessPathFieldQualifier.end()) {
        PCLogger->LogMessage(LogLevels::LOG_INFO,
            "AccessPath: " + AccessPath + " Not Found !");
        return nullptr;
    }

    int field_qualifier = __AccessPathFieldQualifier.find(AccessPath)
                            ->second;

    if (!__access_pou_var->__VariableDataType->IsFieldPresent(AccessPath)) {
        PCLogger->LogMessage(LogLevels::LOG_INFO,
            "AccessPath: " + AccessPath + " Not Found !");
        return nullptr;
    }

    __access_pou_var->GetFieldAttributes(AccessPath, FieldAttributes);
    if (!Utils::IsFieldTypePtr(
        FieldAttributes.FieldDetails.__FieldInterfaceType)) {
        return_var = __access_pou_var->GetPtrToField(AccessPath);
        return_var->__VariableAttributes.FieldDetails.__FieldQualifier
            = field_qualifier;
    } else {

        
        if (FieldAttributes.FieldDetails.__FieldInterfaceType
            == FieldInterfaceType::VAR_EXPLICIT_STORAGE) {
            
            if(FieldAttributes.FieldDetails.__StorageMemType 
                    == MemType::RAM_MEM) {
                
                return_var = GetVariablePointerToMem(
                    FieldAttributes.FieldDetails.__StorageByteOffset,
                    FieldAttributes.FieldDetails.__StorageBitOffset,
                    FieldAttributes.FieldDetails.__FieldTypePtr->__DataTypeName);
                
                if(field_qualifier 
                    == pc_specification::FieldQualifiers::READ_ONLY)
                    return_var->__VariableAttributes
                        .FieldDetails.__FieldQualifier
                        = field_qualifier;

                
            } else {
                string ResourceName 
                = FieldAttributes.FieldDetails.__AssociatedResourceName;

                return_var = GetVariablePointerToResourceMem(ResourceName,
                    FieldAttributes.FieldDetails.__StorageMemType,
                    FieldAttributes.FieldDetails.__StorageByteOffset,
                    FieldAttributes.FieldDetails.__StorageBitOffset,
                    FieldAttributes.FieldDetails.__FieldTypePtr->__DataTypeName);

                if (FieldAttributes.FieldDetails.__StorageMemType
                        == MemType::OUTPUT_MEM && 
                    field_qualifier 
                    == pc_specification::FieldQualifiers::READ_ONLY)
                    return_var->__VariableAttributes
                        .FieldDetails.__FieldQualifier
                        = field_qualifier;
            }

        } else {
            if (FieldAttributes.FieldDetails.__FieldInterfaceType
                    == FieldInterfaceType::VAR_IN_OUT) {

                if (__global_pou_var->__VariableDataType->IsFieldPresent(
                    FieldAttributes.FieldDetails.__FullStorageSpec)) {

                    DataTypeFieldAttributes Attributes;
                    __global_pou_var->GetFieldAttributes(
                        FieldAttributes.FieldDetails.__FullStorageSpec,
                        Attributes);

                    if (Attributes.FieldDetails.__FieldInterfaceType
                        == FieldIntfType::VAR_EXPLICIT_STORAGE) {

                        std::cout << "Getting a Memory Pointer: M"
                            <<  Attributes.FieldDetails.__StorageByteOffset
                            << "." << Attributes.FieldDetails.__StorageBitOffset
                            << " of type: " 
                            << Attributes.FieldDetails
                                .__FieldTypePtr->__DataTypeName <<std::endl;
                        return_var =  GetVariablePointerToMem(
                            Attributes.FieldDetails.__StorageByteOffset,
                            Attributes.FieldDetails.__StorageBitOffset,
                            Attributes.FieldDetails
                                .__FieldTypePtr->__DataTypeName);
                        assert(return_var != nullptr);
                    } else {
                        if (!Utils::IsFieldTypePtr(Attributes
                            .FieldDetails.__FieldInterfaceType))
                            return_var = __global_pou_var->GetPtrToField(
                                FieldAttributes.FieldDetails.__FullStorageSpec);
                        else {
                            std::domain_error("Cannot access a Field which "
                            "is of type (VAR_IN_OUT/VAR_EXTERNAL/VAR_ACCESS). "
                            "This is because the ptr stored at this field is "
                            "created in another process's address space");
                        }
                    }

                    
                    
                    return_var->__VariableAttributes
                        .FieldDetails.__FieldQualifier
                        = field_qualifier;

                } else {
                    std::vector<string> results;
                    string FieldName 
                        = FieldAttributes.FieldDetails.__FullStorageSpec;
                    boost::split(results, FieldName,
                            boost::is_any_of("."), boost::token_compress_on);
                    PCResourceInterface * resource = (PCResourceInterface *)
                        RegisteredResources->GetResource(results[0]);
                    if (resource == nullptr) {
                        return_var = nullptr;
                    } else {
                        return_var = nullptr;
                        if (results.size() > 1) {
                            string ResourceVariableName = FieldName.substr(
                                    FieldName.find('.') + 1, string::npos);
                            return_var 
                                = resource->GetExternVariable(
                                    ResourceVariableName);

                            if (!return_var)
                                return_var 
                                    = resource->GetPoUFieldVariable(
                                        ResourceVariableName);
                            
                            if (return_var)
                                return_var->__VariableAttributes
                                            .FieldDetails.__FieldQualifier
                                            = field_qualifier;
                            
                        }
                        
                    }
                }

            } else {
                return_var = nullptr;
            }
        }
    }

    if (return_var != nullptr) {
        return_var->__VariableName = AccessPath;
    }

    
    
    return return_var;
}




uint8_t * PCConfigurationInterface::GetPtrToRAMMemory() {
    return (uint8_t *) __RAMMemory.GetStorageLocation().get();
}

uint8_t * PCConfigurationInterface::GetPtrToInputMemory(string ResourceName) {
    auto resource = RegisteredResources->GetResource(ResourceName);

    if (!resource)
        return nullptr;

    return (uint8_t *) resource->__InputMemory.GetStorageLocation().get();
}

uint8_t * PCConfigurationInterface::GetPtrToOutputMemory(string ResourceName) {
    auto resource = RegisteredResources->GetResource(ResourceName);

    if (!resource)
        return nullptr;

    return (uint8_t *) resource->__OutputMemory.GetStorageLocation().get();
}  




int PCConfigurationInterface::GetRAMMemSize() {
    return __RAMMemory.GetMemUnitSize();
}

int PCConfigurationInterface::GetInputMemSize(string ResourceName) {
    auto resource = RegisteredResources->GetResource(ResourceName);

    if (!resource)
        return -1;

    return resource->__InputMemory.GetMemUnitSize();
}

int PCConfigurationInterface::GetOutputMemSize(string ResourceName) {
     auto resource = RegisteredResources->GetResource(ResourceName);

    if (!resource)
        return -1;

    return resource->__OutputMemory.GetMemUnitSize();
}
