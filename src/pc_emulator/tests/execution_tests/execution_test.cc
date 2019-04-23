#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_resource.h"
#include "src/pc_emulator/include/pc_datatype.h"
#include "src/pc_emulator/include/pc_variable.h"
#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/include/insns/insn.h"
#include "src/pc_emulator/include/insns/ld_insn.h"
#include "src/pc_emulator/include/pc_clock.h"

#include "gtest/gtest.h"

#include <ctime>
#include <ratio>
#include <chrono>
#include <thread>

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
    EXPECT_EQ(SR->GetInsn(0)->InsnName, "NOT");
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
    EXPECT_EQ(SR->GetInsn(3)->InsnName, "ST");
    EXPECT_EQ(SR->GetInsn(3)->OperandList[0], "Q1");
    EXPECT_EQ(RS->__InsnCount, 4);


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
    EXPECT_EQ(CTD->GetInsn(14)->InsnName, "LE");
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


TEST(ExecutionTestSuite, R_TRIG_Execution_Test) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/execution_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfigurationImpl configuration(TestDir + "/input.prototxt");

    PCResourceImpl * resource 
        = (PCResourceImpl*) configuration.RegisteredResources->GetResource(
                    "CPU_001");

    PCVariable * R_TRIG_FB = resource->GetPOU("R_TRIG");
    IntervalTaskSpecification task_spec;
    IntervalTaskParams*  task_params = new IntervalTaskParams;
    task_spec.set_task_name("Task1");
    task_spec.set_priority(1);
    task_params->set_interval_ms(10);
    task_spec.set_allocated_interval_task_params(task_params);
    Task new_task((PCConfigurationImpl*)&configuration, resource, task_spec);
    Executor new_executor(&configuration, resource, &new_task);
    new_executor.SetExecPoUVariable(R_TRIG_FB);
    R_TRIG_FB->SetField("CLK", "FALSE");
    new_executor.Run();
    EXPECT_EQ(R_TRIG_FB->GetValueStoredAtField<bool>("Q",
                DataTypeCategory::BOOL), false);
    R_TRIG_FB->SetField("CLK", "TRUE");
    new_executor.Run();
    EXPECT_EQ(R_TRIG_FB->GetValueStoredAtField<bool>("Q",
                DataTypeCategory::BOOL), true);
    R_TRIG_FB->SetField("CLK", "TRUE");
    new_executor.Run();
    EXPECT_EQ(R_TRIG_FB->GetValueStoredAtField<bool>("Q",
                DataTypeCategory::BOOL), false);
    R_TRIG_FB->SetField("CLK", "FALSE");
    new_executor.Run();
    EXPECT_EQ(R_TRIG_FB->GetValueStoredAtField<bool>("Q",
                DataTypeCategory::BOOL), false);

    R_TRIG_FB->SetField("CLK", "TRUE");
    new_executor.Run();
    EXPECT_EQ(R_TRIG_FB->GetValueStoredAtField<bool>("Q",
                DataTypeCategory::BOOL), true);
    new_executor.CleanUp();
    configuration.Cleanup();
}

