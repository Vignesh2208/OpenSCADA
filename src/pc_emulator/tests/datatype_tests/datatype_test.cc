
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
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::INT);
    EXPECT_EQ(int_1d->GetPCDataTypeField("[5]", Result), true);
    EXPECT_EQ(Result.__InitialValue, "3");
    EXPECT_EQ(Result.__RangeMin, -10);
    EXPECT_EQ(Result.__RangeMax, 10);
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::INT);
    EXPECT_EQ(int_1d->GetPCDataTypeField("[10]", Result), true);
    EXPECT_EQ(Result.__InitialValue, "8");
    EXPECT_EQ(Result.__RangeMin, -10);
    EXPECT_EQ(Result.__RangeMax, 10);
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::INT);

    ASSERT_TRUE(int_2d != nullptr);
    EXPECT_EQ(int_2d->__AliasName, "INT_2DARR_TYPE_DEF");
    EXPECT_EQ(int_2d->__DataTypeName, "INT");
    EXPECT_EQ(int_2d->__RangeMin, -10);
    EXPECT_EQ(int_2d->__RangeMax, 10);
    EXPECT_EQ(int_2d->__DataTypeCategory, DataTypeCategory::ARRAY);
    EXPECT_EQ(int_2d->__SizeInBits, 16*4);
    EXPECT_EQ(int_2d->__NFields, 4);
    EXPECT_EQ(int_2d->GetPCDataTypeField("[1][1]", Result), true);
    EXPECT_EQ(Result.__InitialValue, "0");
    EXPECT_EQ(Result.__RangeMin, -10);
    EXPECT_EQ(Result.__RangeMax, 10);
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::INT);

    EXPECT_EQ(int_2d->GetPCDataTypeField("[1][2]", Result), true);
    EXPECT_EQ(Result.__InitialValue, "1");
    EXPECT_EQ(Result.__RangeMin, -10);
    EXPECT_EQ(Result.__RangeMax, 10);
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::INT);

    EXPECT_EQ(int_2d->GetPCDataTypeField("[2][1]", Result), true);
    EXPECT_EQ(Result.__InitialValue, "2");
    EXPECT_EQ(Result.__RangeMin, -10);
    EXPECT_EQ(Result.__RangeMax, 10);
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::INT);

    EXPECT_EQ(int_2d->GetPCDataTypeField("[2][2]", Result), true);
    EXPECT_EQ(Result.__InitialValue, "3");
    EXPECT_EQ(Result.__RangeMin, -10);
    EXPECT_EQ(Result.__RangeMax, 10);
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::INT);

    EXPECT_EQ(int_2d->GetPCDataTypeField("[2][3]", Result), false);

}


