#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/include/pc_datatype.h"
#include "src/pc_emulator/include/pc_variable.h"
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/ext_modules/include/comm_module.h"
#include "src/pc_emulator/ext_modules/include/ext_module_intf.h"
#include <stdio.h>
#include <assert.h>

using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldInterfaceType = pc_specification::FieldInterfaceType;

int main() {
    string SpecDir = Utils::GetInstallationDirectory() 
            + "/examples/idle_plc";

    std::cout << "Reading System specification from: " 
        << SpecDir + "/system_specification.prototxt" << std::endl;
    
    CommModule communication_module(SpecDir + "/system_specification.prototxt");

    // Gets a variable container which points to GLOBAL_COMPLEX_VAR[1].complex_vector[1]
    // This is an example of reading/writing to an access path.
    // Access paths are defined under VAR_ACCESS in the system_specification.protottxt
    // Additional qualifiers (READ_WRITE/READ_ONLY) may be specfied in the access path definition
    // A READ_ONLY access path returns a variable container which cannot be used to set any values
    // In this example, the access path corresponding to GLOBAL_COMPLEX_VAR 
    // is defined in line 242 of system_specification.prototxt
    auto VarToAccessPath = communication_module.GetVariableContainer(
        "GLOBAL_COMPLEX_VAR[1].complex_vector[1]");

    assert(VarToAccessPath != nullptr);

    // Gets a variable container to byte number 5 in the RAM. It also initializes
    // an integer variable pointer to this location. The valid data types may include
    // (1) any of the elementary data types 
    //     (listed in src/pc_emulator/proto/configuration.proto:DataTypeCategory)
    // (2) any of the user defined data types declared in system_specification.prototxt
    auto IntVarToRAMMemoryLocation 
        = communication_module.GetVariableContainer(5, 0, "INT");

    assert(IntVarToRAMMemoryLocation != nullptr);

    // Gets a variable container to byte number 30 in the RAM. It also initializes
    // a COMPLEX_STRUCT_3 variable pointer to this location.
    auto ComplexVarToRAMMemoryLocation 
        = communication_module.GetVariableContainer(30, 0, "COMPLEX_STRUCT_3");

    assert(ComplexVarToRAMMemoryLocation != nullptr);

    // -----------------------------READ Accesses --------------------------
    // Note:  ONLY fields/subfields of elementary data types can be read
    // The GetValueStoredAtField template function can be used to read nested fields like shown below
    // for different elementary data types. Note that if the data type of the nested field does not match
    // the passed arguments in any of the following cases, the function call would fail and throw an exception

    // (1) Reading BOOL value stored at nested field:
    //      bool value_read = Var->GetValueStoredAtField<bool>(FieldName, DataTypeCategory::BOOL);
    
    // (2) Reading BYTE value stored at nested field:
    //      uint8_t value_read = Var->GetValueStoredAtField<bool>(FieldName, DataTypeCategory::BYTE);
    
    // (3) Reading WORD value stored at nested field:
    //      uint16_t value_read = Var->GetValueStoredAtField<uint16_t>(FieldName, DataTypeCategory::WORD);
    
    // (4) Reading DWORD value stored at nested field:
    //      uint32_t value_read = Var->GetValueStoredAtField<uint32_t>(FieldName, DataTypeCategory::DWORD);
    
    // (5) Reading LWORD value stored at nested field:
    //      uint64_t value_read = Var->GetValueStoredAtField<uint64_t>(FieldName, DataTypeCategory::LWORD);
    
    // (6) Reading USINT value stored at nested field:
    //      uint8_t value_read = Var->GetValueStoredAtField<uint8_t>(FieldName, DataTypeCategory::USINT);
    
    // (7) Reading SINT value stored at nested field:
    //      int8_t value_read = Var->GetValueStoredAtField<int8_t>(FieldName, DataTypeCategory::SINT);
    
    // (8) Reading UINT value stored at nested field:
    //      uint16_t value_read = Var->GetValueStoredAtField<uint16_t>(FieldName, DataTypeCategory::UINT);
    
    // (9) Reading INT value stored at nested field:
    //      int16_t value_read = Var->GetValueStoredAtField<int16_t>(FieldName, DataTypeCategory::INT);
    
    // (10) Reading UDINT value stored at nested field:
    //      uint32_t value_read = Var->GetValueStoredAtField<uint32_t>(FieldName, DataTypeCategory::UDINT);
    
    // (11) Reading DINT value stored at nested field:
    //      int32_t value_read = Var->GetValueStoredAtField<int32_t>(FieldName, DataTypeCategory::DINT);
    
    // (12) Reading ULINT value stored at nested field:
    //      uint64_t value_read = Var->GetValueStoredAtField<uint64_t>(FieldName, DataTypeCategory::ULINT);
    
    // (13) Reading LINT value stored at nested field:
    //      int64_t value_read = Var->GetValueStoredAtField<int64_t>(FieldName, DataTypeCategory::LINT);
    
    // (14) Reading REAL value stored at nested field:
    //      float value_read = Var->GetValueStoredAtField<float>(FieldName, DataTypeCategory::REAL);
    
    // (15) Reading LREAL value stored at nested field:
    //      double value_read = Var->GetValueStoredAtField<double>(FieldName, DataTypeCategory::LREAL);
    
    // (16) Reading DATE value stored at nested field:
    //      DateType value_read = Var->GetValueStoredAtField<DateType>(FieldName, DataTypeCategory::DATE);
    
    // (17) Reading DATE_AND_TIME value stored at nested field:
    //      DateTODType value_read = Var->GetValueStoredAtField<DateTODType>(FieldName, DataTypeCategory::DATE_AND_TIME);
    
    // (18) Reading TIME_OF_DAY value stored at nested field:
    //      TODType value_read = Var->GetValueStoredAtField<TODType>(FieldName, DataTypeCategory::TIME_OF_DAY);
    
    // (19) Reading TIME value stored at nested field:
    //      TimeType value_read = Var->GetValueStoredAtField<TimeType>(FieldName, DataTypeCategory::CHAR);
    
    std::cout << "###################################################\n\n";
    std::cout << "Reading initial values ...\n\n";
    // Example read access from Access path
    // Reads Integer stored at GLOBAL_COMPLEX_VAR[1].complex_vector[1].complex_field.oned_arr_field[1]
    int16_t read_value1 = VarToAccessPath->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[1]", DataTypeCategory::INT);


    std::cout << 
    "GLOBAL_COMPLEX_VAR[1].complex_vector[1].complex_field.oned_arr_field[1] = "
    << read_value1 <<std::endl;

    // Example read access from RAM Memory
    // Reads Integer stored at memory address bytes 5 & 6 (INT is 2 bytes)
    int16_t read_value2 
    = IntVarToRAMMemoryLocation->GetValueStoredAtField<int16_t>("",
                    DataTypeCategory::INT);

    std::cout << "Integer value stored at MW5 = " << read_value2 <<std::endl;

    
    // Example read access from different part of RAM Memory
    // Reads date stored at COMPLEX_STRUCT_3 variable defined at memory address 30
    DateType read_value3 
    = ComplexVarToRAMMemoryLocation->GetValueStoredAtField<DateType>(
        "complex_vector[1].date_field", DataTypeCategory::DATE);

    std::cout << "complex_vector[1].date_field = " 
        << DataTypeUtils::DateToDTString(read_value3) << std::endl;

    // --------------------------- WRITE ACCESS -----------------------------
    // Writing to nested fields can be done using the SetField API
    // For access paths, write only succeeds if READ_WRITE  is specified as a qualifier

    std::cout << "\n\nUpdating Some values ...\n\n";
    // Sets GLOBAL_COMPLEX_VAR[1].complex_vector[1].complex_field.oned_arr_field[1] to 10
    VarToAccessPath->SetField("complex_field.oned_arr_field[1]", "10");

    // Writing to RAM memory location bytes 5 and 6 with integer value 120
    IntVarToRAMMemoryLocation->SetField("", "120");

    // Writing a date value.
    ComplexVarToRAMMemoryLocation->SetField("complex_vector[1].date_field",
            "d#2020-02-02");

    // Read and print updated values

    std::cout << "\n\nReading updated values ...\n\n";
    read_value1 = VarToAccessPath->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[1]", DataTypeCategory::INT);
    std::cout << 
    "GLOBAL_COMPLEX_VAR[1].complex_vector[1].complex_field.oned_arr_field[1] = "
    << read_value1 <<std::endl;
    
    read_value2 
    = IntVarToRAMMemoryLocation->GetValueStoredAtField<int16_t>("",
                    DataTypeCategory::INT);

    std::cout << "Integer value stored at MW5 = " << read_value2 <<std::endl;

    read_value3 
    = ComplexVarToRAMMemoryLocation->GetValueStoredAtField<DateType>(
        "complex_vector[1].date_field", DataTypeCategory::DATE);

    std::cout << "complex_vector[1].date_field = " 
        << DataTypeUtils::DateToDTString(read_value3) << std::endl;

    
    std::cout << "Finishing ...\n\n";
    communication_module.Cleanup();
    return 0;
}

