#include <iostream>
#include <cstdint>
#include <cstring>

#include "pc_emulator/include/pc_variable.h"
#include "pc_emulator/include/pc_datatype.h"
#include "pc_emulator/include/pc_configuration.h"
#include "pc_emulator/include/pc_resource.h"

using namespace std;
using namespace pc_emulator;

PCConfiguration::PCConfiguration(string ConfigurationPath):
    __ConfigurationPath(ConfigurationPath),
    RegisteredDataTypes(this), RegisteredResources(this) {

        bool retValue = false;
        int fileDescriptor = open(ConfigurationPath.c_str(),
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
                                        &__specification)) {
            std::cerr << std::endl << "Failed to parse spec file!" 
                << std::endl;
            exit(-1);
        }


        
        __ConfigurationName = "Default_Configuration";
        int logLevel = LOG_LEVELS::LOG_INFO;
        string logFilePath = "";
        if (__specification.has_config_name())
            __ConfigurationName = __specification.config_name();
        
        
        if (__specification.has_log_level())
            logLevel = __specification.log_level();
        
        if (__specification.has_log_file_path())
            logFilePath = __specification.log_file_path();

        PCLogger = new Logger(this, logFilePath, logLevel);
        __RAMmemSize = __specification.machine_spec().ram_mem_size_bytes();
        assert(__RAMmemSize > 0);
        __RAMMemory.AllocateStaticMemory(__RAMmemSize);
        __NumResources = __specification.machine_spec().num_cpus();
        assert(__NumResources > 0);

};

void PCConfiguration::RegisterAllResources() {
    for (auto& resource_spec : __specification.machine_spec().resource_spec()) {
        PCResource * new_resource = new PCResource(this,
                                        resource_spec.resource_name(),
                                        resource_spec.input_mem_size_bytes(),
                                        resource_spec.output_mem_size_bytes());
        RegisteredResources.RegisterResource(
                        resource_spec.resource_name(), new_resource);
    }
}

void PCConfiguration::RegisterAllElementaryDataTypes () {
    for (int Category = DataTypeCategories::BOOL ; 
                Category != DataTypeCategories::ARRAY; Category++) {

        PCDataType * newDataType;
        string DataTypeName, InitValue;
        switch(Category) {
            case DataTypeCategories::BOOL :   
                            DataTypeName = "BOOL", InitValue = "0";
                            break;
            case DataTypeCategories::BYTE :     
                                DataTypeName = "BYTE", InitValue = "16#0";
                                break;
            case DataTypeCategories::WORD :     
                                DataTypeName = "WORD", InitValue = "16#0";
                                break;
            case DataTypeCategories::DWORD :     
                                DataTypeName = "DWORD", InitValue = "16#0";
                                break;
            case DataTypeCategories::LWORD :    
                                DataTypeName = "LWORD", InitValue = "16#0";
                                break;
            case DataTypeCategories::CHAR :    
                                DataTypeName = "CHAR", InitValue  = "";
                                break;
            case DataTypeCategories::INT :      
                                DataTypeName = "INT", InitValue = "0";
                                break;
            case DataTypeCategories::SINT :     
                                DataTypeName = "SINT", InitValue = "0";
                                break;
            case DataTypeCategories::DINT :     
                                DataTypeName = "DINT", InitValue = "0";
                                break;
            case DataTypeCategories::LINT :     
                                DataTypeName = "LINT", InitValue = "0";
                                break;
            case DataTypeCategories::UINT :     
                                DataTypeName = "UINT", InitValue = "0";
                                break;
            case DataTypeCategories::USINT :     
                                DataTypeName = "USINT", InitValue = "0";
                                break;
            case DataTypeCategories::UDINT :     
                                DataTypeName = "UDINT", InitValue = "0";
                                break;
            case DataTypeCategories::ULINT :     
                                DataTypeName = "ULINT", InitValue = "0";
                                break;
            case DataTypeCategories::REAL :     
                                DataTypeName = "UREAL", InitValue = "0.0";
                                break;
            case DataTypeCategories::LREAL :     
                                DataTypeName = "LREAL", InitValue = "0.0";
                                break;
            case DataTypeCategories::TIME :     
                                DataTypeName = "TIME", InitValue = "t#0s";
                                break;
            case DataTypeCategories::DATE :     
                                DataTypeName = "DATE", InitValue = "d#0001-01-01";
                                break;
            case DataTypeCategories::TIME_OF_DAY :     
                                DataTypeName = "TOD", InitValue = "tod#00:00:00";
                                break;
            case DataTypeCategories::DATE_AND_TIME :     
                                DataTypeName = "DT";
                                InitValue = "dt#0001-01-01-00:00:00";
                                break;
     
        }

        newDataType = new PCDataType(this, DataTypeName, DataTypeName,
                                    (DataTypeCategories)Category, InitValue);   
        RegisteredDataTypes.RegisterDataType(DataTypeName, newDataType);   
    }

    // note that max string length is 1000 chars
    PCDataType * stringDataType = new PCDataType(this, "STRING", "CHAR",
                                1000, DataTypeCategories::ARRAY);
    RegisteredDataTypes.RegisterDataType("STRING", stringDataType);
}