TEST(DataTypeTestSuite, ComplexDataTypeTest1) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/datatype_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfiguration configuration(TestDir + "/input.prototxt");
    auto complex_1 = configuration.LookupDataType("COMPLEX_STRUCT_1");
    ASSERT_TRUE(complex_1 != nullptr);
    EXPECT_EQ(complex_1->__AliasName, "COMPLEX_STRUCT_1");
    EXPECT_EQ(complex_1->__InitialValue,"");
    EXPECT_EQ(complex_1->__DataTypeName, "COMPLEX_STRUCT_1");
    EXPECT_EQ(complex_1->__DataTypeCategory, DataTypeCategory::DERIVED);
    
    PCDataTypeField Result;

    EXPECT_EQ(complex_1->GetPCDataTypeField("string_field", Result), true);
    EXPECT_EQ(Result.__InitialValue, "");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::ARRAY);
    EXPECT_EQ(Result.__FieldTypeName, "CHAR");
    EXPECT_EQ(Result.__FieldTypePtr, configuration.LookupDataType("CHAR"));
    EXPECT_EQ(Result.__NDimensions, 1);
    EXPECT_EQ(Result.__Dimension1, 1000);
    EXPECT_EQ(Result.__Dimension2, 1);

    EXPECT_EQ(complex_1->GetPCDataTypeField("string_field[1]", Result), true);
    EXPECT_EQ(Result.__InitialValue, "");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::CHAR);
    EXPECT_EQ(Result.__FieldTypeName, "CHAR");
    EXPECT_EQ(Result.__FieldTypePtr, configuration.LookupDataType("CHAR"));

    EXPECT_EQ(complex_1->GetPCDataTypeField("string_field[1000]", Result), true);
    EXPECT_EQ(Result.__InitialValue, "");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::CHAR);
    EXPECT_EQ(Result.__FieldTypeName, "CHAR");
    EXPECT_EQ(Result.__FieldTypePtr, configuration.LookupDataType("CHAR"));
    
    EXPECT_EQ(complex_1->GetPCDataTypeField("oned_arr_field", Result), true);
    EXPECT_EQ(Result.__InitialValue, "{-1,0,1,2,3,4,5,6,7,8}");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::ARRAY);
    EXPECT_EQ(Result.__RangeMax, 10);
    EXPECT_EQ(Result.__RangeMin, -10);
    EXPECT_EQ(Result.__FieldTypeName, "INT");
    EXPECT_EQ(Result.__FieldTypePtr,
                    configuration.LookupDataType("INT"));
    EXPECT_EQ(Result.__NDimensions, 1);
    EXPECT_EQ(Result.__Dimension1, 10);
    EXPECT_EQ(Result.__Dimension2, 1);

    EXPECT_EQ(complex_1->GetPCDataTypeField("oned_arr_field[1]", Result), true);
    EXPECT_EQ(Result.__InitialValue, "-1");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::INT);
    EXPECT_EQ(Result.__RangeMax, 10);
    EXPECT_EQ(Result.__RangeMin, -10);
    EXPECT_EQ(Result.__FieldTypeName, "INT");
    EXPECT_EQ(Result.__FieldTypePtr, configuration.LookupDataType("INT"));

    EXPECT_EQ(complex_1->GetPCDataTypeField("oned_arr_field[7]", Result), true);
    EXPECT_EQ(Result.__InitialValue, "5");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::INT);
    EXPECT_EQ(Result.__FieldTypeName, "INT");
    EXPECT_EQ(Result.__RangeMax, 10);
    EXPECT_EQ(Result.__RangeMin, -10);
    EXPECT_EQ(Result.__FieldTypePtr, configuration.LookupDataType("INT"));

    EXPECT_EQ(complex_1->GetPCDataTypeField("twod_arr_field", Result), true);
    EXPECT_EQ(Result.__InitialValue, "{{0,1},{2,3}}");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::ARRAY);
    EXPECT_EQ(Result.__RangeMax, 10);
    EXPECT_EQ(Result.__RangeMin, -10);
    EXPECT_EQ(Result.__FieldTypeName, "INT");
    EXPECT_EQ(Result.__FieldTypePtr,
                    configuration.LookupDataType("INT"));
    EXPECT_EQ(Result.__NDimensions, 2);
    EXPECT_EQ(Result.__Dimension1, 2);
    EXPECT_EQ(Result.__Dimension2, 2);

    EXPECT_EQ(complex_1->GetPCDataTypeField("twod_arr_field[1][1]", Result), true);
    EXPECT_EQ(Result.__InitialValue, "0");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::INT);
    EXPECT_EQ(Result.__RangeMax, 10);
    EXPECT_EQ(Result.__RangeMin, -10);
    EXPECT_EQ(Result.__FieldTypeName, "INT");
    EXPECT_EQ(Result.__FieldTypePtr, configuration.LookupDataType("INT"));

    EXPECT_EQ(complex_1->GetPCDataTypeField("twod_arr_field[2][2]", Result), true);
    EXPECT_EQ(Result.__InitialValue, "3");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::INT);
    EXPECT_EQ(Result.__FieldTypeName, "INT");
    EXPECT_EQ(Result.__RangeMax, 10);
    EXPECT_EQ(Result.__RangeMin, -10);
    EXPECT_EQ(Result.__FieldTypePtr, configuration.LookupDataType("INT"));
}



