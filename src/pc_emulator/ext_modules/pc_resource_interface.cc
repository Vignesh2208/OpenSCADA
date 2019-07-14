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
#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/include/task.h"
#include "src/pc_emulator/include/insn_registry.h"
#include "include/pc_config_interface.h"
#include "include/pc_resource_interface.h"

using namespace std;
using namespace pc_emulator;
using namespace pc_specification;
using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldIntfType = pc_specification::FieldInterfaceType;


PCResourceInterface::PCResourceInterface(
    PCConfigurationInterface * configuration, 
    string ResourceName, int InputMemSize, int OutputMemSize) {
    __configuration = configuration;
    __ResourceName = ResourceName;
    __InputMemSize = InputMemSize;
    __OutputMemSize = OutputMemSize;

    assert(__InputMemSize > 0 && __OutputMemSize > 0);
    __InputMemory.AllocateSharedMemory(__InputMemSize,
                "/tmp/Output_" + ResourceName,
                ResourceName + "_IMemLock");
    __OutputMemory.AllocateSharedMemory(__OutputMemSize,
                "/tmp/Input_" + ResourceName,
                ResourceName + "_OMemLock");
}

void PCResourceInterface::RegisterPoUVariable(string VariableName,
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

void PCResourceInterface::InitializeAllSFBVars() {
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
        
    }

    for(auto it = __ResourcePoUVars.begin();
            it != __ResourcePoUVars.end(); it ++) {
        PCVariable * pou_var = it->second.get();
        pou_var->AllocateStorage("/tmp/" + __ResourceName
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

void PCResourceInterface::InitializeAllPoUVars() {

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
            }

            for(auto it = __ResourcePoUVars.begin();
                    it != __ResourcePoUVars.end(); it ++) {
                PCVariable * pou_var = it->second.get();
                pou_var->AllocateStorage("/tmp/" + __ResourceName
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
            break;
        }
    }

}


void PCResourceInterface::Cleanup() {
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

    std::cout << "Removing MMap files: " << __ResourceName << std::endl;
    __InputMemory.Cleanup();
    __OutputMemory.Cleanup();
    std::remove(("/tmp/Output_" + __ResourceName).c_str());
    std::remove(("/tmp/Input_" + __ResourceName).c_str());
}

void PCResourceInterface::OnStartup() {
}

PCVariable * PCResourceInterface::GetVariablePointerToMem(
    int memType, int ByteOffset, int BitOffset, string VariableDataTypeName) {

    assert(ByteOffset > 0 && BitOffset >= 0 && BitOffset < 8);
    assert(memType == MemType::INPUT_MEM || memType == MemType::OUTPUT_MEM);

    string memTypeStr;

    if (Utils::ResolveAliasName(VariableDataTypeName, 
                            __configuration) != "BOOL")
        BitOffset = 0;


    if (memType == MemType::INPUT_MEM)
        memTypeStr = "%%I";
    else 
        memTypeStr = "%%Q";
    string VariableName     = __ResourceName
                            + "." + memTypeStr
                            + std::to_string(ByteOffset)
                            + "." + std::to_string(BitOffset);

    // need to track and delete this variable later on
    auto got = __AccessedFields.find(VariableName
                    + "_" 
                    + VariableDataTypeName);

    if(got == __AccessedFields.end()) {

        __AccessedFields.insert(std::make_pair(VariableName +"_" 
                    + VariableDataTypeName,
                    std::unique_ptr<PCVariable>(
                        new PCVariable((PCConfiguration *)__configuration,
                            (PCResource *) this, VariableName,
                                    VariableDataTypeName))));
        PCVariable* V = __AccessedFields.find(VariableName
                    + "_" + VariableDataTypeName)->second.get();
        assert(V != nullptr);

        if(memType == MemType::INPUT_MEM)
            V->__MemoryLocation.SetMemUnitLocation(&__InputMemory);
        else 
            V->__MemoryLocation.SetMemUnitLocation(&__OutputMemory);

        V->__ByteOffset = ByteOffset;
        V->__BitOffset = BitOffset;
        V->__IsDirectlyRepresented = true;
        V->__MemAllocated = true;

        if (memType == MemType::OUTPUT_MEM) {
            V->__VariableAttributes.FieldDetails.__FieldQualifier
                = pc_specification::FieldQualifiers::READ_WRITE;
        } else {
            V->__VariableAttributes.FieldDetails.__FieldQualifier
                = pc_specification::FieldQualifiers::READ_ONLY;
        }
        
        return V;
    } else {
        return got->second.get();
    }
   
}

PCVariable * PCResourceInterface::GetExternVariable(string NestedFieldName) {
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

        if (FieldAttributes.FieldDetails.__FieldInterfaceType
            == FieldIntfType::VAR_EXPLICIT_STORAGE) {

            if (FieldAttributes.FieldDetails.__StorageMemType
                == MemType::INPUT_MEM ||
                FieldAttributes.FieldDetails.__StorageMemType
                == MemType::OUTPUT_MEM) {
                return GetVariablePointerToMem(
                    FieldAttributes.FieldDetails.__StorageMemType,
                    FieldAttributes.FieldDetails.__StorageByteOffset,
                    FieldAttributes.FieldDetails.__StorageBitOffset,
                    FieldAttributes.FieldDetails.__FieldTypePtr->__DataTypeName  
                );
            } else {
                return __configuration->GetVariablePointerToMem(
                    FieldAttributes.FieldDetails.__StorageByteOffset,
                    FieldAttributes.FieldDetails.__StorageBitOffset,
                    FieldAttributes.FieldDetails.__FieldTypePtr->__DataTypeName);
            }
        }

        if (!Utils::IsFieldTypePtr(FieldAttributes
                .FieldDetails.__FieldInterfaceType))
            return global_var->GetPtrToField(NestedFieldName);
        else {
            std::domain_error("Cannot access a Field which is of type (VAR_IN_OUT/VAR_EXTERNAL/VAR_ACCESS). "
            "This is because the ptr stored at this field is created in "
            "another process's address space");

            return nullptr;
        }
    }
    else {
        // this may be referring to a PoU variable
        auto got = GetPoUVariable(NestedFieldName);
            if (got == nullptr) {
                // This may be referring to a Variable declared within
                // the POU
                return GetPOUGlobalVariable(NestedFieldName);
        } else {
            std::domain_error("Cannot access a POU variable itself !");
            return nullptr;
        }
    }
}

PCVariable * PCResourceInterface::GetPoUVariable(string PoUName) {
    auto got =  __ResourcePoUVars.find(PoUName);
    if (got == __ResourcePoUVars.end()) {
        return nullptr;
    }
    return got->second.get();
}

PCVariable * PCResourceInterface::GetPoUFieldVariable(
    string NestedPoUFieldName){
    
    std::vector<string> results;

    boost::split(results, NestedPoUFieldName,
                boost::is_any_of("."), boost::token_compress_on);
    DataTypeFieldAttributes FieldAttributes;
    if (results.size() <= 1) {
        __configuration->PCLogger->LogMessage(LogLevels::LOG_INFO,
            "Nested FieldName: " + NestedPoUFieldName 
            + " in incorrect format !");
        return nullptr;
    }


    // NestedFieldName of the form: resource_name.pou_name.nested_variable_name
    string PoUName = results[0];

    string PoUVariableName = NestedPoUFieldName.substr(
                    results[0].length() + 1, string::npos);
    auto pou_var = __ResourcePoUVars.find(PoUName)->second.get();
    if (pou_var == nullptr) {
        __configuration->PCLogger->LogMessage(LogLevels::LOG_INFO,
            "POU NAME: " + PoUName + " not found !");
        return nullptr;
    }

    if (pou_var->__VariableDataType->IsFieldPresent(PoUVariableName)) {
        DataTypeFieldAttributes FieldAttributes;
        pou_var->GetFieldAttributes(PoUVariableName, FieldAttributes);
        if (!Utils::IsFieldTypePtr(
            FieldAttributes.FieldDetails.__FieldInterfaceType))
            return pou_var->GetPtrToField(PoUVariableName);
        else {
           __configuration->PCLogger->LogMessage(LogLevels::LOG_INFO,
            "Cannot access a POU variable field which is a pointer "
            " because this pointer would have been set in a different "
            "process's address space!"); 
            return nullptr;
        }
    } else {
        __configuration->PCLogger->LogMessage(LogLevels::LOG_INFO,
            "POU NAME: " + PoUName + " Variable: " + PoUVariableName 
            + " not found !");
        return nullptr;
    }
}

// Returns a global variable or a directly represented variable declared
// by any of the POUs defined in this resource provided the NestedFieldName
// matches
PCVariable * PCResourceInterface::GetPOUGlobalVariable(string NestedFieldName) {
    assert(!NestedFieldName.empty());
    std::vector<string> results;
    boost::split(results, NestedFieldName, boost::is_any_of("."),
            boost::token_compress_on);

    if (results.size() <= 1) {
        __configuration->PCLogger->LogMessage(LogLevels::LOG_INFO,
            "Nested FieldName: " + NestedFieldName 
            + " in incorrect format in GetPOUGlobalVariable !");
        return nullptr;
    }

    string PoUName = results[0];
    string PoUVariableName = NestedFieldName.substr(
                    results[0].length() + 1, string::npos);

    if (__ResourcePoUVars.find(PoUName) == __ResourcePoUVars.end()) {
        __configuration->PCLogger->LogMessage(LogLevels::LOG_INFO,
            "PoUName: " + PoUName 
            + " Not found !");
        return nullptr;
    }

    auto var = __ResourcePoUVars.find(PoUName)->second.get();        
    if (var->__VariableDataType->IsFieldPresent(PoUVariableName)) {
        DataTypeFieldAttributes FieldAttributes;
        var->GetFieldAttributes(PoUVariableName, 
                                FieldAttributes);
        if (FieldAttributes.FieldDetails.__FieldInterfaceType
            == FieldIntfType::VAR_GLOBAL) {
            auto V =  var->GetPtrToField(PoUVariableName);
            V->__VariableAttributes.FieldDetails.__FieldQualifier
                = pc_specification::FieldQualifiers::READ_WRITE;
        }

        if (FieldAttributes.FieldDetails.__FieldInterfaceType
            == FieldIntfType::VAR_EXPLICIT_STORAGE) {
            if (FieldAttributes.FieldDetails.__StorageMemType
                == MemType::INPUT_MEM ||
                FieldAttributes.FieldDetails.__StorageMemType
                == MemType::OUTPUT_MEM) {
                return GetVariablePointerToMem(
                    FieldAttributes.FieldDetails.__StorageMemType,
                    FieldAttributes.FieldDetails.__StorageByteOffset,
                    FieldAttributes.FieldDetails.__StorageBitOffset,
                    FieldAttributes.FieldDetails
                        .__FieldTypePtr->__DataTypeName);
            } else {
                return __configuration->GetVariablePointerToMem(
                    FieldAttributes.FieldDetails.__StorageByteOffset,
                    FieldAttributes.FieldDetails.__StorageBitOffset,
                    FieldAttributes.FieldDetails
                        .__FieldTypePtr->__DataTypeName);
            }
        }
    }

    return nullptr;
}