TEST(ExecutionTestSuite, F_TRIG_Execution_Test) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/execution_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfigurationImpl configuration(TestDir + "/input.prototxt");

    PCResourceImpl * resource 
        = (PCResourceImpl*) configuration.RegisteredResources->GetResource(
                    "CPU_001");

    PCVariable * F_TRIG_FB = resource->GetPOU("F_TRIG");
    IntervalTaskSpecification task_spec;
    IntervalTaskParams*  task_params = new IntervalTaskParams;
    task_spec.set_task_name("Task1");
    task_spec.set_priority(1);
    task_params->set_interval_ms(10);
    task_spec.set_allocated_interval_task_params(task_params);
    Task new_task((PCConfigurationImpl*)&configuration, resource, task_spec);
    Executor new_executor(&configuration, resource, &new_task);
    new_executor.SetExecPoUVariable(F_TRIG_FB);
    
    F_TRIG_FB->SetField("CLK", "FALSE");
    new_executor.Run();
    EXPECT_EQ(F_TRIG_FB->GetValueStoredAtField<bool>("Q",
                DataTypeCategory::BOOL), false);

    F_TRIG_FB->SetField("CLK", "TRUE");
    new_executor.Run();
    EXPECT_EQ(F_TRIG_FB->GetValueStoredAtField<bool>("Q",
                DataTypeCategory::BOOL), false);

    F_TRIG_FB->SetField("CLK", "TRUE");
    new_executor.Run();
    EXPECT_EQ(F_TRIG_FB->GetValueStoredAtField<bool>("Q",
                DataTypeCategory::BOOL), false);

    F_TRIG_FB->SetField("CLK", "FALSE");
    new_executor.Run();
    EXPECT_EQ(F_TRIG_FB->GetValueStoredAtField<bool>("Q",
                DataTypeCategory::BOOL), true);

    F_TRIG_FB->SetField("CLK", "FALSE");
    new_executor.Run();
    EXPECT_EQ(F_TRIG_FB->GetValueStoredAtField<bool>("Q",
                DataTypeCategory::BOOL), false);
    new_executor.CleanUp();

    F_TRIG_FB->SetField("CLK", "TRUE");
    new_executor.Run();
    EXPECT_EQ(F_TRIG_FB->GetValueStoredAtField<bool>("Q",
                DataTypeCategory::BOOL), false);


    F_TRIG_FB->SetField("CLK", "FALSE");
    new_executor.Run();
    EXPECT_EQ(F_TRIG_FB->GetValueStoredAtField<bool>("Q",
                DataTypeCategory::BOOL), true);

    new_executor.CleanUp();
    configuration.Cleanup();
}


TEST(ExecutionTestSuite, SR_FlipFlop_Execution_Test) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/execution_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfigurationImpl configuration(TestDir + "/input.prototxt");

    PCResourceImpl * resource 
        = (PCResourceImpl*) configuration.RegisteredResources->GetResource(
                    "CPU_001");

    PCVariable * SR_FB = resource->GetPOU("SR");
    IntervalTaskSpecification task_spec;
    IntervalTaskParams*  task_params = new IntervalTaskParams;
    task_spec.set_task_name("Task1");
    task_spec.set_priority(1);
    task_params->set_interval_ms(10);
    task_spec.set_allocated_interval_task_params(task_params);
    Task new_task((PCConfigurationImpl*)&configuration, resource, task_spec);
    Executor new_executor(&configuration, resource, &new_task);

    new_executor.SetExecPoUVariable(SR_FB);
    SR_FB->SetField("S1", "FALSE");
    SR_FB->SetField("R", "FALSE");
    new_executor.Run();
    EXPECT_EQ(SR_FB->GetValueStoredAtField<bool>("Q1",
                DataTypeCategory::BOOL), false);

    SR_FB->SetField("S1", "TRUE");
    SR_FB->SetField("R", "FALSE");
    new_executor.Run();
    EXPECT_EQ(SR_FB->GetValueStoredAtField<bool>("Q1",
                DataTypeCategory::BOOL), true);

    SR_FB->SetField("S1", "FALSE");
    SR_FB->SetField("R", "TRUE");
    new_executor.Run();
    EXPECT_EQ(SR_FB->GetValueStoredAtField<bool>("Q1",
                DataTypeCategory::BOOL), false);

    SR_FB->SetField("S1", "TRUE");
    SR_FB->SetField("R", "TRUE");
    new_executor.Run();
    EXPECT_EQ(SR_FB->GetValueStoredAtField<bool>("Q1",
                DataTypeCategory::BOOL), true);

    SR_FB->SetField("S1", "FALSE");
    SR_FB->SetField("R", "FALSE");
    new_executor.Run();
    EXPECT_EQ(SR_FB->GetValueStoredAtField<bool>("Q1",
                DataTypeCategory::BOOL), true);

    SR_FB->SetField("S1", "FALSE");
    SR_FB->SetField("R", "FALSE");
    new_executor.Run();
    EXPECT_EQ(SR_FB->GetValueStoredAtField<bool>("Q1",
                DataTypeCategory::BOOL), true);

    new_executor.CleanUp();
    configuration.Cleanup();

}


