#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_resource.h"
#include "src/pc_emulator/include/pc_datatype.h"
#include "src/pc_emulator/include/pc_variable.h"
#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/include/insns/insn.h"
#include "src/pc_emulator/include/insns/ld_insn.h"

#include "gtest/gtest.h"

using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldInterfaceType = pc_specification::FieldInterfaceType;


TEST(SFCTestSuite, ConversionSFCTest) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/insn_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfigurationImpl configuration(TestDir + "/input.prototxt");

    PCResourceImpl * resource 
        = (PCResourceImpl*) configuration.RegisteredResources->GetResource(
                    "CPU_001");
    PCVariable * Temp_INT = resource->GetTmpVariable("INT", "15");
    PCVariable * Temp_BOOL = resource->GetTmpVariable("BOOL", "True");
    PCVariable * Temp_CHAR = resource->GetTmpVariable("CHAR", "a");
    PCVariable * Temp_REAL = resource->GetTmpVariable("REAL", "1.0");
    PCVariable * Temp_DATE = resource->GetTmpVariable("CHAR", "d#2020-01-02");
    PCVariable * Temp_BYTE = resource->GetTmpVariable("CHAR", "16#ff");

    PCVariable * Storage_INT = configuration.GetVariablePointerToMem(
        1, 0, "INT");
    PCVariable * Storage_BOOL = configuration.GetVariablePointerToMem(
        10, 0, "BOOL");
    PCVariable * Storage_CHAR = configuration.GetVariablePointerToMem(
        12, 0, "CHAR");
    PCVariable * Storage_REAL = configuration.GetVariablePointerToMem(
        13, 0, "REAL");
    PCVariable * Storage_TIME = configuration.GetVariablePointerToMem(
        17, 0, "TIME");
    PCVariable * Storage_DATE = configuration.GetVariablePointerToMem(
        23, 0, "DATE");
    PCVariable * Storage_DT = configuration.GetVariablePointerToMem(
         29, 0, "DT");
    PCVariable * Storage_BYTE = configuration.GetVariablePointerToMem(
         35, 0, "BYTE");

    std::vector<PCVariable*> Ops;
    PCVariable * CurrentResult;

    // INT to INT
    CurrentResult = Temp_INT;
    Ops.push_back(Storage_INT);
    EXPECT_EQ(Utils::GetMostAppropriateTypeCast(CurrentResult,
        Ops)->__DataTypeCategory, DataTypeCategory::INT);

    // INT to BOOL: Not possible
    CurrentResult = Temp_INT;
    Ops.clear();
    Ops.push_back(Storage_BOOL);
    EXPECT_EQ(Utils::GetMostAppropriateTypeCast(CurrentResult,
        Ops), nullptr);

    
    CurrentResult = Temp_INT;
    Ops.clear();
    Ops.push_back(Temp_BOOL);
    EXPECT_EQ(Utils::GetMostAppropriateTypeCast(CurrentResult,
        Ops), nullptr);


    // BOOL to INT: Not possible
    CurrentResult = Temp_BOOL;
    Ops.clear();
    Ops.push_back(Storage_INT);
    EXPECT_EQ(Utils::GetMostAppropriateTypeCast(CurrentResult,
        Ops), nullptr);


    // INT to REAL
    CurrentResult = Temp_INT;
    Ops.clear();
    Ops.push_back(Storage_REAL);
    EXPECT_EQ(Utils::GetMostAppropriateTypeCast(CurrentResult,
        Ops)->__DataTypeCategory, DataTypeCategory::REAL);

    CurrentResult = Temp_INT;
    Ops.clear();
    Ops.push_back(Temp_REAL);
    EXPECT_EQ(Utils::GetMostAppropriateTypeCast(CurrentResult,
        Ops)->__DataTypeCategory, DataTypeCategory::REAL);

    // REAL to INT
    CurrentResult = Temp_REAL;
    Ops.clear();
    Ops.push_back(Storage_INT);
    EXPECT_EQ(Utils::GetMostAppropriateTypeCast(CurrentResult,
        Ops)->__DataTypeCategory, DataTypeCategory::INT);

    // Both variables are TEMP, so REAL takes precedence over INT
    CurrentResult = Temp_REAL;
    Ops.clear();
    Ops.push_back(Temp_INT);
    EXPECT_EQ(Utils::GetMostAppropriateTypeCast(CurrentResult,
        Ops)->__DataTypeCategory, DataTypeCategory::REAL);


    // INT to CHAR: Not possible
    CurrentResult = Temp_INT;
    Ops.clear();
    Ops.push_back(Storage_CHAR);
    EXPECT_EQ(Utils::GetMostAppropriateTypeCast(CurrentResult,
        Ops), nullptr);

    CurrentResult = Temp_INT;
    Ops.clear();
    Ops.push_back(Temp_CHAR);
    EXPECT_EQ(Utils::GetMostAppropriateTypeCast(CurrentResult,
        Ops), nullptr);
    
    // CHAR to INT: Not possible
    CurrentResult = Temp_CHAR;
    Ops.clear();
    Ops.push_back(Storage_INT);
    EXPECT_EQ(Utils::GetMostAppropriateTypeCast(CurrentResult,
        Ops), nullptr);


    // BYTE to CHAR
    CurrentResult = Temp_BYTE;
    Ops.clear();
    Ops.push_back(Storage_CHAR);
    EXPECT_EQ(Utils::GetMostAppropriateTypeCast(CurrentResult,
        Ops)->__DataTypeCategory, DataTypeCategory::CHAR);

    // Both variables are TEMP, so CHAR takes precedence over byte
    CurrentResult = Temp_CHAR;
    Ops.clear();
    Ops.push_back(Temp_BYTE);
    EXPECT_EQ(Utils::GetMostAppropriateTypeCast(CurrentResult,
        Ops)->__DataTypeCategory, DataTypeCategory::CHAR);

    CurrentResult = Temp_BYTE;
    Ops.clear();
    Ops.push_back(Temp_CHAR);
    EXPECT_EQ(Utils::GetMostAppropriateTypeCast(CurrentResult,
        Ops)->__DataTypeCategory, DataTypeCategory::CHAR);

    // CHAR to BYTE
    CurrentResult = Temp_CHAR;
    Ops.clear();
    Ops.push_back(Storage_BYTE);
    EXPECT_EQ(Utils::GetMostAppropriateTypeCast(CurrentResult,
        Ops)->__DataTypeCategory, DataTypeCategory::BYTE);

    // DATE to REAL: Not possible
    CurrentResult = Temp_REAL;
    Ops.clear();
    Ops.push_back(Storage_DATE);
    EXPECT_EQ(Utils::GetMostAppropriateTypeCast(CurrentResult,
        Ops), nullptr);

    // REAL to DATE: Not possible
    CurrentResult = Temp_DATE;
    Ops.clear();
    Ops.push_back(Storage_REAL);
    EXPECT_EQ(Utils::GetMostAppropriateTypeCast(CurrentResult,
        Ops), nullptr);


    // REAL to BYTE: Not possible
    CurrentResult = Temp_REAL;
    Ops.clear();
    Ops.push_back(Temp_BYTE);
    EXPECT_EQ(Utils::GetMostAppropriateTypeCast(CurrentResult,
        Ops), nullptr);

    CurrentResult = Temp_REAL;
    Ops.clear();
    Ops.push_back(Storage_BYTE);
    EXPECT_EQ(Utils::GetMostAppropriateTypeCast(CurrentResult,
        Ops), nullptr);



    // BYTE to REAL: Not possible
    CurrentResult = Temp_BYTE;
    Ops.clear();
    Ops.push_back(Storage_REAL);
    EXPECT_EQ(Utils::GetMostAppropriateTypeCast(CurrentResult,
        Ops), nullptr);


    configuration.Cleanup();
    std::cout << "Finished Test !" << std::endl;
}


