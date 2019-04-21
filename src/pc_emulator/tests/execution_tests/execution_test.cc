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


TEST(ExecutionTestSuite, InsnCodeBodyProcessingTest) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/execution_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfigurationImpl configuration(TestDir + "/input.prototxt");

    PCResourceImpl * resource 
        = (PCResourceImpl*) configuration.RegisteredResources->GetResource(
                    "CPU_001");


    EXPECT_EQ(Utils::IsOperandImmediate("0"), true);
    EXPECT_EQ(Utils::IsOperandImmediate("16#ff"), true);
    EXPECT_EQ(Utils::IsOperandImmediate("tod#00:00:00"), true);
    EXPECT_EQ(Utils::IsOperandImmediate("\"Hello\""), true);
    EXPECT_EQ(Utils::IsOperandImmediate("dt#2020-10-10 00:00:01"), true);
    EXPECT_EQ(Utils::IsOperandImmediate("d#2020-01-01"), true);
    EXPECT_EQ(Utils::IsOperandImmediate("TRUE"), true);
    EXPECT_EQ(Utils::IsOperandImmediate("false"), true);
    EXPECT_EQ(Utils::IsOperandImmediate("PCVariable.Field1.Field2"), false);
    EXPECT_EQ(Utils::IsOperandImmediate("1234"), true);
    EXPECT_EQ(Utils::IsOperandImmediate("34.52"), true);

    EXPECT_EQ(resource->GetVariableForImmediateOperand("TRUE")
    ->__VariableDataType->__DataTypeCategory, DataTypeCategory::BOOL);
    EXPECT_EQ(resource->GetVariableForImmediateOperand("FALSE")
    ->__VariableDataType->__DataTypeCategory, DataTypeCategory::BOOL);
    EXPECT_EQ(resource->GetVariableForImmediateOperand("true")
    ->__VariableDataType->__DataTypeCategory, DataTypeCategory::BOOL);
    EXPECT_EQ(resource->GetVariableForImmediateOperand("false")
    ->__VariableDataType->__DataTypeCategory, DataTypeCategory::BOOL);
    EXPECT_EQ(resource->GetVariableForImmediateOperand("True")
    ->__VariableDataType->__DataTypeCategory, DataTypeCategory::BOOL);
    EXPECT_EQ(resource->GetVariableForImmediateOperand("False")
    ->__VariableDataType->__DataTypeCategory, DataTypeCategory::BOOL);
    EXPECT_EQ(resource->GetVariableForImmediateOperand("0")
    ->__VariableDataType->__DataTypeCategory, DataTypeCategory::INT);
    EXPECT_EQ(resource->GetVariableForImmediateOperand("16#ff")
    ->__VariableDataType->__DataTypeCategory, DataTypeCategory::BYTE);
    EXPECT_EQ(resource->GetVariableForImmediateOperand("16#ffff")
    ->__VariableDataType->__DataTypeCategory, DataTypeCategory::WORD);
    EXPECT_EQ(resource->GetVariableForImmediateOperand("16#ffffffff")
    ->__VariableDataType->__DataTypeCategory, DataTypeCategory::DWORD);
    EXPECT_EQ(resource->GetVariableForImmediateOperand("\"Hello\"")
    ->__VariableDataType->__DataTypeCategory, DataTypeCategory::ARRAY);
    EXPECT_EQ(resource->GetVariableForImmediateOperand("dt#2020-10-10 00:00:01")
    ->__VariableDataType->__DataTypeCategory, DataTypeCategory::DATE_AND_TIME);
    EXPECT_EQ(resource->GetVariableForImmediateOperand("d#2020-10-10")
    ->__VariableDataType->__DataTypeCategory, DataTypeCategory::DATE);
    EXPECT_EQ(resource->GetVariableForImmediateOperand("t#2020s")
    ->__VariableDataType->__DataTypeCategory, DataTypeCategory::TIME);
    EXPECT_EQ(resource->GetVariableForImmediateOperand("tod#10:10:10")
    ->__VariableDataType->__DataTypeCategory, DataTypeCategory::TIME_OF_DAY);
    EXPECT_EQ(resource->GetVariableForImmediateOperand("1234")
    ->__VariableDataType->__DataTypeCategory, DataTypeCategory::INT);
    EXPECT_EQ(resource->GetVariableForImmediateOperand("1234.52")
    ->__VariableDataType->__DataTypeCategory, DataTypeCategory::REAL);


    auto SR = resource->GetCodeContainer("SR");
    EXPECT_EQ(SR->GetInsn(0)->InsnName, "LD");
    EXPECT_EQ(SR->GetInsn(0)->OperandList[0], "R");
    EXPECT_EQ(SR->GetInsn(1)->InsnName, "AND");
    EXPECT_EQ(SR->GetInsn(1)->OperandList[0], "Q1");
    EXPECT_EQ(SR->GetInsn(2)->InsnName, "OR");
    EXPECT_EQ(SR->GetInsn(2)->OperandList[0], "S1");
    EXPECT_EQ(SR->GetInsn(3)->InsnName, "ST");
    EXPECT_EQ(SR->GetInsn(3)->OperandList[0], "Q1");
    EXPECT_EQ(SR->__InsnCount, 4);



    auto RS = resource->GetCodeContainer("RS");
    EXPECT_EQ(RS->GetInsn(0)->InsnName, "LD");
    EXPECT_EQ(RS->GetInsn(0)->OperandList[0], "S");
    EXPECT_EQ(RS->GetInsn(1)->InsnName, "OR");
    EXPECT_EQ(RS->GetInsn(1)->OperandList[0], "Q1");
    EXPECT_EQ(RS->GetInsn(2)->InsnName, "ANDN");
    EXPECT_EQ(RS->GetInsn(2)->OperandList[0], "R1");
    EXPECT_EQ(RS->__InsnCount, 3);


    auto R_TRIG = resource->GetCodeContainer("R_TRIG");
    EXPECT_EQ(R_TRIG->GetInsn(0)->InsnName, "NOT");
    EXPECT_EQ(R_TRIG->GetInsn(0)->OperandList[0], "MEM");
    EXPECT_EQ(R_TRIG->GetInsn(1)->InsnName, "AND");
    EXPECT_EQ(R_TRIG->GetInsn(1)->OperandList[0], "CLK");
    EXPECT_EQ(R_TRIG->GetInsn(2)->InsnName, "ST");
    EXPECT_EQ(R_TRIG->GetInsn(2)->OperandList[0], "Q");
    EXPECT_EQ(R_TRIG->GetInsn(3)->InsnName, "LD");
    EXPECT_EQ(R_TRIG->GetInsn(3)->OperandList[0], "CLK");
    EXPECT_EQ(R_TRIG->GetInsn(4)->InsnName, "ST");
    EXPECT_EQ(R_TRIG->GetInsn(4)->OperandList[0], "MEM");
    EXPECT_EQ(R_TRIG->__InsnCount, 5);


    auto F_TRIG = resource->GetCodeContainer("F_TRIG");
    EXPECT_EQ(F_TRIG->GetInsn(0)->InsnName, "NOT");
    EXPECT_EQ(F_TRIG->GetInsn(0)->OperandList[0], "MEM");
    EXPECT_EQ(F_TRIG->GetInsn(1)->InsnName, "ANDN");
    EXPECT_EQ(F_TRIG->GetInsn(1)->OperandList[0], "CLK");
    EXPECT_EQ(F_TRIG->GetInsn(2)->InsnName, "ST");
    EXPECT_EQ(F_TRIG->GetInsn(2)->OperandList[0], "Q");
    EXPECT_EQ(F_TRIG->GetInsn(3)->InsnName, "NOT");
    EXPECT_EQ(F_TRIG->GetInsn(3)->OperandList[0], "CLK");
    EXPECT_EQ(F_TRIG->GetInsn(4)->InsnName, "ST");
    EXPECT_EQ(F_TRIG->GetInsn(4)->OperandList[0], "MEM");
    EXPECT_EQ(F_TRIG->__InsnCount, 5);

    auto CTU = resource->GetCodeContainer("CTU");
    EXPECT_EQ(CTU->GetInsn(0)->InsnName, "LD");
    EXPECT_EQ(CTU->GetInsn(0)->OperandList[0], "R");
    EXPECT_EQ(CTU->GetInsn(1)->InsnName, "JMPCN");
    EXPECT_EQ(CTU->GetInsn(1)->OperandList[0], "else");
    EXPECT_EQ(CTU->GetInsn(2)->InsnName, "LD");
    EXPECT_EQ(CTU->GetInsn(2)->OperandList[0], "0");
    EXPECT_EQ(CTU->GetInsn(3)->InsnName, "ST");
    EXPECT_EQ(CTU->GetInsn(3)->OperandList[0], "CV");
    EXPECT_EQ(CTU->GetInsn(4)->InsnName, "JMP");
    EXPECT_EQ(CTU->GetInsn(4)->OperandList[0], "end");
    EXPECT_EQ(CTU->GetInsn(5)->InsnName, "LD");
    EXPECT_EQ(CTU->GetInsn(5)->OperandList[0], "CU");
    EXPECT_EQ(CTU->GetInsn(5)->InsnLabel, "else");
    EXPECT_EQ(CTU->GetInsn(6)->InsnName, "JMPCN");
    EXPECT_EQ(CTU->GetInsn(6)->OperandList[0], "end");
    EXPECT_EQ(CTU->GetInsn(7)->InsnName, "LD");
    EXPECT_EQ(CTU->GetInsn(7)->OperandList[0], "PV");
    EXPECT_EQ(CTU->GetInsn(8)->InsnName, "GT");
    EXPECT_EQ(CTU->GetInsn(8)->OperandList[0], "CV");
    EXPECT_EQ(CTU->GetInsn(9)->InsnName, "JMPCN");
    EXPECT_EQ(CTU->GetInsn(9)->OperandList[0], "end");
    EXPECT_EQ(CTU->GetInsn(10)->InsnName, "LD");
    EXPECT_EQ(CTU->GetInsn(10)->OperandList[0], "CV");
    EXPECT_EQ(CTU->GetInsn(11)->InsnName, "ADD");
    EXPECT_EQ(CTU->GetInsn(11)->OperandList[0], "1");
    EXPECT_EQ(CTU->GetInsn(12)->InsnName, "ST");
    EXPECT_EQ(CTU->GetInsn(12)->OperandList[0], "CV");
    EXPECT_EQ(CTU->GetInsn(13)->InsnName, "LD");
    EXPECT_EQ(CTU->GetInsn(13)->OperandList[0], "CV");
    EXPECT_EQ(CTU->GetInsn(13)->InsnLabel, "end");
    EXPECT_EQ(CTU->GetInsn(14)->InsnName, "GE");
    EXPECT_EQ(CTU->GetInsn(14)->OperandList[0], "PV");
    EXPECT_EQ(CTU->GetInsn(15)->InsnName, "ST");
    EXPECT_EQ(CTU->GetInsn(15)->OperandList[0], "Q");
    EXPECT_EQ(CTU->__InsnCount, 16);


    auto CTD = resource->GetCodeContainer("CTD");
    EXPECT_EQ(CTD->GetInsn(0)->InsnName, "LD");
    EXPECT_EQ(CTD->GetInsn(0)->OperandList[0], "LD");
    EXPECT_EQ(CTD->GetInsn(1)->InsnName, "JMPCN");
    EXPECT_EQ(CTD->GetInsn(1)->OperandList[0], "else");
    EXPECT_EQ(CTD->GetInsn(2)->InsnName, "LD");
    EXPECT_EQ(CTD->GetInsn(2)->OperandList[0], "PV");
    EXPECT_EQ(CTD->GetInsn(3)->InsnName, "ST");
    EXPECT_EQ(CTD->GetInsn(3)->OperandList[0], "CV");
    EXPECT_EQ(CTD->GetInsn(4)->InsnName, "JMP");
    EXPECT_EQ(CTD->GetInsn(4)->OperandList[0], "end");
    EXPECT_EQ(CTD->GetInsn(5)->InsnName, "LD");
    EXPECT_EQ(CTD->GetInsn(5)->OperandList[0], "CD");
    EXPECT_EQ(CTD->GetInsn(5)->InsnLabel, "else");
    EXPECT_EQ(CTD->GetInsn(6)->InsnName, "JMPCN");
    EXPECT_EQ(CTD->GetInsn(6)->OperandList[0], "end");
    EXPECT_EQ(CTD->GetInsn(7)->InsnName, "LD");
    EXPECT_EQ(CTD->GetInsn(7)->OperandList[0], "CV");
    EXPECT_EQ(CTD->GetInsn(8)->InsnName, "GT");
    EXPECT_EQ(CTD->GetInsn(8)->OperandList[0], "0");
    EXPECT_EQ(CTD->GetInsn(9)->InsnName, "JMPCN");
    EXPECT_EQ(CTD->GetInsn(9)->OperandList[0], "end");
    EXPECT_EQ(CTD->GetInsn(10)->InsnName, "LD");
    EXPECT_EQ(CTD->GetInsn(10)->OperandList[0], "CV");
    EXPECT_EQ(CTD->GetInsn(11)->InsnName, "SUB");
    EXPECT_EQ(CTD->GetInsn(11)->OperandList[0], "1");
    EXPECT_EQ(CTD->GetInsn(12)->InsnName, "ST");
    EXPECT_EQ(CTD->GetInsn(12)->OperandList[0], "CV");
    EXPECT_EQ(CTD->GetInsn(13)->InsnName, "LD");
    EXPECT_EQ(CTD->GetInsn(13)->OperandList[0], "CV");
    EXPECT_EQ(CTD->GetInsn(13)->InsnLabel, "end");
    EXPECT_EQ(CTD->GetInsn(14)->InsnName, "GE");
    EXPECT_EQ(CTD->GetInsn(14)->OperandList[0], "0");
    EXPECT_EQ(CTD->GetInsn(15)->InsnName, "ST");
    EXPECT_EQ(CTD->GetInsn(15)->OperandList[0], "Q");
    EXPECT_EQ(CTD->__InsnCount, 16);

    std::unordered_map<string, string> VarsToSet;
    std::unordered_map<string, string> VarsToGet;
    Utils::ExtractCallInterfaceMapping(VarsToSet, VarsToGet, 
    "(Var1 := Var2, Var3 := Var4, Var5 := Var6, Var7 => Var8, Var9 => Var10)");
    EXPECT_EQ(VarsToSet.find("Var1")->second, "Var2");
    EXPECT_EQ(VarsToSet.find("Var3")->second, "Var4");
    EXPECT_EQ(VarsToSet.find("Var5")->second, "Var6");
    EXPECT_EQ(VarsToSet.find("Var9"), VarsToSet.end());
    EXPECT_EQ(VarsToGet.find("Var7")->second, "Var8");
    EXPECT_EQ(VarsToGet.find("Var9")->second, "Var10");

    VarsToGet.clear();
    VarsToSet.clear();
    Utils::ExtractCallInterfaceMapping(VarsToSet, VarsToGet, 
    "Var1 := Var2,  Var3 := Var4,   Var5 := Var6,   Var7 => Var8,  Var9 => Var10");
    EXPECT_EQ(VarsToSet.find("Var1")->second, "Var2");
    EXPECT_EQ(VarsToSet.find("Var3")->second, "Var4");
    EXPECT_EQ(VarsToSet.find("Var5")->second, "Var6");
    EXPECT_EQ(VarsToSet.find("Var9"), VarsToSet.end());
    EXPECT_EQ(VarsToGet.find("Var7")->second, "Var8");
    EXPECT_EQ(VarsToGet.find("Var9")->second, "Var10");

    VarsToGet.clear();
    VarsToSet.clear();
    Utils::ExtractCallInterfaceMapping(VarsToSet, VarsToGet, 
    "(Var1:=Var2,Var3:=Var4,Var5:=Var6,Var7=>Var8,Var9=>Var10)");
    EXPECT_EQ(VarsToSet.find("Var1")->second, "Var2");
    EXPECT_EQ(VarsToSet.find("Var3")->second, "Var4");
    EXPECT_EQ(VarsToSet.find("Var5")->second, "Var6");
    EXPECT_EQ(VarsToSet.find("Var9"), VarsToSet.end());
    EXPECT_EQ(VarsToGet.find("Var7")->second, "Var8");
    EXPECT_EQ(VarsToGet.find("Var9")->second, "Var10");

    configuration.Cleanup();
    std::cout << "Finished Test !" << std::endl;
}

