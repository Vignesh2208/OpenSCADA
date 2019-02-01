#include <iostream>
#include <cstdint>
#include <cstring>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <vector>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include "src/pc_emulator/include/pc_variable.h"
#include "src/pc_emulator/include/pc_datatype.h"
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_resource.h"
#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/proto/configuration.pb.h"

using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldIntfType = pc_specification::FieldInterfaceType;

#define STRING(s) #s

string Utils::GetInstallationDirectory() {

    string InstallationDir = getpwuid(getuid())->pw_dir;

 
    /*
    vector<string> splits;
    boost::split(splits, InstallationDir, boost::is_any_of("/"));
    InstallationDir = "";
    for(int i = 0; i < (int)splits.size(); i++) {

        if (splits[i] == "OpenSCADA" || splits[i] == "SCADA_Sim") {
            InstallationDir += splits[i];
            break;
        } else {
            InstallationDir += splits[i] + "/";
        }
    }
    */
    return InstallationDir + "/OpenSCADA";
}

string Utils::GetElementaryDataTypeName(int Category) {
    switch(Category) {
        case DataTypeCategory::BOOL :   
                        return "BOOL";
        case DataTypeCategory::BYTE :     
                        return "BYTE";
        case DataTypeCategory::WORD :
                        return "WORD";  
        case DataTypeCategory::DWORD : 
                        return "DWORD";    
        case DataTypeCategory::LWORD :  
                        return "LWORD";  
        case DataTypeCategory::CHAR : 
                        return "CHAR";   
        case DataTypeCategory::INT :
                        return "INT";      
        case DataTypeCategory::SINT : 
                        return "SINT";    
        case DataTypeCategory::DINT : 
                        return "DINT";    
        case DataTypeCategory::LINT : 
                        return "LINT";    
        case DataTypeCategory::UINT :  
                        return "UINT";   
        case DataTypeCategory::USINT : 
                        return "USINT";    
        case DataTypeCategory::UDINT : 
                        return "UDINT";    
        case DataTypeCategory::ULINT :  
                        return "ULINT";   
        case DataTypeCategory::REAL : 
                        return "REAL";   
        case DataTypeCategory::LREAL :
                        return "LREAL";     
        case DataTypeCategory::TIME :
                        return "TIME";     
        case DataTypeCategory::DATE :
                        return "DATE";     
        case DataTypeCategory::TIME_OF_DAY : 
                        return "TOD";    
        case DataTypeCategory::DATE_AND_TIME :  
                        return "DT";   
    }
    return "NA";
}

int Utils::GetVarOpType(int varop) {
    switch(varop) {
        case VariableOps::ADD:
        case VariableOps::SUB:
        case VariableOps::MUL:
        case VariableOps::DIV:
        case VariableOps::MOD:    return VarOpType::ARITHMETIC;

        case VariableOps::AND:
        case VariableOps::OR:
        case VariableOps::XOR:
        case VariableOps::LS:
        case VariableOps::RS:     return VarOpType::BITWISE;

        default :   return VarOpType::RELATIONAL;
    }
}

bool Utils::ExtractFromStorageSpec(string StorageSpec, 
                        int * memType, int * ByteOffset, int * BitOffset) {
    if (!boost::starts_with(StorageSpec, "%%") || StorageSpec.length() < 4)
        return false;

    assert(memType != nullptr && ByteOffset != nullptr && BitOffset != nullptr);
    if (StorageSpec[1] == 'M') 
        *memType = (int)MemType::RAM_MEM;
    else if (StorageSpec[1] == 'I') 
        *memType = (int)MemType::INPUT_MEM;
    else if (StorageSpec[1] == 'Q') 
        *memType = (int)MemType::OUTPUT_MEM;
    else
        return false;

    if (StorageSpec[2] == 'W') {
        *ByteOffset = std::stoi(StorageSpec.substr(3,  string::npos));
        *BitOffset = 0;
    } else {
        *ByteOffset = std::stoi(StorageSpec.substr(2, StorageSpec.find('.')));
        *BitOffset = std::stoi(StorageSpec.substr(StorageSpec.find('.') + 1,
                                                    string::npos));
    }

    return true;
}


