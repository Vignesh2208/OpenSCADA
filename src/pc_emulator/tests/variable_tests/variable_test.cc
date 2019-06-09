#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_datatype.h"
#include "src/pc_emulator/include/pc_variable.h"
#include "src/pc_emulator/include/utils.h"
#include "gtest/gtest.h"

using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldInterfaceType = pc_specification::FieldInterfaceType;


TEST(VariableTestSuite, ConfigGlobalVariableTest) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/variable_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfigurationImpl configuration(TestDir + "/input.prototxt", false);
    PCVariable * global_var = configuration.__global_pou_var.get();

    

    ASSERT_TRUE(global_var != nullptr);
    
    EXPECT_EQ(global_var->GetPtrStoredAtField("global_bool_var"),
                    configuration.GetVariablePointerToMem(3, 1, "BOOL"));
    
    EXPECT_EQ(global_var->GetPtrStoredAtField("global_int_var"),
                    configuration.GetVariablePointerToMem(4, 0, "INT"));
    
    
    ASSERT_TRUE(Utils::TestEQPtrs(
        global_var->GetPtrToField("global_int_arr[1][1]"),
        configuration.GetVariablePointerToMem(20, 0, "INT")));

    
    ASSERT_TRUE(Utils::TestEQPtrs(
        global_var->GetPtrToField("global_int_arr[1][2]"),
        configuration.GetVariablePointerToMem(22, 0, "INT")));

    
    ASSERT_TRUE(Utils::TestEQPtrs(
        global_var->GetPtrToField("global_int_arr[2][1]"),
        configuration.GetVariablePointerToMem(24, 0, "INT")));
    
    ASSERT_TRUE(Utils::TestEQPtrs(
        global_var->GetPtrToField("global_int_arr[2][2]"),
        configuration.GetVariablePointerToMem(26, 0, "INT")));

    
    ASSERT_TRUE(Utils::TestEQPtrs(
        global_var->GetPtrToField("global_bool_arr[1]"),
        configuration.GetVariablePointerToMem(10, 0, "BOOL")));
    
    ASSERT_TRUE(Utils::TestEQPtrs(
        global_var->GetPtrToField("global_bool_arr[2]"),
        configuration.GetVariablePointerToMem(10, 1, "BOOL")));

    ASSERT_TRUE(Utils::TestEQPtrs(
        global_var->GetPtrToField("global_bool_arr[8]"),
        configuration.GetVariablePointerToMem(10, 7, "BOOL")));

    ASSERT_TRUE(Utils::TestEQPtrs(
        global_var->GetPtrToField("global_bool_arr[10]"),
        configuration.GetVariablePointerToMem(11, 1, "BOOL")));
    
    configuration.Cleanup();
}


TEST(VariableTestSuite, ResourcePoUVariableTest) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/variable_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfigurationImpl configuration(TestDir + "/input.prototxt", false);
    PCVariable * pou_var = configuration.GetPOU("CPU_001.PROGRAM_1");
    PCVariable * global_var = configuration.__global_pou_var.get();

   
    
    ASSERT_TRUE(pou_var != nullptr);
    
    EXPECT_EQ(pou_var->GetValueStoredAtField<int16_t>("input_1", 
                                        DataTypeCategory::INT), 0);
    
    EXPECT_EQ(pou_var->GetValueStoredAtField<int16_t>("input_2", 
                                        DataTypeCategory::INT), 0);
    EXPECT_EQ(pou_var->GetValueStoredAtField<float>("output_1", 
                                        DataTypeCategory::REAL), 0.0);
    EXPECT_EQ(pou_var->GetValueStoredAtField<float>("output_2", 
                                        DataTypeCategory::REAL), 0.0);
    EXPECT_EQ(pou_var->GetValueStoredAtField<int16_t>("var_1", 
                                        DataTypeCategory::INT), 0);

    EXPECT_EQ(pou_var->GetPtrStoredAtField("inout_1"), nullptr);
    
    EXPECT_EQ(pou_var->GetPtrStoredAtField("drv_1"), 
            configuration.GetVariablePointerToMem(10, 0, "INT"));
    EXPECT_EQ(pou_var->GetPtrStoredAtField("start"), 
            configuration.GetExternVariable("start"));
    EXPECT_EQ(pou_var->GetPtrStoredAtField("start_time"), 
            configuration.GetExternVariable("start_time"));

    EXPECT_EQ(pou_var->GetPtrStoredAtField("global_int_arr"), 
            configuration.GetExternVariable("global_int_arr"));

    ASSERT_TRUE(Utils::TestEQPtrs(
        pou_var->GetPtrToField("global_int_arr[1][1]"),
        configuration.GetVariablePointerToMem(20, 0, "INT")));

    ASSERT_TRUE(Utils::TestEQPtrs(
        pou_var->GetPtrToField("global_int_arr[1][1]"),
        global_var->GetPtrToField("global_int_arr[1][1]")));

    EXPECT_EQ(pou_var->GetValueStoredAtField<int16_t>("start_int", 
                            DataTypeCategory::INT), 10);
    EXPECT_EQ(pou_var->GetValueStoredAtField<bool>("start", 
                            DataTypeCategory::BOOL), true);
    EXPECT_EQ(pou_var->GetValueStoredAtField<int16_t>("drv_1", 
                                    DataTypeCategory::INT), 10);
    TimeType time;
    time.SecsElapsed = 0;
    EXPECT_EQ(pou_var->GetValueStoredAtField<TimeType>("start_time", 
                                DataTypeCategory::TIME).SecsElapsed, 1);

    configuration.Cleanup();
}


