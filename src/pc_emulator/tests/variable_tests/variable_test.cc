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
    PCConfiguration configuration(TestDir + "/input.prototxt");
    PCVariable * global_var = configuration.__global_pou_var;
    ASSERT_TRUE(global_var != nullptr);
    EXPECT_EQ(global_var->GetPtrStoredAtField("global_bool_var"),
                    configuration.GetVariablePointerToMem(RAM_MEM, 3, 1, 
                    "BOOL"));
    EXPECT_EQ(global_var->GetPtrStoredAtField("global_int_var"),
                    configuration.GetVariablePointerToMem(RAM_MEM, 4, 0, 
                    "INT"));
    EXPECT_EQ(global_var->GetPtrStoredAtField("global_int_arr[1][1]"),
                configuration.GetVariablePointerToMem(RAM_MEM, 20, 0, 
                "INT"));
    EXPECT_EQ(global_var->GetPtrStoredAtField("global_int_arr[1][2]"),
                configuration.GetVariablePointerToMem(RAM_MEM, 22, 0, 
                "INT"));
    EXPECT_EQ(global_var->GetPtrStoredAtField("global_int_arr[2][1]"),
                configuration.GetVariablePointerToMem(RAM_MEM, 24, 0, 
                "INT"));
    EXPECT_EQ(global_var->GetPtrStoredAtField("global_int_arr[2][2]"),
                configuration.GetVariablePointerToMem(RAM_MEM, 26, 0, 
                "INT"));
    EXPECT_EQ(global_var->GetPtrStoredAtField("global_bool_arr[1]"),
                configuration.GetVariablePointerToMem(RAM_MEM, 10, 0, 
                "BOOL"));
    EXPECT_EQ(global_var->GetPtrStoredAtField("global_bool_arr[2]"),
                configuration.GetVariablePointerToMem(RAM_MEM, 10, 1, 
                "BOOL"));
    EXPECT_EQ(global_var->GetPtrStoredAtField("global_bool_arr[8]"),
                configuration.GetVariablePointerToMem(RAM_MEM, 10, 7, 
                "BOOL"));
    EXPECT_EQ(global_var->GetPtrStoredAtField("global_bool_arr[10]"),
            configuration.GetVariablePointerToMem(RAM_MEM, 11, 1, 
            "BOOL"));
}