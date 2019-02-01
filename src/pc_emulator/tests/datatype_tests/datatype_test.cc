
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_datatype.h"
#include "src/pc_emulator/include/utils.h"
#include "gtest/gtest.h"

using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldInterfaceType = pc_specification::FieldInterfaceType;

TEST(DataTypeTestSuite, BasicDataTypeTest) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/datatype_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfiguration configuration(TestDir + "/input.prototxt");
    auto int_type_def = configuration.LookupDataType("INT_TYPE_DEF");
    auto int_1d = configuration.LookupDataType("INT_1DARR_TYPE_DEF");
    auto int_2d = configuration.LookupDataType("INT_2DARR_TYPE_DEF");

    PCDataTypeField Result;

    ASSERT_TRUE(int_type_def != nullptr);
    EXPECT_EQ(int_type_def->__AliasName, "INT_TYPE_DEF");
    EXPECT_EQ(int_type_def->__DataTypeName, "INT");
    EXPECT_EQ(int_type_def->__RangeMin, -10);
    EXPECT_EQ(int_type_def->__RangeMax, 10);
    EXPECT_EQ(int_type_def->__InitialValue, "-1");
    EXPECT_EQ(int_type_def->__SizeInBits, 16);
    EXPECT_EQ(int_type_def->__NFields, 0);
    ASSERT_FALSE(int_type_def->IsFieldPresent("@"));

    ASSERT_TRUE(int_1d != nullptr);
    EXPECT_EQ(int_1d->__AliasName, "INT_1DARR_TYPE_DEF");
    EXPECT_EQ(int_1d->__DataTypeName, "INT");
    EXPECT_EQ(int_1d->__RangeMin, -10);
    EXPECT_EQ(int_1d->__RangeMax, 10);
    EXPECT_EQ(int_1d->__DataTypeCategory, DataTypeCategory::ARRAY);
    EXPECT_EQ(int_1d->__SizeInBits, 16*10);
    EXPECT_EQ(int_1d->__NFields, 10);
    EXPECT_EQ(int_1d->GetPCDataTypeField("[1]", Result), true);
    EXPECT_EQ(Result.__InitialValue, "-1");
    EXPECT_EQ(Result.__RangeMin, -10);
    EXPECT_EQ(Result.__RangeMax, 10);
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(int_1d->GetPCDataTypeField("[5]", Result), true);
    EXPECT_EQ(Result.__InitialValue, "3");
    EXPECT_EQ(Result.__RangeMin, -10);
    EXPECT_EQ(Result.__RangeMax, 10);
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(int_1d->GetPCDataTypeField("[10]", Result), true);
    EXPECT_EQ(Result.__InitialValue, "8");
    EXPECT_EQ(Result.__RangeMin, -10);
    EXPECT_EQ(Result.__RangeMax, 10);
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
}