TEST(ExecutionTestSuite, RS_FlipFlop_Execution_Test) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/execution_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfigurationImpl configuration(TestDir + "/input.prototxt");

    PCResourceImpl * resource 
        = (PCResourceImpl*) configuration.RegisteredResources->GetResource(
                    "CPU_001");

    PCVariable * RS_FB = resource->GetPOU("RS");
    IntervalTaskSpecification task_spec;
    IntervalTaskParams*  task_params = new IntervalTaskParams;
    task_spec.set_task_name("Task1");
    task_spec.set_priority(1);
    task_params->set_interval_ms(10);
    task_spec.set_allocated_interval_task_params(task_params);
    Task new_task((PCConfigurationImpl*)&configuration, resource, task_spec);
    Executor new_executor(&configuration, resource, &new_task);

    new_executor.SetExecPoUVariable(RS_FB);
    RS_FB->SetField("S", "FALSE");
    RS_FB->SetField("R1", "FALSE");
    new_executor.Run();
    EXPECT_EQ(RS_FB->GetValueStoredAtField<bool>("Q1",
                DataTypeCategory::BOOL), false);

    RS_FB->SetField("S", "TRUE");
    RS_FB->SetField("R1", "FALSE");
    new_executor.Run();
    EXPECT_EQ(RS_FB->GetValueStoredAtField<bool>("Q1",
                DataTypeCategory::BOOL), true);

    RS_FB->SetField("S", "FALSE");
    RS_FB->SetField("R1", "TRUE");
    new_executor.Run();
    EXPECT_EQ(RS_FB->GetValueStoredAtField<bool>("Q1",
                DataTypeCategory::BOOL), false);

    RS_FB->SetField("S", "TRUE");
    RS_FB->SetField("R1", "TRUE");
    new_executor.Run();
    EXPECT_EQ(RS_FB->GetValueStoredAtField<bool>("Q1",
                DataTypeCategory::BOOL), false);

    
    RS_FB->SetField("S", "FALSE");
    RS_FB->SetField("R1", "FALSE");
    new_executor.Run();
    EXPECT_EQ(RS_FB->GetValueStoredAtField<bool>("Q1",
                DataTypeCategory::BOOL), false);

    RS_FB->SetField("S", "TRUE");
    RS_FB->SetField("R1", "FALSE");
    new_executor.Run();
    EXPECT_EQ(RS_FB->GetValueStoredAtField<bool>("Q1",
                DataTypeCategory::BOOL), true);

    RS_FB->SetField("S", "FALSE");
    RS_FB->SetField("R1", "FALSE");
    new_executor.Run();
    EXPECT_EQ(RS_FB->GetValueStoredAtField<bool>("Q1",
                DataTypeCategory::BOOL), true);

    new_executor.CleanUp();
    configuration.Cleanup();

}


