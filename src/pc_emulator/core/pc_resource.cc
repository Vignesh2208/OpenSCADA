#include <assert.h>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <regex>
#include <vector>

#include "pc_emulator/include/pc_pou_code_container.h"
#include "pc_emulator/include/pc_datatype.h"
#include "pc_emulator/include/pc_variable.h"
#include "pc_emulator/include/pc_resource.h"
#include "pc_emulator/include/pc_configuration.h"
#include "pc_emulator/include/utils.h"

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