TEST(DataTypeTestSuite, ComplexDataTypeTest2) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/datatype_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfiguration configuration(TestDir + "/input.prototxt");
    auto complex_1 = configuration.LookupDataType("COMPLEX_STRUCT_2");
    ASSERT_TRUE(complex_1 != nullptr);
    EXPECT_EQ(complex_1->__AliasName, "COMPLEX_STRUCT_2");
    EXPECT_EQ(complex_1->__InitialValue,"");
    EXPECT_EQ(complex_1->__DataTypeName, "COMPLEX_STRUCT_2");
    EXPECT_EQ(complex_1->__DataTypeCategory, DataTypeCategory::DERIVED);
    
    PCDataTypeField Result;

    EXPECT_EQ(complex_1->GetPCDataTypeField("complex_field", Result), true);
    EXPECT_EQ(Result.__InitialValue, "");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::DERIVED);
    EXPECT_EQ(Result.__FieldTypeName, "COMPLEX_STRUCT_1");
    EXPECT_EQ(Result.__FieldTypePtr,
                    configuration.LookupDataType("COMPLEX_STRUCT_1"));

    EXPECT_EQ(complex_1->GetPCDataTypeField("complex_field.string_field",
                                                        Result), true);
    EXPECT_EQ(Result.__InitialValue, "");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::ARRAY);
    EXPECT_EQ(Result.__FieldTypeName, "CHAR");
    EXPECT_EQ(Result.__FieldTypePtr, configuration.LookupDataType("CHAR"));
    EXPECT_EQ(Result.__NDimensions, 1);
    EXPECT_EQ(Result.__Dimension1, 1000);
    EXPECT_EQ(Result.__Dimension2, 1);
    
    EXPECT_EQ(complex_1->GetPCDataTypeField("complex_field.string_field[1]",
                                                        Result), true);
    EXPECT_EQ(Result.__InitialValue, "");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::CHAR);
    EXPECT_EQ(Result.__FieldTypeName, "CHAR");
    EXPECT_EQ(Result.__FieldTypePtr, configuration.LookupDataType("CHAR"));

    
    EXPECT_EQ(complex_1->GetPCDataTypeField("complex_field.string_field[1000]",
                                            Result), true);
    EXPECT_EQ(Result.__InitialValue, "");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::CHAR);
    EXPECT_EQ(Result.__FieldTypeName, "CHAR");
    EXPECT_EQ(Result.__FieldTypePtr, configuration.LookupDataType("CHAR"));
    
    EXPECT_EQ(complex_1->GetPCDataTypeField("complex_field.oned_arr_field",
                                            Result), true);
    EXPECT_EQ(Result.__InitialValue, "{-1,0,1,2,3,4,5,6,7,8}");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::ARRAY);
    EXPECT_EQ(Result.__RangeMax, 10);
    EXPECT_EQ(Result.__RangeMin, -10);
    EXPECT_EQ(Result.__FieldTypeName, "INT");
    EXPECT_EQ(Result.__FieldTypePtr,
                    configuration.LookupDataType("INT"));
    EXPECT_EQ(Result.__NDimensions, 1);
    EXPECT_EQ(Result.__Dimension1, 10);
    EXPECT_EQ(Result.__Dimension2, 1);

    EXPECT_EQ(complex_1->GetPCDataTypeField("complex_field.oned_arr_field[1]",
                                            Result), true);
    EXPECT_EQ(Result.__InitialValue, "-1");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::INT);
    EXPECT_EQ(Result.__RangeMax, 10);
    EXPECT_EQ(Result.__RangeMin, -10);
    EXPECT_EQ(Result.__FieldTypeName, "INT");
    EXPECT_EQ(Result.__FieldTypePtr, configuration.LookupDataType("INT"));

    EXPECT_EQ(complex_1->GetPCDataTypeField("complex_field.oned_arr_field[7]",
                                            Result), true);
    EXPECT_EQ(Result.__InitialValue, "5");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::INT);
    EXPECT_EQ(Result.__FieldTypeName, "INT");
    EXPECT_EQ(Result.__RangeMax, 10);
    EXPECT_EQ(Result.__RangeMin, -10);
    EXPECT_EQ(Result.__FieldTypePtr, configuration.LookupDataType("INT"));

    EXPECT_EQ(complex_1->GetPCDataTypeField("complex_field.twod_arr_field",
                                            Result), true);
    EXPECT_EQ(Result.__InitialValue, "{{0,1},{2,3}}");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::ARRAY);
    EXPECT_EQ(Result.__RangeMax, 10);
    EXPECT_EQ(Result.__RangeMin, -10);
    EXPECT_EQ(Result.__FieldTypeName, "INT");
    EXPECT_EQ(Result.__FieldTypePtr,
                    configuration.LookupDataType("INT"));
    EXPECT_EQ(Result.__NDimensions, 2);
    EXPECT_EQ(Result.__Dimension1, 2);
    EXPECT_EQ(Result.__Dimension2, 2);

    EXPECT_EQ(complex_1->GetPCDataTypeField("complex_field.twod_arr_field[1][1]",
                                            Result), true);
    EXPECT_EQ(Result.__InitialValue, "0");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::INT);
    EXPECT_EQ(Result.__RangeMax, 10);
    EXPECT_EQ(Result.__RangeMin, -10);
    EXPECT_EQ(Result.__FieldTypeName, "INT");
    EXPECT_EQ(Result.__FieldTypePtr, configuration.LookupDataType("INT"));

    EXPECT_EQ(complex_1->GetPCDataTypeField("complex_field.twod_arr_field[2][2]",
                                            Result), true);
    EXPECT_EQ(Result.__InitialValue, "3");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::INT);
    EXPECT_EQ(Result.__FieldTypeName, "INT");
    EXPECT_EQ(Result.__RangeMax, 10);
    EXPECT_EQ(Result.__RangeMin, -10);
    EXPECT_EQ(Result.__FieldTypePtr, configuration.LookupDataType("INT"));
    
}