TEST(ExecutionTestSuite, CTU_Counter_Execution_Test) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/execution_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfigurationImpl configuration(TestDir + "/input.prototxt");

    PCResourceImpl * resource 
        = (PCResourceImpl*) configuration.RegisteredResources->GetResource(
                    "CPU_001");
    
    PCVariable * CTU_FB = resource->GetPOU("CTU");
    IntervalTaskSpecification task_spec;
    IntervalTaskParams*  task_params = new IntervalTaskParams;
    task_spec.set_task_name("Task1");
    task_spec.set_priority(1);
    task_params->set_interval_ms(10);
    task_spec.set_allocated_interval_task_params(task_params);
    Task new_task((PCConfigurationImpl*)&configuration, resource, task_spec);
    Executor new_executor(&configuration, resource, &new_task);

    new_executor.SetExecPoUVariable(CTU_FB);
    CTU_FB->SetField("R", "FALSE");
    CTU_FB->SetField("PV", "5");
    CTU_FB->SetField("CU", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 0);
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);
    std::cout << "################################\n";
    
    CTU_FB->SetField("CU", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 1);
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);

    std::cout << "################################\n";

    
    CTU_FB->SetField("CU", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 1);
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);

    std::cout << "################################\n";

    CTU_FB->SetField("CU", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 2);
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);
    std::cout << "################################\n";


    CTU_FB->SetField("CU", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 2);
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);

    std::cout << "################################\n";

    CTU_FB->SetField("CU", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 3);
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);
    std::cout << "################################\n";

    CTU_FB->SetField("CU", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 3);
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);

    std::cout << "################################\n";

    CTU_FB->SetField("CU", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 4);
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);
    std::cout << "################################\n";

    CTU_FB->SetField("CU", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 4);
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);

    std::cout << "################################\n";

    CTU_FB->SetField("CU", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 5);
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), true);
    std::cout << "################################\n";


    CTU_FB->SetField("CU", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 5);
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), true);

    std::cout << "################################\n";

    CTU_FB->SetField("CU", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 5);
    EXPECT_EQ(CTU_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), true);
    std::cout << "################################\n";
    

    new_executor.CleanUp();
    configuration.Cleanup();

}


TEST(ExecutionTestSuite, CTD_Counter_Execution_Test) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/execution_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfigurationImpl configuration(TestDir + "/input.prototxt");

    PCResourceImpl * resource 
        = (PCResourceImpl*) configuration.RegisteredResources->GetResource(
                    "CPU_001");
    
    PCVariable * CTD_FB = resource->GetPOU("CTD");
    IntervalTaskSpecification task_spec;
    IntervalTaskParams*  task_params = new IntervalTaskParams;
    task_spec.set_task_name("Task1");
    task_spec.set_priority(1);
    task_params->set_interval_ms(10);
    task_spec.set_allocated_interval_task_params(task_params);
    Task new_task((PCConfigurationImpl*)&configuration, resource, task_spec);
    Executor new_executor(&configuration, resource, &new_task);

    new_executor.SetExecPoUVariable(CTD_FB);
    CTD_FB->SetField("LD", "TRUE");
    CTD_FB->SetField("PV", "5");
    CTD_FB->SetField("CD", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 5);
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);
    std::cout << "################################\n";
    
    CTD_FB->SetField("LD", "FALSE");
    CTD_FB->SetField("CD", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 4);
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);

    std::cout << "################################\n";

    
    CTD_FB->SetField("CD", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 4);
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);

    std::cout << "################################\n";

    CTD_FB->SetField("CD", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 3);
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);
    std::cout << "################################\n";


    CTD_FB->SetField("CD", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 3);
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);

    std::cout << "################################\n";

    CTD_FB->SetField("CD", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 2);
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);
    std::cout << "################################\n";

    CTD_FB->SetField("CD", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 2);
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);

    std::cout << "################################\n";

    CTD_FB->SetField("CD", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 1);
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);
    std::cout << "################################\n";

    CTD_FB->SetField("CD", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 1);
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);

    std::cout << "################################\n";

    CTD_FB->SetField("CD", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 0);
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), true);
    std::cout << "################################\n";


    CTD_FB->SetField("CD", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 0);
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), true);

    std::cout << "################################\n";

    CTD_FB->SetField("CD", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 0);
    EXPECT_EQ(CTD_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), true);
    std::cout << "################################\n";
    

    new_executor.CleanUp();
    configuration.Cleanup();

}