bool Utils::ExtractFromAccessStorageSpec(PCConfiguration * __configuration,
                        string StorageSpec,  int * memType, int * ByteOffset,
                        int * BitOffset) {
    if (StorageSpec.length() < 4)
        return false;

    assert(memType != nullptr && ByteOffset != nullptr && BitOffset != nullptr);

    if (boost::starts_with(StorageSpec,"%%")) {
        assert(StorageSpec[1] == 'M');
        *memType = (int)MemType::RAM_MEM;
        if (StorageSpec[2] == 'W') {
            *ByteOffset = std::stoi(StorageSpec.substr(3,  string::npos));
            *BitOffset = 0;
        } else {
            *ByteOffset = std::stoi(StorageSpec.substr(2, StorageSpec.find('.')));
            *BitOffset = std::stoi(StorageSpec.substr(StorageSpec.find('.') + 1,
                                                        string::npos));
        }
        return true;
    } else {
        std::vector<string> results;
        boost::split(results, StorageSpec, [](char c){return c == '.';});

        if(results.size() == 1) {// no dot found, this must be some field of global variable
            return false;
        } else {
            string candidate_resource = results[0];
            PCResource * resource = 
                __configuration->RegisteredResources.GetResource(
                                                        candidate_resource);

            if (resource == nullptr) // no resource by this name, must be some
                                     // nested field of global variable
                return false;
            else {
                string RemStorageSpec = StorageSpec.substr(
                    StorageSpec.find('.') + 1, string::npos);
                if (!Utils::ExtractFromStorageSpec(RemStorageSpec, memType,
                            ByteOffset, BitOffset))
                    return false; // this must be a nested field of some resource variable
                else
                    return true;
            }
        }
    }

    return false;
}

void Utils::InitializeDataType(PCConfiguration * __configuration,
                            PCDataType * __new_data_type,
                            const pc_specification::DataType& DataTypeSpec) {
    for (auto& field : DataTypeSpec.datatype_field()) {
        if (field.intf_type() != FieldIntfType::VAR_EXPLICIT_STORAGE) {
            if (field.has_dimension_1() && !field.has_dimension_2()) {
                __new_data_type->AddArrayDataTypeField(field.field_name(),
                        field.field_datatype_name(), field.dimension_1(),
                        field.initial_value(),
                        field.intf_type(), field.range_min(),
                        field.range_max());
            } else if (field.has_dimension_1() && field.has_dimension_2()) {

                __new_data_type->AddArrayDataTypeField(field.field_name(),
                        field.field_datatype_name(), field.dimension_1(),
                        field.dimension_2(),
                        field.initial_value(),
                        field.intf_type(), field.range_min(),
                        field.range_max());

            } else {
                __new_data_type->AddDataTypeField(field.field_name(),
                        field.field_datatype_name(), field.initial_value(),
                        field.intf_type(), field.range_min(),
                        field.range_max());
            }
        }
        else if (field.intf_type() 
                    == FieldIntfType::VAR_EXPLICIT_STORAGE
                && field.has_field_storage_spec()) {

            int mem_type = 0;
            int ByteOffset = 0;
            int BitOffset = 0;
            if (field.field_storage_spec().has_full_storage_spec()) {
                //extract memtype, byte and bit offsets from string specification
                
                if (!Utils::ExtractFromStorageSpec(
                        field.field_storage_spec().full_storage_spec(),
                        &mem_type, &ByteOffset, &BitOffset))
                    __configuration->PCLogger->RaiseException(
                        "Incorrectly formatted storage specification !");

            } else {
                mem_type = (int)field.field_storage_spec().mem_type();
                ByteOffset = field.field_storage_spec().byte_offset();
                BitOffset = field.field_storage_spec().bit_offset();
            }

            if (field.has_dimension_1() && !field.has_dimension_2()) {
                __new_data_type->AddArrayDataTypeFieldAT(field.field_name(),
                        field.field_datatype_name(), field.dimension_1(),
                        field.initial_value(),
                        field.range_min(),
                        field.range_max(),
                        mem_type, ByteOffset, BitOffset);
            } else if (field.has_dimension_1() && field.has_dimension_2()) {

                __new_data_type->AddArrayDataTypeFieldAT(field.field_name(),
                        field.field_datatype_name(), field.dimension_1(),
                        field.dimension_2(),
                        field.initial_value(),
                        field.range_min(),
                        field.range_max(),
                        mem_type, ByteOffset, BitOffset);

            } else {
                __new_data_type->AddDataTypeFieldAT(field.field_name(),
                        field.field_datatype_name(), field.initial_value(),
                        field.range_min(),
                        field.range_max(),
                        mem_type, ByteOffset, BitOffset);
            }

        }
    }
}

