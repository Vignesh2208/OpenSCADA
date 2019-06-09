#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_datatype.h"
#include "src/pc_emulator/include/pc_variable.h"
#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/ext_modules/include/comm_module.h"
#include "src/pc_emulator/ext_modules/include/ext_module_intf.h"
#include "src/pc_emulator/ext_modules/include/sensor_module.h"
#include "src/pc_emulator/ext_modules/include/actuator_module.h"
#include "gtest/gtest.h"

using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldInterfaceType = pc_specification::FieldInterfaceType;


TEST(AccessVariableTestSuite, SimpleAccessVariableTest) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/access_variable_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfigurationImpl configuration(TestDir + "/input.prototxt", false);
    PCVariable * access_var = configuration.__access_pou_var.get();

    ASSERT_TRUE(access_var != nullptr);
    
    EXPECT_EQ(access_var->GetPtrStoredAtField("GLOBAL_BOOL_VAR"),
                    configuration.GetVariablePointerToMem(3, 1, "BOOL"));
    
    EXPECT_EQ(access_var->GetPtrStoredAtField("GLOBAL_INT_VAR"),
                    configuration.GetVariablePointerToMem(4, 0, "INT"));
    
    
    ASSERT_TRUE(Utils::TestEQPtrs(
        access_var->GetPtrToField("ACCESS_INT_ARR[1][1]"),
        configuration.GetVariablePointerToMem(100, 0, "INT")));

    ASSERT_TRUE(Utils::TestEQPtrs(
        access_var->GetPtrToField("ACCESS_INT_ARR[1][2]"),
        configuration.GetVariablePointerToMem(102, 0, "INT")));

    ASSERT_TRUE(Utils::TestEQPtrs(
        access_var->GetPtrToField("ACCESS_INT_ARR[2][1]"),
        configuration.GetVariablePointerToMem(104, 0, "INT")));

    ASSERT_TRUE(Utils::TestEQPtrs(
        access_var->GetPtrToField("ACCESS_INT_ARR[2][2]"),
        configuration.GetVariablePointerToMem(106, 0, "INT")));

    ASSERT_TRUE(Utils::TestEQPtrs(
        access_var->GetPtrToField("GLOBAL_INT_ARR[1][2]"),
        configuration.GetVariablePointerToMem(22, 0, "INT")));

    
    ASSERT_TRUE(Utils::TestEQPtrs(
        access_var->GetPtrToField("GLOBAL_INT_ARR[2][1]"),
        configuration.GetVariablePointerToMem(24, 0, "INT")));
    
    ASSERT_TRUE(Utils::TestEQPtrs(
        access_var->GetPtrToField("GLOBAL_INT_ARR[2][2]"),
        configuration.GetVariablePointerToMem(26, 0, "INT")));

    
    ASSERT_TRUE(Utils::TestEQPtrs(
        access_var->GetPtrToField("GLOBAL_BOOL_ARR[1]"),
        configuration.GetVariablePointerToMem(10, 0, "BOOL")));
    
    ASSERT_TRUE(Utils::TestEQPtrs(
        access_var->GetPtrToField("GLOBAL_BOOL_ARR[2]"),
        configuration.GetVariablePointerToMem(10, 1, "BOOL")));

    ASSERT_TRUE(Utils::TestEQPtrs(
        access_var->GetPtrToField("GLOBAL_BOOL_ARR[8]"),
        configuration.GetVariablePointerToMem(10, 7, "BOOL")));

    ASSERT_TRUE(Utils::TestEQPtrs(
        access_var->GetPtrToField("GLOBAL_BOOL_ARR[10]"),
        configuration.GetVariablePointerToMem(11, 1, "BOOL")));

    EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>(
            "GLOBAL_COMPLEX_VAR[1].complex_vector[1].complex_field.oned_arr_field[1]",
            DataTypeCategory::INT), -1);

    EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>(
            "GLOBAL_COMPLEX_VAR[2].complex_vector[1].int_field",
            DataTypeCategory::INT), 10);


    EXPECT_EQ(access_var->GetValueStoredAtField<bool>(
            "SENSOR_IN_2", DataTypeCategory::BOOL), true);

    EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>(
            "SENSOR_IN_1", DataTypeCategory::INT), 10);

    EXPECT_EQ(access_var->GetValueStoredAtField<bool>(
            "MOTOR_OUT_2", DataTypeCategory::BOOL), true);

    EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>(
            "MOTOR_OUT_1", DataTypeCategory::INT), 10);

    EXPECT_EQ(access_var->GetPtrStoredAtField("SENSOR_IN_2"),
                configuration.GetVariablePointerToResourceMem("CPU_001",
                MemType::INPUT_MEM, 12, 2, "BOOL"));
    
    EXPECT_EQ(access_var->GetPtrStoredAtField("MOTOR_OUT_2"),
                configuration.GetVariablePointerToResourceMem("CPU_001",
                MemType::OUTPUT_MEM, 12, 2, "BOOL"));


    EXPECT_EQ(access_var->GetPtrStoredAtField("SENSOR_IN_1"),
                configuration.GetVariablePointerToResourceMem("CPU_001",
                MemType::INPUT_MEM, 10, 0, "INT"));
    
    EXPECT_EQ(access_var->GetPtrStoredAtField("MOTOR_OUT_1"),
                configuration.GetVariablePointerToResourceMem("CPU_001",
                MemType::OUTPUT_MEM, 10, 0, "INT"));


    EXPECT_EQ(access_var->GetPtrStoredAtField("SENSOR_IN_3"),
                configuration.GetVariablePointerToResourceMem("CPU_001",
                MemType::INPUT_MEM, 20, 0, "COMPLEX_STRUCT_3"));
    
    EXPECT_EQ(access_var->GetPtrStoredAtField("MOTOR_OUT_3"),
                configuration.GetVariablePointerToResourceMem("CPU_001",
                MemType::OUTPUT_MEM, 2000, 0, "COMPLEX_STRUCT_3"));

     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("MOTOR_OUT_3.complex_vector[1]."
        "complex_field.oned_arr_field[1]", DataTypeCategory::INT), -1);

     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("MOTOR_OUT_3.complex_vector[1]."
        "complex_field.oned_arr_field[2]",DataTypeCategory::INT), 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("MOTOR_OUT_3.complex_vector[1]."
        "complex_field.oned_arr_field[3]", DataTypeCategory::INT), 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("MOTOR_OUT_3.complex_vector[1]."
        "complex_field.oned_arr_field[4]", DataTypeCategory::INT), 2);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("MOTOR_OUT_3.complex_vector[1]."
        "complex_field.oned_arr_field[5]", DataTypeCategory::INT), 3);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("MOTOR_OUT_3.complex_vector[1]."
        "complex_field.oned_arr_field[6]", DataTypeCategory::INT), 4);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("MOTOR_OUT_3.complex_vector[1]."
        "complex_field.oned_arr_field[7]", DataTypeCategory::INT), 5);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("MOTOR_OUT_3.complex_vector[1]."
        "complex_field.oned_arr_field[8]", DataTypeCategory::INT), 6);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("MOTOR_OUT_3.complex_vector[1]."
        "complex_field.oned_arr_field[9]", DataTypeCategory::INT), 7);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("MOTOR_OUT_3.complex_vector[1]."
        "complex_field.oned_arr_field[10]", DataTypeCategory::INT), 8);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("MOTOR_OUT_3.complex_vector[1]."
        "complex_field.twod_arr_field[1][1]", DataTypeCategory::INT), 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("MOTOR_OUT_3.complex_vector[1]."
        "complex_field.twod_arr_field[1][2]", DataTypeCategory::INT), 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("MOTOR_OUT_3.complex_vector[1]."
        "complex_field.twod_arr_field[2][1]", DataTypeCategory::INT), 2);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("MOTOR_OUT_3.complex_vector[1]."
        "complex_field.twod_arr_field[2][2]", DataTypeCategory::INT), 3);
     EXPECT_EQ(access_var->GetValueStoredAtField<TimeType>("MOTOR_OUT_3.complex_vector[1]."
        "time_field", DataTypeCategory::TIME).SecsElapsed, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateType>("MOTOR_OUT_3.complex_vector[1]."
        "date_field", DataTypeCategory::DATE).Year, 2020);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateType>("MOTOR_OUT_3.complex_vector[1]."
        "date_field", DataTypeCategory::DATE).Month, 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateType>("MOTOR_OUT_3.complex_vector[1]."
        "date_field", DataTypeCategory::DATE).Day, 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<TODDataType>("MOTOR_OUT_3.complex_vector[1]."
        "tod_field", DataTypeCategory::TIME_OF_DAY).Hr, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<TODDataType>("MOTOR_OUT_3.complex_vector[1]."
        "tod_field", DataTypeCategory::TIME_OF_DAY).Min, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<TODDataType>("MOTOR_OUT_3.complex_vector[1]."
        "tod_field", DataTypeCategory::TIME_OF_DAY).Sec, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("MOTOR_OUT_3.complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Year, 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("MOTOR_OUT_3.complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Month, 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("MOTOR_OUT_3.complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Day, 1);
    EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("MOTOR_OUT_3.complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Hr, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("MOTOR_OUT_3.complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Min, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("MOTOR_OUT_3.complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Sec, 0);

     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("MOTOR_OUT_3.complex_vector[1].int_field",
            DataTypeCategory::INT), 10);
    

           
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("SENSOR_IN_3.complex_vector[1]."
        "complex_field.oned_arr_field[1]", DataTypeCategory::INT), -1);

     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("SENSOR_IN_3.complex_vector[1]."
        "complex_field.oned_arr_field[2]",DataTypeCategory::INT), 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("SENSOR_IN_3.complex_vector[1]."
        "complex_field.oned_arr_field[3]", DataTypeCategory::INT), 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("SENSOR_IN_3.complex_vector[1]."
        "complex_field.oned_arr_field[4]", DataTypeCategory::INT), 2);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("SENSOR_IN_3.complex_vector[1]."
        "complex_field.oned_arr_field[5]", DataTypeCategory::INT), 3);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("SENSOR_IN_3.complex_vector[1]."
        "complex_field.oned_arr_field[6]", DataTypeCategory::INT), 4);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("SENSOR_IN_3.complex_vector[1]."
        "complex_field.oned_arr_field[7]", DataTypeCategory::INT), 5);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("SENSOR_IN_3.complex_vector[1]."
        "complex_field.oned_arr_field[8]", DataTypeCategory::INT), 6);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("SENSOR_IN_3.complex_vector[1]."
        "complex_field.oned_arr_field[9]", DataTypeCategory::INT), 7);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("SENSOR_IN_3.complex_vector[1]."
        "complex_field.oned_arr_field[10]", DataTypeCategory::INT), 8);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("SENSOR_IN_3.complex_vector[1]."
        "complex_field.twod_arr_field[1][1]", DataTypeCategory::INT), 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("SENSOR_IN_3.complex_vector[1]."
        "complex_field.twod_arr_field[1][2]", DataTypeCategory::INT), 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("SENSOR_IN_3.complex_vector[1]."
        "complex_field.twod_arr_field[2][1]", DataTypeCategory::INT), 2);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("SENSOR_IN_3.complex_vector[1]."
        "complex_field.twod_arr_field[2][2]", DataTypeCategory::INT), 3);
     EXPECT_EQ(access_var->GetValueStoredAtField<TimeType>("SENSOR_IN_3.complex_vector[1]."
        "time_field", DataTypeCategory::TIME).SecsElapsed, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateType>("SENSOR_IN_3.complex_vector[1]."
        "date_field", DataTypeCategory::DATE).Year, 2020);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateType>("SENSOR_IN_3.complex_vector[1]."
        "date_field", DataTypeCategory::DATE).Month, 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateType>("SENSOR_IN_3.complex_vector[1]."
        "date_field", DataTypeCategory::DATE).Day, 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<TODDataType>("SENSOR_IN_3.complex_vector[1]."
        "tod_field", DataTypeCategory::TIME_OF_DAY).Hr, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<TODDataType>("SENSOR_IN_3.complex_vector[1]."
        "tod_field", DataTypeCategory::TIME_OF_DAY).Min, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<TODDataType>("SENSOR_IN_3.complex_vector[1]."
        "tod_field", DataTypeCategory::TIME_OF_DAY).Sec, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("SENSOR_IN_3.complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Year, 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("SENSOR_IN_3.complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Month, 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("SENSOR_IN_3.complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Day, 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("SENSOR_IN_3.complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Hr, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("SENSOR_IN_3.complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Min, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("SENSOR_IN_3.complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Sec, 0);

     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("SENSOR_IN_3.complex_vector[1].int_field",
            DataTypeCategory::INT), 10);
    

    configuration.Cleanup();
}