TEST(ExecutionTestSuite, CTUD_Counter_Execution_Test) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/execution_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfigurationImpl configuration(TestDir + "/input.prototxt");

    PCResourceImpl * resource 
        = (PCResourceImpl*) configuration.RegisteredResources->GetResource(
                    "CPU_001");
    
    PCVariable * CTUD_FB = resource->GetPOU("CTUD");
    IntervalTaskSpecification task_spec;
    IntervalTaskParams*  task_params = new IntervalTaskParams;
    task_spec.set_task_name("Task1");
    task_spec.set_priority(1);
    task_params->set_interval_ms(10);
    task_spec.set_allocated_interval_task_params(task_params);
    Task new_task((PCConfigurationImpl*)&configuration, resource, task_spec);
    Executor new_executor(&configuration, resource, &new_task);

    new_executor.SetExecPoUVariable(CTUD_FB);

    CTUD_FB->SetField("R", "FALSE");
    CTUD_FB->SetField("LD", "FALSE");
    CTUD_FB->SetField("CD", "FALSE");
    CTUD_FB->SetField("PV", "5");
    CTUD_FB->SetField("CU", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 0);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QU",
        DataTypeCategory::BOOL), false);
    std::cout << "################################\n";
    
    
    CTUD_FB->SetField("CU", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 1);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QU",
        DataTypeCategory::BOOL), false);

    std::cout << "################################\n";

    
    CTUD_FB->SetField("CU", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 1);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QU",
        DataTypeCategory::BOOL), false);

    std::cout << "################################\n";

    CTUD_FB->SetField("CU", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 2);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QU",
        DataTypeCategory::BOOL), false);
    std::cout << "################################\n";


    CTUD_FB->SetField("CU", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 2);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QU",
        DataTypeCategory::BOOL), false);

    std::cout << "################################\n";

    CTUD_FB->SetField("CU", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 3);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QU",
        DataTypeCategory::BOOL), false);
    std::cout << "################################\n";

    CTUD_FB->SetField("CU", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 3);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QU",
        DataTypeCategory::BOOL), false);

    std::cout << "################################\n";

    CTUD_FB->SetField("CU", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 4);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QU",
        DataTypeCategory::BOOL), false);
    std::cout << "################################\n";

    CTUD_FB->SetField("CU", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 4);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QU",
        DataTypeCategory::BOOL), false);

    std::cout << "################################\n";

    CTUD_FB->SetField("CU", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 5);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QU",
        DataTypeCategory::BOOL), true);
    std::cout << "################################\n";


    CTUD_FB->SetField("CU", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 5);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QU",
        DataTypeCategory::BOOL), true);

    std::cout << "################################\n";

    CTUD_FB->SetField("CU", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 5);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QU",
        DataTypeCategory::BOOL), true);
    std::cout << "################################\n";
    
    // Now count down
    CTUD_FB->SetField("CD", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 5);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QD",
        DataTypeCategory::BOOL), false);
    std::cout << "################################\n";
    
    CTUD_FB->SetField("CD", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 4);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QD",
        DataTypeCategory::BOOL), false);

    std::cout << "################################\n";

    
    CTUD_FB->SetField("CD", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 4);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QD",
        DataTypeCategory::BOOL), false);

    std::cout << "################################\n";

    CTUD_FB->SetField("CD", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 3);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QD",
        DataTypeCategory::BOOL), false);
    std::cout << "################################\n";


    CTUD_FB->SetField("CD", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 3);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QD",
        DataTypeCategory::BOOL), false);

    std::cout << "################################\n";

    CTUD_FB->SetField("CD", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 2);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QD",
        DataTypeCategory::BOOL), false);
    std::cout << "################################\n";

    CTUD_FB->SetField("CD", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 2);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QD",
        DataTypeCategory::BOOL), false);

    std::cout << "################################\n";

    CTUD_FB->SetField("CD", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 1);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QD",
        DataTypeCategory::BOOL), false);
    std::cout << "################################\n";

    CTUD_FB->SetField("CD", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 1);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QD",
        DataTypeCategory::BOOL), false);

    std::cout << "################################\n";

    CTUD_FB->SetField("CD", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 0);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QD",
        DataTypeCategory::BOOL), true);
    std::cout << "################################\n";


    CTUD_FB->SetField("CD", "FALSE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 0);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QD",
        DataTypeCategory::BOOL), true);

    std::cout << "################################\n";

    CTUD_FB->SetField("CD", "TRUE");
    new_executor.Run();
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<int16_t>("CV",
        DataTypeCategory::INT), 0);
    EXPECT_EQ(CTUD_FB->GetValueStoredAtField<bool>("QD",
        DataTypeCategory::BOOL), true);
    std::cout << "################################\n";

    new_executor.CleanUp();
    configuration.Cleanup();

}