void PCConfiguration::RegisterAllComplexDataTypes() {
    if (!__specification.has_config_global_pou_var())
        __global_pou_var = nullptr;
    else {

        PCDataType * global_var_type = new PCDataType(this, 
                    "__CONFIG_GLOBAL__", "__CONFIG_GLOBAL__",
                    DataTypeCategories::POU);

        RegisteredDataTypes.RegisterDataType("__CONFIG__GLOBAL__", 
                                        global_var_type);

        for (auto& field : 
                    __specification.config_global_pou_var().datatype_field()) {
            if (field.intf_type() != FieldInterfaceType::VAR_EXPLICIT_STORAGE) {
                if (field.has_dimension_1() && !field.has_dimension_2()) {
                    global_var_type->AddArrayDataTypeField(field.field_name(),
                            field.field_datatype_name(), field.dimension_1(),
                            field.initial_value(),
                            field.intf_type(), field.range_min(),
                            field.range_max());
                } else if (field.has_dimension_1() && field.has_dimension_2()) {

                    global_var_type->AddArrayDataTypeField(field.field_name(),
                            field.field_datatype_name(), field.dimension_1(),
                            field.dimension_2(),
                            field.initial_value(),
                            field.intf_type(), field.range_min(),
                            field.range_max());

                } else {
                    global_var_type->AddDataTypeField(field.field_name(),
                            field.field_datatype_name(), field.initial_value(),
                            field.intf_type(), field.range_min(),
                            field.range_max());
                }
            }
            else if (field.intf_type() 
                        == FieldInterfaceType::VAR_EXPLICIT_STORAGE
                    && field.has_field_storage_spec()) {

                int mem_type = 0;
                int ByteOffset = 0;
                int BitOffset = 0;
                if (field.field_storage_spec().has_full_storage_spec()) {
                    //extract memtype, byte and bit offsets from string specification
                } else {
                    mem_type = (int)field.field_storage_spec().mem_type();
                    ByteOffset = field.field_storage_spec().byte_offset();
                    BitOffset = field.field_storage_spec().bit_offset();
                }

                if (field.has_dimension_1() && !field.has_dimension_2()) {
                    global_var_type->AddArrayDataTypeFieldAT(field.field_name(),
                            field.field_datatype_name(), field.dimension_1(),
                            field.initial_value(),
                            field.range_min(),
                            field.range_max(),
                            mem_type, ByteOffset, BitOffset);
                } else if (field.has_dimension_1() && field.has_dimension_2()) {

                    global_var_type->AddArrayDataTypeFieldAT(field.field_name(),
                            field.field_datatype_name(), field.dimension_1(),
                            field.dimension_2(),
                            field.initial_value(),
                            field.range_min(),
                            field.range_max(),
                            mem_type, ByteOffset, BitOffset);

                } else {
                    global_var_type->AddDataTypeFieldAT(field.field_name(),
                            field.field_datatype_name(), field.initial_value(),
                            field.range_min(),
                            field.range_max(),
                            mem_type, ByteOffset, BitOffset);
                }

            }
        }

        __global_pou_var = new PCVariable(this, nullptr,
                                "__CONFIG_GLOBAL_VAR__", "__CONFIG_GLOBAL__");
        
    }
    
    if (!__specification.has_config_access_pou_var())
        __access_pou_var = nullptr;
    else {

    }
}

PCVariable * PCConfiguration::GetVariablePointerToMem(int MemType,
                int ByteOffset, int BitOffset,string VariableDataTypeName) {

    assert(ByteOffset > 0 && BitOffset >= 0 && BitOffset < 8);
    assert(MemType == MEM_TYPE::RAM_MEM);
    string VariableName = __ConfigurationName + std::to_string(MemType)
                            + "." + std::to_string(ByteOffset)
                            + "." + std::to_string(BitOffset);
    // need to track and delete this variable later on
    PCVariable* V = new PCVariable(this, nullptr, VariableName,
                                VariableDataTypeName);
    assert(V != nullptr);

    
    V->__MemoryLocation.SetMemUnitLocation(&__RAMMemory);
    V->__ByteOffset = ByteOffset;
    V->__BitOffset = BitOffset;
    V->__IsDirectlyRepresented = true;

    return V;
}