TEST(AccessVariableTestSuite, ExtConfigInterfaceTest) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/access_variable_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfigurationImpl configuration(TestDir + "/input.prototxt", false);
    PCConfigurationInterface config_ext_interface(TestDir + "/input.prototxt");

        
    PCVariable * access_var = configuration.__access_pou_var.get();

    ASSERT_TRUE(access_var != nullptr);
    

    auto Var1 = config_ext_interface.GetExternVariable(
            "GLOBAL_INT_VAR");

    EXPECT_EQ(Var1->GetValueStoredAtField<int16_t>("",
                DataTypeCategory::INT), 10);

    EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("GLOBAL_INT_VAR",
            DataTypeCategory::INT), 10);

     auto Var2 = config_ext_interface.GetExternVariable(
            "GLOBAL_COMPLEX_VAR");
     assert(Var2 != nullptr);

     EXPECT_EQ(Var2->GetValueStoredAtField<int16_t>("[1].complex_vector[1]."
        "complex_field.oned_arr_field[1]", DataTypeCategory::INT), -1);

     EXPECT_EQ(Var2->GetValueStoredAtField<int16_t>("[1].complex_vector[1]."
        "complex_field.oned_arr_field[2]",DataTypeCategory::INT), 0);
     EXPECT_EQ(Var2->GetValueStoredAtField<int16_t>("[1].complex_vector[1]."
        "complex_field.oned_arr_field[3]", DataTypeCategory::INT), 1);
     EXPECT_EQ(Var2->GetValueStoredAtField<int16_t>("[1].complex_vector[1]."
        "complex_field.oned_arr_field[4]", DataTypeCategory::INT), 2);
     EXPECT_EQ(Var2->GetValueStoredAtField<int16_t>("[1].complex_vector[1]."
        "complex_field.oned_arr_field[5]", DataTypeCategory::INT), 3);
     EXPECT_EQ(Var2->GetValueStoredAtField<int16_t>("[1].complex_vector[1]."
        "complex_field.oned_arr_field[6]", DataTypeCategory::INT), 4);
     EXPECT_EQ(Var2->GetValueStoredAtField<int16_t>("[1].complex_vector[1]."
        "complex_field.oned_arr_field[7]", DataTypeCategory::INT), 5);
     EXPECT_EQ(Var2->GetValueStoredAtField<int16_t>("[1].complex_vector[1]."
        "complex_field.oned_arr_field[8]", DataTypeCategory::INT), 6);
     EXPECT_EQ(Var2->GetValueStoredAtField<int16_t>("[1].complex_vector[1]."
        "complex_field.oned_arr_field[9]", DataTypeCategory::INT), 7);
     EXPECT_EQ(Var2->GetValueStoredAtField<int16_t>("[1].complex_vector[1]."
        "complex_field.oned_arr_field[10]", DataTypeCategory::INT), 8);
     EXPECT_EQ(Var2->GetValueStoredAtField<int16_t>("[1].complex_vector[1]."
        "complex_field.twod_arr_field[1][1]", DataTypeCategory::INT), 0);
     EXPECT_EQ(Var2->GetValueStoredAtField<int16_t>("[1].complex_vector[1]."
        "complex_field.twod_arr_field[1][2]", DataTypeCategory::INT), 1);
     EXPECT_EQ(Var2->GetValueStoredAtField<int16_t>("[1].complex_vector[1]."
        "complex_field.twod_arr_field[2][1]", DataTypeCategory::INT), 2);
     EXPECT_EQ(Var2->GetValueStoredAtField<int16_t>("[1].complex_vector[1]."
        "complex_field.twod_arr_field[2][2]", DataTypeCategory::INT), 3);
     EXPECT_EQ(Var2->GetValueStoredAtField<TimeType>("[1].complex_vector[1]."
        "time_field", DataTypeCategory::TIME).SecsElapsed, 0);
     EXPECT_EQ(Var2->GetValueStoredAtField<DateType>("[1].complex_vector[1]."
        "date_field", DataTypeCategory::DATE).Year, 2020);
     EXPECT_EQ(Var2->GetValueStoredAtField<DateType>("[1].complex_vector[1]."
        "date_field", DataTypeCategory::DATE).Month, 1);
     EXPECT_EQ(Var2->GetValueStoredAtField<DateType>("[1].complex_vector[1]."
        "date_field", DataTypeCategory::DATE).Day, 1);
     EXPECT_EQ(Var2->GetValueStoredAtField<TODDataType>("[1].complex_vector[1]."
        "tod_field", DataTypeCategory::TIME_OF_DAY).Hr, 0);
     EXPECT_EQ(Var2->GetValueStoredAtField<TODDataType>("[1].complex_vector[1]."
        "tod_field", DataTypeCategory::TIME_OF_DAY).Min, 0);
     EXPECT_EQ(Var2->GetValueStoredAtField<TODDataType>("[1].complex_vector[1]."
        "tod_field", DataTypeCategory::TIME_OF_DAY).Sec, 0);
     EXPECT_EQ(Var2->GetValueStoredAtField<DateTODDataType>("[1].complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Year, 1);
     EXPECT_EQ(Var2->GetValueStoredAtField<DateTODDataType>("[1].complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Month, 1);
     EXPECT_EQ(Var2->GetValueStoredAtField<DateTODDataType>("[1].complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Day, 1);
    EXPECT_EQ(Var2->GetValueStoredAtField<DateTODDataType>("[1].complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Hr, 0);
     EXPECT_EQ(Var2->GetValueStoredAtField<DateTODDataType>("[1].complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Min, 0);
     EXPECT_EQ(Var2->GetValueStoredAtField<DateTODDataType>("[1].complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Sec, 0);

     EXPECT_EQ(Var2->GetValueStoredAtField<int16_t>("[2].complex_vector[1].int_field",
            DataTypeCategory::INT), 10);
    

     access_var = config_ext_interface.GetExternVariable("MOTOR_OUT_3");
     assert(access_var != nullptr);

     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.oned_arr_field[1]", DataTypeCategory::INT), -1);

     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.oned_arr_field[2]",DataTypeCategory::INT), 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.oned_arr_field[3]", DataTypeCategory::INT), 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.oned_arr_field[4]", DataTypeCategory::INT), 2);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.oned_arr_field[5]", DataTypeCategory::INT), 3);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.oned_arr_field[6]", DataTypeCategory::INT), 4);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.oned_arr_field[7]", DataTypeCategory::INT), 5);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.oned_arr_field[8]", DataTypeCategory::INT), 6);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.oned_arr_field[9]", DataTypeCategory::INT), 7);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.oned_arr_field[10]", DataTypeCategory::INT), 8);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.twod_arr_field[1][1]", DataTypeCategory::INT), 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.twod_arr_field[1][2]", DataTypeCategory::INT), 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.twod_arr_field[2][1]", DataTypeCategory::INT), 2);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.twod_arr_field[2][2]", DataTypeCategory::INT), 3);
     EXPECT_EQ(access_var->GetValueStoredAtField<TimeType>("complex_vector[1]."
        "time_field", DataTypeCategory::TIME).SecsElapsed, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateType>("complex_vector[1]."
        "date_field", DataTypeCategory::DATE).Year, 2020);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateType>("complex_vector[1]."
        "date_field", DataTypeCategory::DATE).Month, 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateType>("complex_vector[1]."
        "date_field", DataTypeCategory::DATE).Day, 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<TODDataType>("complex_vector[1]."
        "tod_field", DataTypeCategory::TIME_OF_DAY).Hr, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<TODDataType>("complex_vector[1]."
        "tod_field", DataTypeCategory::TIME_OF_DAY).Min, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<TODDataType>("complex_vector[1]."
        "tod_field", DataTypeCategory::TIME_OF_DAY).Sec, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Year, 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Month, 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Day, 1);
    EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Hr, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Min, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Sec, 0);

     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1].int_field",
            DataTypeCategory::INT), 10);
    
     access_var = config_ext_interface.GetExternVariable("SENSOR_IN_3");
     assert(access_var != nullptr);
           
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.oned_arr_field[1]", DataTypeCategory::INT), -1);

     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.oned_arr_field[2]",DataTypeCategory::INT), 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.oned_arr_field[3]", DataTypeCategory::INT), 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.oned_arr_field[4]", DataTypeCategory::INT), 2);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.oned_arr_field[5]", DataTypeCategory::INT), 3);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.oned_arr_field[6]", DataTypeCategory::INT), 4);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.oned_arr_field[7]", DataTypeCategory::INT), 5);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.oned_arr_field[8]", DataTypeCategory::INT), 6);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.oned_arr_field[9]", DataTypeCategory::INT), 7);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.oned_arr_field[10]", DataTypeCategory::INT), 8);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.twod_arr_field[1][1]", DataTypeCategory::INT), 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.twod_arr_field[1][2]", DataTypeCategory::INT), 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.twod_arr_field[2][1]", DataTypeCategory::INT), 2);
     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1]."
        "complex_field.twod_arr_field[2][2]", DataTypeCategory::INT), 3);
     EXPECT_EQ(access_var->GetValueStoredAtField<TimeType>("complex_vector[1]."
        "time_field", DataTypeCategory::TIME).SecsElapsed, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateType>("complex_vector[1]."
        "date_field", DataTypeCategory::DATE).Year, 2020);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateType>("complex_vector[1]."
        "date_field", DataTypeCategory::DATE).Month, 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateType>("complex_vector[1]."
        "date_field", DataTypeCategory::DATE).Day, 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<TODDataType>("complex_vector[1]."
        "tod_field", DataTypeCategory::TIME_OF_DAY).Hr, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<TODDataType>("complex_vector[1]."
        "tod_field", DataTypeCategory::TIME_OF_DAY).Min, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<TODDataType>("complex_vector[1]."
        "tod_field", DataTypeCategory::TIME_OF_DAY).Sec, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Year, 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Month, 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Day, 1);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Hr, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Min, 0);
     EXPECT_EQ(access_var->GetValueStoredAtField<DateTODDataType>("complex_vector[1]."
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Sec, 0);

     EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>("complex_vector[1].int_field",
            DataTypeCategory::INT), 10);

     access_var = config_ext_interface.GetExternVariable("SENSOR_IN_2");
     assert(access_var != nullptr);

    EXPECT_EQ(access_var->GetValueStoredAtField<bool>(
            "", DataTypeCategory::BOOL), true);


     access_var = config_ext_interface.GetExternVariable("SENSOR_IN_1");
     assert(access_var != nullptr);

    EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>(
            "", DataTypeCategory::INT), 10);

     access_var = config_ext_interface.GetExternVariable("MOTOR_OUT_2");
     assert(access_var != nullptr);

    EXPECT_EQ(access_var->GetValueStoredAtField<bool>(
            "", DataTypeCategory::BOOL), true);


     access_var = config_ext_interface.GetExternVariable("MOTOR_OUT_1");
     assert(access_var != nullptr);

    EXPECT_EQ(access_var->GetValueStoredAtField<int16_t>(
            "", DataTypeCategory::INT), 10);
    

    configuration.Cleanup();
    config_ext_interface.Cleanup();

}