TEST(ExecutionTestSuite, TON_Timer_Execution_Test) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/execution_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfigurationImpl configuration(TestDir + "/input.prototxt");

    PCResourceImpl * resource 
        = (PCResourceImpl*) configuration.RegisteredResources->GetResource(
                    "CPU_001");
    
    
    PCVariable * TON_FB = resource->GetPOU("TON");
    IntervalTaskSpecification task_spec;
    IntervalTaskParams*  task_params = new IntervalTaskParams;
    task_spec.set_task_name("Task1");
    task_spec.set_priority(1);
    task_params->set_interval_ms(10);
    task_spec.set_allocated_interval_task_params(task_params);
    Task new_task((PCConfigurationImpl*)&configuration, resource, task_spec);
    Executor new_executor(&configuration, resource, &new_task);

    
    new_executor.SetExecPoUVariable(TON_FB);
    TON_FB->SetField("IN", "FALSE");
    TON_FB->SetField("PT", "t#1s");
    
    new_executor.Run();
    
    EXPECT_EQ(TON_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);
    EXPECT_EQ(TON_FB->GetValueStoredAtField<TimeType>("ET",
        DataTypeCategory::TIME).SecsElapsed, 0);

    TON_FB->SetField("IN", "TRUE");
    new_executor.Run();
    EXPECT_EQ(TON_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);
    EXPECT_EQ(TON_FB->GetValueStoredAtField<TimeType>("ET",
        DataTypeCategory::TIME).SecsElapsed, 0);

    std::this_thread::sleep_for(
        std::chrono::microseconds(100000));

    TON_FB->SetField("IN", "TRUE");
    new_executor.Run();
    EXPECT_EQ(TON_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);

    std::this_thread::sleep_for(
        std::chrono::microseconds(1000000));
    
    TON_FB->SetField("IN", "TRUE");
    new_executor.Run();
    EXPECT_EQ(TON_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), true);
    EXPECT_EQ(TON_FB->GetValueStoredAtField<TimeType>("ET",
        DataTypeCategory::TIME).SecsElapsed, 1.0);

    TON_FB->SetField("IN", "FALSE");
    new_executor.Run();
    EXPECT_EQ(TON_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);
    EXPECT_EQ(TON_FB->GetValueStoredAtField<TimeType>("ET",
        DataTypeCategory::TIME).SecsElapsed, 0);

    
    new_executor.CleanUp();
    configuration.Cleanup();

}