TEST(VariableTestSuite, ConfigComplexDirectlyRepVariableTest) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/variable_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfigurationImpl configuration(TestDir + "/input.prototxt", false);
    PCVariable * global_var = configuration.__global_pou_var.get();
    ASSERT_TRUE(global_var != nullptr);
    EXPECT_EQ(global_var->GetPtrStoredAtField("complex_global"),
                    configuration.GetVariablePointerToMem(30, 0, 
                    "COMPLEX_STRUCT_1"));

    ASSERT_TRUE(Utils::TestEQPtrs(
        global_var->GetPtrToField("complex_global[1].string_field[1]"),
        configuration.GetVariablePointerToMem(30, 0, "CHAR")));

    ASSERT_TRUE(Utils::TestEQPtrs(
        global_var->GetPtrToField("complex_global[1].string_field[2]"),
        configuration.GetVariablePointerToMem(31, 0, "CHAR")));

    ASSERT_TRUE(Utils::TestEQPtrs(
        global_var->GetPtrToField("complex_global[1].string_field[40]"),
        configuration.GetVariablePointerToMem(69, 0, "CHAR")));

    EXPECT_EQ(global_var->GetValueStoredAtField<int16_t>(
            "complex_global[1].oned_arr_field[1]",
            DataTypeCategory::INT), -1);
    EXPECT_EQ(global_var->GetValueStoredAtField<int16_t>(
            "complex_global[1].oned_arr_field[2]",
            DataTypeCategory::INT), 0);
    EXPECT_EQ(global_var->GetValueStoredAtField<int16_t>(
            "complex_global[1].oned_arr_field[3]",
            DataTypeCategory::INT), 1);
    EXPECT_EQ(global_var->GetValueStoredAtField<int16_t>(
            "complex_global[1].oned_arr_field[4]",
            DataTypeCategory::INT), 2);
    EXPECT_EQ(global_var->GetValueStoredAtField<int16_t>(
            "complex_global[1].oned_arr_field[5]",
            DataTypeCategory::INT), 3);
    EXPECT_EQ(global_var->GetValueStoredAtField<int16_t>(
            "complex_global[1].oned_arr_field[6]",
            DataTypeCategory::INT), 4);
    EXPECT_EQ(global_var->GetValueStoredAtField<int16_t>(
            "complex_global[1].oned_arr_field[7]",
            DataTypeCategory::INT), 5);
    EXPECT_EQ(global_var->GetValueStoredAtField<int16_t>(
            "complex_global[1].oned_arr_field[8]",
            DataTypeCategory::INT), 6);
    EXPECT_EQ(global_var->GetValueStoredAtField<int16_t>(
            "complex_global[1].oned_arr_field[9]",
            DataTypeCategory::INT), 7);
    EXPECT_EQ(global_var->GetValueStoredAtField<int16_t>(
            "complex_global[1].oned_arr_field[10]",
            DataTypeCategory::INT), 8);

    EXPECT_EQ(global_var->GetValueStoredAtField<int16_t>(
            "complex_global[1].twod_arr_field[1][1]",
            DataTypeCategory::INT), 0);
    EXPECT_EQ(global_var->GetValueStoredAtField<int16_t>(
            "complex_global[1].twod_arr_field[1][2]",
            DataTypeCategory::INT), 1);
    EXPECT_EQ(global_var->GetValueStoredAtField<int16_t>(
            "complex_global[1].twod_arr_field[2][1]",
            DataTypeCategory::INT), 2);
    EXPECT_EQ(global_var->GetValueStoredAtField<int16_t>(
            "complex_global[1].twod_arr_field[2][2]",
            DataTypeCategory::INT), 3);

    EXPECT_EQ(global_var->GetValueStoredAtField<int16_t>(
            "complex_global_2[1].complex_vector[1].complex_field.oned_arr_field[1]",
            DataTypeCategory::INT), -1);
    EXPECT_EQ(global_var->GetValueStoredAtField<int16_t>(
            "complex_global_2[1].complex_vector[1].int_field",
            DataTypeCategory::INT), 10);
    configuration.Cleanup();
}



