
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_datatype.h"
#include "src/pc_emulator/include/utils.h"
#include "gtest/gtest.h"

using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

TEST(DataTypeTestSuite, Test1) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/datatype_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfiguration configuration(TestDir + "/input.prototxt");
    auto int_type_def = configuration.LookupDataType("INPUT_TYPE_DEF");

    EXPECT_EQ(int_type_def->__AliasName, "INT_TYPE_DEF");
    EXPECT_EQ(int_type_def->__DataTypeName, "INT");
    EXPECT_EQ(int_type_def->__RangeMin, -10);
    EXPECT_EQ(int_type_def->__RangeMax, 10);
    EXPECT_EQ(int_type_def->__InitialValue, "-1");
    EXPECT_EQ(int_type_def->__SizeInBits, 16);
    EXPECT_EQ(int_type_def->__NFields, 0);
    ASSERT_FALSE(int_type_def->IsFieldPresent("@"));
}