TEST(ExecutionTestSuite, TOF_Timer_Execution_Test) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/execution_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfigurationImpl configuration(TestDir + "/input.prototxt");

    PCResourceImpl * resource 
        = (PCResourceImpl*) configuration.RegisteredResources->GetResource(
                    "CPU_001");
    
    
    PCVariable * TOF_FB = resource->GetPOU("TOF");
    IntervalTaskSpecification task_spec;
    IntervalTaskParams*  task_params = new IntervalTaskParams;
    task_spec.set_task_name("Task1");
    task_spec.set_priority(1);
    task_params->set_interval_ms(10);
    task_spec.set_allocated_interval_task_params(task_params);
    Task new_task((PCConfigurationImpl*)&configuration, resource, task_spec);
    Executor new_executor(&configuration, resource, &new_task);

    
    new_executor.SetExecPoUVariable(TOF_FB);
    TOF_FB->SetField("IN", "TRUE");
    TOF_FB->SetField("PT", "t#1s");
    
    new_executor.Run();
    
    EXPECT_EQ(TOF_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);
    EXPECT_EQ(TOF_FB->GetValueStoredAtField<TimeType>("ET",
        DataTypeCategory::TIME).SecsElapsed, 0);

    TOF_FB->SetField("IN", "FALSE");
    new_executor.Run();
    EXPECT_EQ(TOF_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), true);
    EXPECT_EQ(TOF_FB->GetValueStoredAtField<TimeType>("ET",
        DataTypeCategory::TIME).SecsElapsed, 0);

    std::this_thread::sleep_for(
        std::chrono::microseconds(100000));

    TOF_FB->SetField("IN", "FALSE");
    new_executor.Run();
    EXPECT_EQ(TOF_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), true);

    std::this_thread::sleep_for(
        std::chrono::microseconds(1000000));
    
    TOF_FB->SetField("IN", "FALSE");
    new_executor.Run();
    EXPECT_EQ(TOF_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);
    EXPECT_EQ(TOF_FB->GetValueStoredAtField<TimeType>("ET",
        DataTypeCategory::TIME).SecsElapsed, 1.0);

    TOF_FB->SetField("IN", "TRUE");
    new_executor.Run();
    EXPECT_EQ(TOF_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);
    EXPECT_EQ(TOF_FB->GetValueStoredAtField<TimeType>("ET",
        DataTypeCategory::TIME).SecsElapsed, 0);

    
    new_executor.CleanUp();
    configuration.Cleanup();

}


TEST(ExecutionTestSuite, TP_Timer_Execution_Test) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/execution_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfigurationImpl configuration(TestDir + "/input.prototxt");

    PCResourceImpl * resource 
        = (PCResourceImpl*) configuration.RegisteredResources->GetResource(
                    "CPU_001");
    
    
    PCVariable * TP_FB = resource->GetPOU("TP");
    IntervalTaskSpecification task_spec;
    IntervalTaskParams*  task_params = new IntervalTaskParams;
    task_spec.set_task_name("Task1");
    task_spec.set_priority(1);
    task_params->set_interval_ms(10);
    task_spec.set_allocated_interval_task_params(task_params);
    Task new_task((PCConfigurationImpl*)&configuration, resource, task_spec);
    Executor new_executor(&configuration, resource, &new_task);

    
    new_executor.SetExecPoUVariable(TP_FB);

    TP_FB->SetField("PT", "t#1s");
    TP_FB->SetField("IN", "FALSE"); 
    new_executor.Run();
    
    EXPECT_EQ(TP_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);
    EXPECT_EQ(TP_FB->GetValueStoredAtField<TimeType>("ET",
        DataTypeCategory::TIME).SecsElapsed, 0);


    TP_FB->SetField("IN", "TRUE"); 
    new_executor.Run();
    
    EXPECT_EQ(TP_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), true);
    EXPECT_EQ(TP_FB->GetValueStoredAtField<TimeType>("ET",
        DataTypeCategory::TIME).SecsElapsed, 0);

    std::this_thread::sleep_for(
        std::chrono::microseconds(100000));

    TP_FB->SetField("IN", "FALSE"); 
    new_executor.Run();
    
    EXPECT_EQ(TP_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), true);

    std::this_thread::sleep_for(
        std::chrono::microseconds(100000));

    TP_FB->SetField("IN", "FALSE"); 
    new_executor.Run();
    
    EXPECT_EQ(TP_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), true);
    
    std::this_thread::sleep_for(
        std::chrono::microseconds(800000));

    TP_FB->SetField("IN", "FALSE"); 
    new_executor.Run();
    
    EXPECT_EQ(TP_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);
    EXPECT_EQ(TP_FB->GetValueStoredAtField<TimeType>("ET",
        DataTypeCategory::TIME).SecsElapsed, 0);

    TP_FB->SetField("IN", "TRUE"); 
    new_executor.Run();
    EXPECT_EQ(TP_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), true);
    EXPECT_EQ(TP_FB->GetValueStoredAtField<TimeType>("ET",
        DataTypeCategory::TIME).SecsElapsed, 0);

    std::this_thread::sleep_for(
        std::chrono::microseconds(1000000));

    TP_FB->SetField("IN", "TRUE"); 
    new_executor.Run();
    
    EXPECT_EQ(TP_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);
    EXPECT_EQ(TP_FB->GetValueStoredAtField<TimeType>("ET",
        DataTypeCategory::TIME).SecsElapsed, 1.0);

    TP_FB->SetField("IN", "FALSE"); 
    new_executor.Run();
    
    EXPECT_EQ(TP_FB->GetValueStoredAtField<bool>("Q",
        DataTypeCategory::BOOL), false);
    EXPECT_EQ(TP_FB->GetValueStoredAtField<TimeType>("ET",
        DataTypeCategory::TIME).SecsElapsed, 0);
    
    new_executor.CleanUp();
    configuration.Cleanup();

}