TEST(VariableTestSuite, AccessCheckTest) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/variable_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfigurationImpl configuration(TestDir + "/input.prototxt", false);
    
    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1", "input_1"),
                true);

    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1", "input_2"),
                true);

    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1", "output_1"),
                true);

    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1", "output_2"),
                true);  

    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1", "inout_1"),
                true);  

    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1", "inout_2"),
                true);  

    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1", "drv_1"),
                true); 

    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1", "global_int_arr"),
                true);

    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1", "start"),
                true);

    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1", "start_int"),
                true);


    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1", "start_time"),
                true);


    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1", "input_1"),
                false);

    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1", "input_2"),
                false);

    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1", "output_1"),
                true);

    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1", "output_2"),
                true);  

    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1", "inout_1"),
                true);  

    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1", "inout_2"),
                true);  

    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1", "drv_1"),
                true); 

    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1", "global_int_arr"),
                true);

    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1", "start"),
                true);

    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1", "start_int"),
                true);

    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1", "start_time"),
                true);


    // Checking internal FB variables

    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.input_1"), false); 
    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.output_1"), true); 
    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.inout_1"), true); 
    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.global_int_arr"), true); 
    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.start"), true); 
    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.start_time"), true);
    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.start_int"), true); 

    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.input_1"), true); 
    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.output_1"), false); 
    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.inout_1"), true); 
    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.global_int_arr"), true); 
    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.start"), true); 
    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.start_time"), true);
    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.start_int"), true);

    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.var_1.input_1"), false); 
    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.var_1.output_1"), false); 
    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.var_1.inout_1"), false); 
    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.var_1.global_int_arr"), false); 
    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.var_1.start"), false); 
    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.var_1.start_time"), false);
    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.var_1.start_int"), false);  


    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.var_1.input_1"), false); 
    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.var_1.output_1"), false); 
    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.var_1.inout_1"), false); 
    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.var_1.global_int_arr"), false); 
    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.var_1.start"), false); 
    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.var_1.start_time"), false);
    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "PROGRAM_1",
        "FB_1_Instance.var_1.start_int"), false);  

   EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "FB_1",
        "var_1.input_1"), false); 
    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "FB_1",
        "var_1.output_1"), true); 
    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "FB_1",
        "var_1.inout_1"), true); 
    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "FB_1",
        "var_1.global_int_arr"), true); 
    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "FB_1",
        "var_1.start"), true); 
    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "FB_1",
        "var_1.start_time"), true);
    EXPECT_EQ(Utils::ReadAccessCheck(&configuration, "FB_1",
        "var_1.start_int"), true);  


    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "FB_1",
        "var_1.input_1"), true); 
    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "FB_1",
        "var_1.output_1"), false); 
    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "FB_1",
        "var_1.inout_1"), true); 
    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "FB_1",
        "var_1.global_int_arr"), true); 
    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "FB_1",
        "var_1.start"), true); 
    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "FB_1",
        "var_1.start_time"), true);
    EXPECT_EQ(Utils::WriteAccessCheck(&configuration, "FB_1",
        "var_1.start_int"), true);      

    configuration.Cleanup();
}
