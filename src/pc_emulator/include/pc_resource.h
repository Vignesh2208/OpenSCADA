#ifndef __PC_EMULATOR_INCLUDE_PC_RESOURCE_H__
#define __PC_EMULATOR_INCLUDE_PC_RESOURCE_H__

#include <iostream>
#include <vector>
#include <climits>
#include <cstdlib>
#include <unordered_map>
#include <queue>
#include "pc_variable.h"
#include "pc_mem_unit.h"
#include "pc_pou_code_container.h"
#include "pc_clock.h"
#include "src/pc_emulator/proto/configuration.pb.h"

using namespace std;
using namespace pc_specification;
using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldInterfaceType = pc_specification::FieldInterfaceType;

namespace pc_emulator {
    class PCConfiguration;
    class Task;

    class CompactTaskDescription {
        public :
            string __TaskName;
            double __nxt_schedule_time_ms;

            CompactTaskDescription(string TaskName, double nx_schedule_time_ms)
                : __TaskName(TaskName), 
                __nxt_schedule_time_ms(nx_schedule_time_ms) {};

    };

    bool operator==(const CompactTaskDescription& a, const CompactTaskDescription& b);
    bool operator>(const CompactTaskDescription& a, const CompactTaskDescription& b);
    bool operator<(const CompactTaskDescription& a, const CompactTaskDescription& b);
    bool operator<=(const CompactTaskDescription& a, const CompactTaskDescription& b);
    bool operator>=(const CompactTaskDescription& a, const CompactTaskDescription& b);

    class PCResource {
        private:
            
            
            int __InputMemSize;
            int __OutputMemSize;
            PCMemUnit __InputMemory;
            PCMemUnit __OutputMemory;
            std::unordered_map<std::string,  PCVariable*> __ResourcePoUVars;
            std::unordered_map<std::string, PCVariable*> __AccessedFields;
            std::unordered_map<std::string, PoUCodeContainer*> __CodeContainers;
            std::unordered_map<std::string, Task *> __Tasks;
            std::unordered_map<int, priority_queue<CompactTaskDescription>> 
                                            __IntervalTasksByPriority;
            std::unordered_map<string, std::vector<Task*>> __InterruptTasks;
            
            

        public :
            string __ResourceName;
            PCConfiguration * __configuration;
            Clock *clock;
            PCResource(PCConfiguration * configuration, 
                string ResourceName, int InputMemSize, int OutputMemSize):
                __configuration(configuration), __ResourceName(ResourceName),
                __InputMemSize(InputMemSize), __OutputMemSize(OutputMemSize) {

                    assert(__InputMemSize > 0 && __OutputMemSize > 0);
                    __InputMemory.AllocateStaticMemory(__InputMemSize);
                    __OutputMemory.AllocateStaticMemory(__OutputMemSize);
                    
            }

            void InitializeClock() ;
            void InitializeAllPoUVars();
            void InitializeAllTasks();
            void OnStartup();


            void AddTask(Task * Tsk);
            Task * GetTask(string TskName);
            Task * GetInterruptTaskToExecute();
            Task * GetIntervalTaskToExecuteAt(double schedule_time);
            void QueueTask(Task * Tsk);
            
            void RegisterPoUVariable(string VariableName, PCVariable * Var);
            PCVariable * GetVariable(string NestedFieldName);
            PCVariable * GetPoUVariable(string PoUName);
            PCVariable * GetGlobalVariable(string NestedFieldName);
            PCVariable * GetVariablePointerToMem(int MemType, int ByteOffset,
                                int BitOffset, string VariableDataTypeName);
            
            PoUCodeContainer * CreateNewCodeContainer(string PoUDataTypeName);
            PoUCodeContainer * GetCodeContainer(string PoUDataTypeName);
            void Cleanup();
    };

}

#endif