TEST(DataTypeTestSuite, ComplexDataTypeTest3) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/datatype_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfiguration configuration(TestDir + "/input.prototxt");
    auto complex_3 = configuration.LookupDataType("COMPLEX_STRUCT_3");
    ASSERT_TRUE(complex_3 != nullptr);
    EXPECT_EQ(complex_3->__AliasName, "COMPLEX_STRUCT_3");
    EXPECT_EQ(complex_3->__InitialValue,"");
    EXPECT_EQ(complex_3->__DataTypeName, "COMPLEX_STRUCT_3");
    EXPECT_EQ(complex_3->__DataTypeCategory, DataTypeCategory::DERIVED);
    
    PCDataTypeField Result;

    
    EXPECT_EQ(complex_3->GetPCDataTypeField("complex_vector", Result), true);
    EXPECT_EQ(Result.__InitialValue, "");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::ARRAY);
    EXPECT_EQ(Result.__FieldTypeName, "COMPLEX_STRUCT_2");
    EXPECT_EQ(Result.__FieldTypePtr,
                    configuration.LookupDataType("COMPLEX_STRUCT_2"));
    EXPECT_EQ(Result.__NDimensions, 1);
    EXPECT_EQ(Result.__Dimension1, 10);
    EXPECT_EQ(Result.__Dimension2, 1);

    EXPECT_EQ(complex_3->GetPCDataTypeField("complex_vector[1]", Result), true);
    EXPECT_EQ(Result.__InitialValue, "");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::DERIVED);
    EXPECT_EQ(Result.__FieldTypeName, "COMPLEX_STRUCT_2");
    EXPECT_EQ(Result.__FieldTypePtr,
                    configuration.LookupDataType("COMPLEX_STRUCT_2"));

    EXPECT_EQ(complex_3->GetPCDataTypeField("complex_vector[1].complex_field",
                                            Result), true);
    EXPECT_EQ(Result.__InitialValue, "");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::DERIVED);
    EXPECT_EQ(Result.__FieldTypeName, "COMPLEX_STRUCT_1");
    EXPECT_EQ(Result.__FieldTypePtr,
                    configuration.LookupDataType("COMPLEX_STRUCT_1"));
    EXPECT_EQ(Result.__FieldTypePtr->__DataTypeName, "COMPLEX_STRUCT_1");
    

    EXPECT_EQ(complex_3->GetPCDataTypeField(
        "complex_vector[1].complex_field.string_field",  Result), true);
    EXPECT_EQ(Result.__InitialValue, "");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::ARRAY);
    EXPECT_EQ(Result.__FieldTypeName, "CHAR");
    EXPECT_EQ(Result.__FieldTypePtr,
                    configuration.LookupDataType("CHAR"));
    EXPECT_EQ(Result.__NDimensions, 1);
    EXPECT_EQ(Result.__Dimension1, 1000);
    EXPECT_EQ(Result.__Dimension2, 1);

    EXPECT_EQ(complex_3->GetPCDataTypeField(
        "complex_vector[1].complex_field.twod_arr_field",  Result), true);
    EXPECT_EQ(Result.__InitialValue, "{{0,1},{2,3}}");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::ARRAY);
    EXPECT_EQ(Result.__FieldTypeName, "INT");
    EXPECT_EQ(Result.__FieldTypePtr,
                    configuration.LookupDataType("INT"));
    EXPECT_EQ(Result.__NDimensions, 2);
    EXPECT_EQ(Result.__Dimension1, 2);
    EXPECT_EQ(Result.__Dimension2, 2);


    EXPECT_EQ(complex_3->GetPCDataTypeField(
        "complex_vector[1].complex_field.string_field[2]",  Result), true);
    EXPECT_EQ(Result.__InitialValue, "");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::CHAR);
    EXPECT_EQ(Result.__FieldTypeName, "CHAR");
    EXPECT_EQ(Result.__FieldTypePtr,
                    configuration.LookupDataType("CHAR"));

    

    EXPECT_EQ(complex_3->GetPCDataTypeField(
        "complex_vector[1].complex_field.twod_arr_field[1][2]",  Result), true);
    EXPECT_EQ(Result.__InitialValue, "1");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::INT);
    EXPECT_EQ(Result.__FieldTypeName, "INT");
    EXPECT_EQ(Result.__FieldTypePtr,
                    configuration.LookupDataType("INT"));


    EXPECT_EQ(complex_3->GetPCDataTypeField("complex_vector[1].int_field", 
                                            Result), true);
    EXPECT_EQ(Result.__InitialValue, "-1");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::INT);
    EXPECT_EQ(Result.__FieldTypeName, "INT");
    EXPECT_EQ(Result.__FieldTypePtr,
                    configuration.LookupDataType("INT"));


    EXPECT_EQ(complex_3->GetPCDataTypeField("complex_vector[1].word_field", 
                                            Result), true);
    EXPECT_EQ(Result.__InitialValue, "16#1");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::WORD);
    EXPECT_EQ(Result.__FieldTypeName, "WORD");
    EXPECT_EQ(Result.__FieldTypePtr,
                    configuration.LookupDataType("WORD"));

    EXPECT_EQ(complex_3->GetPCDataTypeField("complex_vector[1].tod_field", 
                                            Result), true);
    EXPECT_EQ(Result.__InitialValue, "tod#00:00:00");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::TIME_OF_DAY);
    EXPECT_EQ(Result.__FieldTypeName, "TOD");
    EXPECT_EQ(Result.__FieldTypePtr,
                    configuration.LookupDataType("TOD"));

    EXPECT_EQ(complex_3->GetPCDataTypeField("complex_vector[1].date_tod_field", 
                                            Result), true);
    EXPECT_EQ(Result.__InitialValue, "dt#0001-01-01-00:00:00");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::DATE_AND_TIME);
    EXPECT_EQ(Result.__FieldTypeName, "DT");
    EXPECT_EQ(Result.__FieldTypePtr,
                    configuration.LookupDataType("DT"));

    EXPECT_EQ(complex_3->GetPCDataTypeField("complex_vector[1].date_field", 
                                            Result), true);
    EXPECT_EQ(Result.__InitialValue, "d#0001-01-01");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::DATE);
    EXPECT_EQ(Result.__FieldTypeName, "DATE");
    EXPECT_EQ(Result.__FieldTypePtr,
                    configuration.LookupDataType("DATE"));

    EXPECT_EQ(complex_3->GetPCDataTypeField("complex_vector[1].time_field", 
                                            Result), true);
    EXPECT_EQ(Result.__InitialValue, "t#0s");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::TIME);
    EXPECT_EQ(Result.__FieldTypeName, "TIME");
    EXPECT_EQ(Result.__FieldTypePtr,
                    configuration.LookupDataType("TIME"));

    EXPECT_EQ(complex_3->GetPCDataTypeField(
        "complex_vector[5].complex_field.real_field",  Result), true);
    EXPECT_EQ(Result.__InitialValue, "0.1");
    EXPECT_EQ(Result.__FieldInterfaceType, FieldInterfaceType::NA);
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::REAL);
    EXPECT_EQ(Result.__FieldTypeName, "REAL");
    EXPECT_EQ(Result.__FieldTypePtr,
                    configuration.LookupDataType("REAL"));
    EXPECT_EQ(Result.__RangeMin, -10);
    EXPECT_EQ(Result.__RangeMax, 10);
}