TEST(SFCTestSuite, ConversionSFCTest2) {
    string Result;
    DateTODDataType dt1, dt2;
    DateType date1, date2;
    TimeType time1, time2;
    TODDataType tod1, tod2;

    DataTypeUtils::BoolToAny(true, DataTypeCategory::BYTE, Result);
    EXPECT_EQ(Result, "16#1");
    DataTypeUtils::BoolToAny(true, DataTypeCategory::WORD, Result);
    EXPECT_EQ(Result, "16#1");
    DataTypeUtils::BoolToAny(true, DataTypeCategory::DWORD, Result);
    EXPECT_EQ(Result, "16#1");
    DataTypeUtils::BoolToAny(true, DataTypeCategory::LWORD, Result);
    EXPECT_EQ(Result, "16#1");

    DataTypeUtils::ByteToAny(0x1f, DataTypeCategory::BOOL, Result);
    EXPECT_EQ(Result, "1");
    DataTypeUtils::ByteToAny(0x1f, DataTypeCategory::WORD, Result);
    EXPECT_EQ(Result, "16#1f");
    DataTypeUtils::ByteToAny(0x1f, DataTypeCategory::DWORD, Result);
    EXPECT_EQ(Result, "16#1f");
    DataTypeUtils::ByteToAny(0x1f, DataTypeCategory::LWORD, Result);
    EXPECT_EQ(Result, "16#1f");

    DataTypeUtils::WordToAny(0x1ffe, DataTypeCategory::BOOL, Result);
    EXPECT_EQ(Result, "1");
    DataTypeUtils::WordToAny(0x1fffe, DataTypeCategory::BYTE, Result);
    EXPECT_EQ(Result, "16#fe");
    DataTypeUtils::WordToAny(0x1ffe, DataTypeCategory::DWORD, Result);
    EXPECT_EQ(Result, "16#1ffe");
    DataTypeUtils::WordToAny(0x1ffe, DataTypeCategory::LWORD, Result);
    EXPECT_EQ(Result, "16#1ffe");

    DataTypeUtils::DWordToAny(0x1ffe1ffe, DataTypeCategory::BOOL, Result);
    EXPECT_EQ(Result, "1");
    DataTypeUtils::DWordToAny(0x1fffe1ffe, DataTypeCategory::BYTE, Result);
    EXPECT_EQ(Result, "16#fe");
    DataTypeUtils::DWordToAny(0x1ffe1ffe, DataTypeCategory::WORD, Result);
    EXPECT_EQ(Result, "16#1ffe");
    DataTypeUtils::DWordToAny(0x1ffe1ffe, DataTypeCategory::LWORD, Result);
    EXPECT_EQ(Result, "16#1ffe1ffe");


    DataTypeUtils::LWordToAny(0x1ffe1ffe1ffe1ffe, DataTypeCategory::BOOL, Result);
    EXPECT_EQ(Result, "1");
    DataTypeUtils::LWordToAny(0x1fffe1ffe1ffe1ffe, DataTypeCategory::BYTE, Result);
    EXPECT_EQ(Result, "16#fe");
    DataTypeUtils::LWordToAny(0x1ffe1ffe1ffe1ffe, DataTypeCategory::WORD, Result);
    EXPECT_EQ(Result, "16#1ffe");
    DataTypeUtils::LWordToAny(0x1ffe1ffe1ffe1ffe, DataTypeCategory::DWORD, Result);
    EXPECT_EQ(Result, "16#1ffe1ffe");

    DataTypeUtils::CharToAny('a', DataTypeCategory::BYTE, Result);
    EXPECT_EQ(Result, "16#61");
    DataTypeUtils::ByteToAny(0x62, DataTypeCategory::CHAR, Result);
    EXPECT_EQ(Result[0], 'b');

    dt1.Date.Year = 2020;
    dt1.Date.Month = 1;
    dt1.Date.Day = 31;
    dt1.Tod.Hr = 0;
    dt1.Tod.Min = 0;
    dt1.Tod.Sec = 1;

    dt2.Date.Year = 2020;
    dt2.Date.Month = 1;
    dt2.Date.Day = 31;
    dt2.Tod.Hr = 0;
    dt2.Tod.Min = 1;
    dt2.Tod.Sec = 0;

    date1 = dt1.Date;

    time1.SecsElapsed = 59;

    EXPECT_EQ(DataTypeUtils::DTToDTString(dt1), "2020-01-31T00:00:01Z");
    EXPECT_EQ(DataTypeUtils::DateToDTString(date1), "2020-01-31T00:00:00Z");

    DataTypeUtils::AddToDT(dt1, time1);
    std::cout << DataTypeUtils::DTToDTString(dt1) << std::endl;
    EXPECT_EQ(dt1, dt2);


    dt1.Date.Year = 2020;
    dt1.Date.Month = 1;
    dt1.Date.Day = 31;
    dt1.Tod.Hr = 0;
    dt1.Tod.Min = 0;
    dt1.Tod.Sec = 1;

    dt2.Date.Year = 2020;
    dt2.Date.Month = 1;
    dt2.Date.Day = 30;
    dt2.Tod.Hr = 23;
    dt2.Tod.Min = 59;
    dt2.Tod.Sec = 2;
    DataTypeUtils::SubFromDT(dt1, time1);
    std::cout << DataTypeUtils::DTToDTString(dt1) << std::endl;
    EXPECT_EQ(dt1, dt2);

    tod1.Hr = 23;
    tod1.Min = 59;
    tod1.Sec = 1;

    tod2.Hr = 0;
    tod2.Min = 0;
    tod2.Sec = 0;

    DataTypeUtils::AddToTOD(tod1, time1);
    EXPECT_EQ(tod1, tod2);


    tod1.Hr = 23;
    tod1.Min = 59;
    tod1.Sec = 1;

    DataTypeUtils::SubFromTOD(tod2, time1);
    EXPECT_EQ(tod2, tod1);

    dt1.Date.Year = 2020;
    dt1.Date.Month = 1;
    dt1.Date.Day = 31;
    dt1.Tod.Hr = 0;
    dt1.Tod.Min = 0;
    dt1.Tod.Sec = 1;

    dt2.Date.Year = 2020;
    dt2.Date.Month = 1;
    dt2.Date.Day = 30;
    dt2.Tod.Hr = 23;
    dt2.Tod.Min = 59;
    dt2.Tod.Sec = 2;

    time1 = DataTypeUtils::SubDTs(dt1, dt2);
    EXPECT_EQ(time1.SecsElapsed, 59);

    time1 = DataTypeUtils::SubDTs(dt2, dt1);
    EXPECT_EQ(time1.SecsElapsed, -59);

    tod1.Hr = 23;
    tod1.Min = 59;
    tod1.Sec = 1;

    tod2.Hr = 0;
    tod2.Min = 0;
    tod2.Sec = 0;

    time1 = DataTypeUtils::SubTODs(tod1, tod2);
    EXPECT_EQ(time1.SecsElapsed, 23*3600 + 59*60 + 1);

    time1 = DataTypeUtils::SubTODs(tod2, tod1);
    EXPECT_EQ(time1.SecsElapsed, -23*3600 - 59*60 - 1);

    date1 = dt1.Date;
    date2 = dt2.Date;

    time1 = DataTypeUtils::SubDATEs(date1, date2);
    EXPECT_EQ(time1.SecsElapsed, 24*3600);

    time1 = DataTypeUtils::SubDATEs(date2, date1);
    EXPECT_EQ(time1.SecsElapsed, -24*3600);

}