TEST(ExecutionTestSuite, Program_Execution_Test) {
    string TestDir = Utils::GetInstallationDirectory() 
            + "/src/pc_emulator/tests/execution_tests";

    std::cout << "Config File: " << TestDir + "/input.prototxt" << std::endl;
    PCConfigurationImpl configuration(TestDir + "/input.prototxt");

    PCResourceImpl * resource 
        = (PCResourceImpl*) configuration.RegisteredResources->GetResource(
                    "CPU_001");
    
    
    PCVariable * Program = resource->GetPOU("PROGRAM_1");
    IntervalTaskSpecification task_spec;
    IntervalTaskParams*  task_params = new IntervalTaskParams;
    task_spec.set_task_name("Task1");
    task_spec.set_priority(1);
    task_params->set_interval_ms(10);
    task_spec.set_allocated_interval_task_params(task_params);
    Task new_task((PCConfigurationImpl*)&configuration, resource, task_spec);
    Executor new_executor(&configuration, resource, &new_task);

    
    new_executor.SetExecPoUVariable(Program);

    Program->SetField("Dividend", "100");
    Program->SetField("Divisor", "10"); 
    new_executor.Run();
    
    EXPECT_EQ(Program->GetValueStoredAtField<int16_t>("Quotient",
        DataTypeCategory::INT), 10);
    EXPECT_EQ(Program->GetValueStoredAtField<int16_t>("DivRem",
        DataTypeCategory::INT), 0);
    EXPECT_EQ(Program->GetValueStoredAtField<int16_t>("FnCallCount",
        DataTypeCategory::INT), 1);
    EXPECT_EQ(Program->GetValueStoredAtField<bool>("DivError",
        DataTypeCategory::BOOL), false);

    Program->SetField("Dividend", "100");
    Program->SetField("Divisor", "11"); 
    new_executor.Run();
    
    EXPECT_EQ(Program->GetValueStoredAtField<int16_t>("Quotient",
        DataTypeCategory::INT), 9);
    EXPECT_EQ(Program->GetValueStoredAtField<int16_t>("DivRem",
        DataTypeCategory::INT), 1);
    EXPECT_EQ(Program->GetValueStoredAtField<int16_t>("FnCallCount",
        DataTypeCategory::INT), 2);
    EXPECT_EQ(Program->GetValueStoredAtField<bool>("DivError",
        DataTypeCategory::BOOL), false);

    Program->SetField("Dividend", "100");
    Program->SetField("Divisor", "0"); 
    new_executor.Run();
    EXPECT_EQ(Program->GetValueStoredAtField<int16_t>("FnCallCount",
        DataTypeCategory::INT), 3);
    EXPECT_EQ(Program->GetValueStoredAtField<bool>("DivError",
        DataTypeCategory::BOOL), true);
    
    new_executor.CleanUp();
    configuration.Cleanup();

}