TEST(AccessVariableTestSuite, CommModuleInterfaceTest) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/access_variable_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfigurationImpl configuration(TestDir + "/input.prototxt", false);
    CommModule communication_module(TestDir + "/input.prototxt");

    auto VarContainer = communication_module.GetVariableContainer("GLOBAL_COMPLEX_VAR[1]."
        "complex_vector[1]");


     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[1]", DataTypeCategory::INT), -1);

     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[2]",DataTypeCategory::INT), 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[3]", DataTypeCategory::INT), 1);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[4]", DataTypeCategory::INT), 2);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[5]", DataTypeCategory::INT), 3);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[6]", DataTypeCategory::INT), 4);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[7]", DataTypeCategory::INT), 5);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[8]", DataTypeCategory::INT), 6);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[9]", DataTypeCategory::INT), 7);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[10]", DataTypeCategory::INT), 8);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.twod_arr_field[1][1]", DataTypeCategory::INT), 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.twod_arr_field[1][2]", DataTypeCategory::INT), 1);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.twod_arr_field[2][1]", DataTypeCategory::INT), 2);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.twod_arr_field[2][2]", DataTypeCategory::INT), 3);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<TimeType>(
        "time_field", DataTypeCategory::TIME).SecsElapsed, 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateType>(
        "date_field", DataTypeCategory::DATE).Year, 2020);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateType>(
        "date_field", DataTypeCategory::DATE).Month, 1);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateType>(
        "date_field", DataTypeCategory::DATE).Day, 1);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<TODDataType>(
        "tod_field", DataTypeCategory::TIME_OF_DAY).Hr, 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<TODDataType>(
        "tod_field", DataTypeCategory::TIME_OF_DAY).Min, 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<TODDataType>(
        "tod_field", DataTypeCategory::TIME_OF_DAY).Sec, 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateTODDataType>(
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Year, 1);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateTODDataType>(
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Month, 1);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateTODDataType>(
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Day, 1);
    EXPECT_EQ(VarContainer->GetValueStoredAtField<DateTODDataType>(
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Hr, 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateTODDataType>(
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Min, 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateTODDataType>(
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Sec, 0);

     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>("int_field",
            DataTypeCategory::INT), 10);

    VarContainer = communication_module.GetVariableContainer("MOTOR_OUT_3."
        "complex_vector[1]");


     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[1]", DataTypeCategory::INT), -1);

     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[2]",DataTypeCategory::INT), 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[3]", DataTypeCategory::INT), 1);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[4]", DataTypeCategory::INT), 2);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[5]", DataTypeCategory::INT), 3);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[6]", DataTypeCategory::INT), 4);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[7]", DataTypeCategory::INT), 5);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[8]", DataTypeCategory::INT), 6);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[9]", DataTypeCategory::INT), 7);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[10]", DataTypeCategory::INT), 8);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.twod_arr_field[1][1]", DataTypeCategory::INT), 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.twod_arr_field[1][2]", DataTypeCategory::INT), 1);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.twod_arr_field[2][1]", DataTypeCategory::INT), 2);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.twod_arr_field[2][2]", DataTypeCategory::INT), 3);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<TimeType>(
        "time_field", DataTypeCategory::TIME).SecsElapsed, 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateType>(
        "date_field", DataTypeCategory::DATE).Year, 2020);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateType>(
        "date_field", DataTypeCategory::DATE).Month, 1);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateType>(
        "date_field", DataTypeCategory::DATE).Day, 1);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<TODDataType>(
        "tod_field", DataTypeCategory::TIME_OF_DAY).Hr, 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<TODDataType>(
        "tod_field", DataTypeCategory::TIME_OF_DAY).Min, 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<TODDataType>(
        "tod_field", DataTypeCategory::TIME_OF_DAY).Sec, 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateTODDataType>(
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Year, 1);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateTODDataType>(
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Month, 1);
        
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateTODDataType>(
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Day, 1);
    EXPECT_EQ(VarContainer->GetValueStoredAtField<DateTODDataType>(
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Hr, 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateTODDataType>(
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Min, 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateTODDataType>(
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Sec, 0);

     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>("int_field",
            DataTypeCategory::INT), 10);

     VarContainer = communication_module.GetVariableContainer("SENSOR_IN_3."
        "complex_vector[1]");


     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[1]", DataTypeCategory::INT), -1);

     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[2]",DataTypeCategory::INT), 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[3]", DataTypeCategory::INT), 1);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[4]", DataTypeCategory::INT), 2);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[5]", DataTypeCategory::INT), 3);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[6]", DataTypeCategory::INT), 4);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[7]", DataTypeCategory::INT), 5);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[8]", DataTypeCategory::INT), 6);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[9]", DataTypeCategory::INT), 7);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.oned_arr_field[10]", DataTypeCategory::INT), 8);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.twod_arr_field[1][1]", DataTypeCategory::INT), 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.twod_arr_field[1][2]", DataTypeCategory::INT), 1);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.twod_arr_field[2][1]", DataTypeCategory::INT), 2);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>(
        "complex_field.twod_arr_field[2][2]", DataTypeCategory::INT), 3);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<TimeType>(
        "time_field", DataTypeCategory::TIME).SecsElapsed, 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateType>(
        "date_field", DataTypeCategory::DATE).Year, 2020);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateType>(
        "date_field", DataTypeCategory::DATE).Month, 1);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateType>(
        "date_field", DataTypeCategory::DATE).Day, 1);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<TODDataType>(
        "tod_field", DataTypeCategory::TIME_OF_DAY).Hr, 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<TODDataType>(
        "tod_field", DataTypeCategory::TIME_OF_DAY).Min, 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<TODDataType>(
        "tod_field", DataTypeCategory::TIME_OF_DAY).Sec, 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateTODDataType>(
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Year, 1);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateTODDataType>(
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Month, 1);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateTODDataType>(
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Date.Day, 1);
    EXPECT_EQ(VarContainer->GetValueStoredAtField<DateTODDataType>(
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Hr, 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateTODDataType>(
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Min, 0);
     EXPECT_EQ(VarContainer->GetValueStoredAtField<DateTODDataType>(
        "date_tod_field", DataTypeCategory::DATE_AND_TIME).Tod.Sec, 0);

     EXPECT_EQ(VarContainer->GetValueStoredAtField<int16_t>("int_field",
            DataTypeCategory::INT), 10);
    
    configuration.Cleanup();
    communication_module.Cleanup();

}