TEST(DataTypeTestSuite, GlobalVariablesTest) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/datatype_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfiguration configuration(TestDir + "/input.prototxt");
    auto global = configuration.LookupDataType("__CONFIG_GLOBAL__");
    ASSERT_TRUE(global != nullptr);
    EXPECT_EQ(global->__DataTypeCategory, DataTypeCategory::POU);
    EXPECT_EQ(global->__InitialValue, "");
    EXPECT_EQ(global->__AliasName, "__CONFIG_GLOBAL__");

    PCDataTypeField Result;
    ASSERT_TRUE(global->GetPCDataTypeField("global_bool_var", Result));
    EXPECT_EQ(Result.__FieldName, "global_bool_var");
    EXPECT_EQ(Result.__FieldTypeName, "BOOL");
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::BOOL);
    EXPECT_EQ(Result.__FieldTypePtr, configuration.LookupDataType("BOOL"));
    EXPECT_EQ(Result.__StorageByteOffset, 3);
    EXPECT_EQ(Result.__StorageBitOffset, 1);
    EXPECT_EQ(Result.__StorageMemType, RAM_MEM);


    ASSERT_TRUE(global->GetPCDataTypeField("global_int_var", Result));
    EXPECT_EQ(Result.__FieldName, "global_int_var");
    EXPECT_EQ(Result.__FieldTypeName, "INT");
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::INT);
    EXPECT_EQ(Result.__FieldTypePtr, configuration.LookupDataType("INT"));
    EXPECT_EQ(Result.__StorageByteOffset, 4);
    EXPECT_EQ(Result.__StorageBitOffset, 0);
    EXPECT_EQ(Result.__StorageMemType, RAM_MEM);


    ASSERT_TRUE(global->GetPCDataTypeField("global_bool_arr", Result));
    EXPECT_EQ(Result.__FieldName, "global_bool_arr");
    EXPECT_EQ(Result.__FieldTypeName, "BOOL");
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::ARRAY);
    EXPECT_EQ(Result.__FieldTypePtr, configuration.LookupDataType("BOOL"));
    EXPECT_EQ(Result.__StorageByteOffset, 10);
    EXPECT_EQ(Result.__StorageBitOffset, 0);
    EXPECT_EQ(Result.__StorageMemType, RAM_MEM);

    ASSERT_TRUE(global->GetPCDataTypeField("global_bool_arr[2]", Result));
    EXPECT_EQ(Result.__FieldName, "global_bool_arr[2]");
    EXPECT_EQ(Result.__FieldTypeName, "BOOL");
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::BOOL);
    EXPECT_EQ(Result.__FieldTypePtr, configuration.LookupDataType("BOOL"));
    EXPECT_EQ(Result.__StorageByteOffset, 10);
    EXPECT_EQ(Result.__StorageBitOffset, 1);
    EXPECT_EQ(Result.__StorageMemType, RAM_MEM);

    ASSERT_TRUE(global->GetPCDataTypeField("global_bool_arr[10]", Result));
    EXPECT_EQ(Result.__FieldName, "global_bool_arr[10]");
    EXPECT_EQ(Result.__FieldTypeName, "BOOL");
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::BOOL);
    EXPECT_EQ(Result.__FieldTypePtr, configuration.LookupDataType("BOOL"));
    EXPECT_EQ(Result.__StorageByteOffset, 11);
    EXPECT_EQ(Result.__StorageBitOffset, 1);
    EXPECT_EQ(Result.__StorageMemType, RAM_MEM);

    ASSERT_TRUE(global->GetPCDataTypeField("global_int_arr", Result));
    EXPECT_EQ(Result.__FieldName, "global_int_arr");
    EXPECT_EQ(Result.__FieldTypeName, "INT");
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::ARRAY);
    EXPECT_EQ(Result.__FieldTypePtr, configuration.LookupDataType("INT"));
    EXPECT_EQ(Result.__StorageByteOffset, 20);
    EXPECT_EQ(Result.__StorageBitOffset, 0);
    EXPECT_EQ(Result.__NDimensions, 2);
    EXPECT_EQ(Result.__Dimension1, 2);
    EXPECT_EQ(Result.__Dimension2, 2);
    EXPECT_EQ(Result.__StorageMemType, RAM_MEM);

    ASSERT_TRUE(global->GetPCDataTypeField("global_int_arr[1][2]", Result));
    EXPECT_EQ(Result.__FieldName, "global_int_arr[1][2]");
    EXPECT_EQ(Result.__FieldTypeName, "INT");
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::INT);
    EXPECT_EQ(Result.__FieldTypePtr, configuration.LookupDataType("INT"));
    EXPECT_EQ(Result.__StorageByteOffset, 22);
    EXPECT_EQ(Result.__StorageBitOffset, 0);
    EXPECT_EQ(Result.__StorageMemType, RAM_MEM);

    ASSERT_TRUE(global->GetPCDataTypeField("global_int_arr[2][2]", Result));
    EXPECT_EQ(Result.__FieldName, "global_int_arr[2][2]");
    EXPECT_EQ(Result.__FieldTypeName, "INT");
    EXPECT_EQ(Result.__FieldTypeCategory, DataTypeCategory::INT);
    EXPECT_EQ(Result.__FieldTypePtr, configuration.LookupDataType("INT"));
    EXPECT_EQ(Result.__StorageByteOffset, 26);
    EXPECT_EQ(Result.__StorageBitOffset, 0);
    EXPECT_EQ(Result.__StorageMemType, RAM_MEM);

}