void Utils::InitializeAccessDataType(PCConfiguration * __configuration,
                                PCDataType * __new_data_type,
                                const pc_specification::DataType& DataTypeSpec) {

    for (auto& field : DataTypeSpec.datatype_field()) {
        if (field.intf_type() == FieldIntfType::VAR_ACCESS 
                && field.has_field_storage_spec()) {

            int mem_type = 0;
            int ByteOffset = 0;
            int BitOffset = 0;
            if (field.field_storage_spec().has_full_storage_spec()) {
                //extract memtype, byte and bit offsets from string specification
                
                if (!Utils::ExtractFromAccessStorageSpec(
                        __configuration,
                        field.field_storage_spec().full_storage_spec(),
                        &mem_type, &ByteOffset, &BitOffset)) {
                    if (field.has_dimension_1() && !field.has_dimension_2()) {
                        __new_data_type->AddArrayDataTypeField(field.field_name(),
                                field.field_datatype_name(), field.dimension_1(),
                                field.initial_value(),
                                field.intf_type(), field.range_min(),
                                field.range_max());
                    } else if (field.has_dimension_1() && field.has_dimension_2()) {

                        __new_data_type->AddArrayDataTypeField(field.field_name(),
                                field.field_datatype_name(), field.dimension_1(),
                                field.dimension_2(),
                                field.initial_value(),
                                field.intf_type(), field.range_min(),
                                field.range_max());

                    } else {
                        __new_data_type->AddDataTypeField(field.field_name(),
                                field.field_datatype_name(), field.initial_value(),
                                field.intf_type(), field.range_min(),
                                field.range_max());
                    }


                    continue; // these fields are added as a pointer. we will
                               // set these pointers later when a variable of this
                                // data type is created.
                }

            } else {
                mem_type = (int)field.field_storage_spec().mem_type();
                ByteOffset = field.field_storage_spec().byte_offset();
                BitOffset = field.field_storage_spec().bit_offset();

                assert(field.field_storage_spec().mem_type() == MemType::RAM_MEM);
            }

            if (field.has_dimension_1() && !field.has_dimension_2()) {
                __new_data_type->AddArrayDataTypeFieldAT(field.field_name(),
                        field.field_datatype_name(), field.dimension_1(),
                        field.initial_value(),
                        field.range_min(),
                        field.range_max(),
                        mem_type, ByteOffset, BitOffset);
            } else if (field.has_dimension_1() && field.has_dimension_2()) {

                __new_data_type->AddArrayDataTypeFieldAT(field.field_name(),
                        field.field_datatype_name(), field.dimension_1(),
                        field.dimension_2(),
                        field.initial_value(),
                        field.range_min(),
                        field.range_max(),
                        mem_type, ByteOffset, BitOffset);

            } else {
                __new_data_type->AddDataTypeFieldAT(field.field_name(),
                        field.field_datatype_name(), field.initial_value(),
                        field.range_min(),
                        field.range_max(),
                        mem_type, ByteOffset, BitOffset);
            }

        }
    }

}
