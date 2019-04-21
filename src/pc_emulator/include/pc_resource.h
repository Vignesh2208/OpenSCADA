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
#include "configuration.h"
#include "resource.h"
#include "task.h"
#include "src/pc_emulator/proto/configuration.pb.h"

using namespace std;
using namespace pc_specification;
using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldInterfaceType = pc_specification::FieldInterfaceType;

namespace pc_emulator {
    class PCConfigurationImpl;
    class Task;
    class InsnRegistry;
    class SFCRegistry;
    class SFBRegistry;

    //! Compact description of a task associated with a resource
    class CompactTaskDescription {
        public :
            string __TaskName; /*!< Name of the task */
            double __nxt_schedule_time_ms;  /*!< Time at which it should be scheduled */

            //! Constructor
            CompactTaskDescription(string TaskName, double nx_schedule_time_ms)
                : __TaskName(TaskName), 
                __nxt_schedule_time_ms(nx_schedule_time_ms) {};

        //! Checks if two tasks have to be scheduled at the same time
        friend bool operator==(const CompactTaskDescription& b,
                            const CompactTaskDescription& a) {
            return b.__nxt_schedule_time_ms == a.__nxt_schedule_time_ms;
        }

        //! Checks if schedule time of Task1 > Task2
        friend bool operator>(const CompactTaskDescription& b,
                            const CompactTaskDescription& a) {
            return b.__nxt_schedule_time_ms > a.__nxt_schedule_time_ms;
        }
        //! Checks if schedule time of Task1 < Task2
        friend bool operator<(const CompactTaskDescription& b, 
                            const CompactTaskDescription& a) {
            return b.__nxt_schedule_time_ms < a.__nxt_schedule_time_ms;
        }
        //! Checks if schedule time of Task1 <= Task2
        friend bool operator<=(const CompactTaskDescription& b,
                            const CompactTaskDescription& a) {
            return b.__nxt_schedule_time_ms <= a.__nxt_schedule_time_ms;
        }

        //! Checks if schedule time of Task1 >= Task2
        friend bool operator>=(const CompactTaskDescription& b,
                            const CompactTaskDescription& a) {
            return b.__nxt_schedule_time_ms >= a.__nxt_schedule_time_ms;
        }

    };

    //! Specific implementation of the PCResource class. It defines one CPU on a PLC.
    class PCResourceImpl: public PCResource {
        private:

            
            std::unordered_map<std::string, std::unique_ptr<PoUCodeContainer>> 
                __CodeContainers; /*!< Holds the code body for each POU */
            std::unordered_map<std::string, std::unique_ptr<Task>> 
                __Tasks; /*!< Holds each task associated with this resource keyed by name */
            Task * __IntervalTask; /*!< The single interval task associated
                with this resource */
            std::unordered_map<string, std::vector<Task*>> 
                __InterruptTasks; /*!< Holds all interrupt tasks associated with this resource*/
                   
            //! Gets a global variable or a directly represented defined in a POU
            /*!
                \param NestedFieldName: May point to a subfield of a global variable or 
                    a directly represented variable defined inside one of the POUs
                    associated with this resource
                \return A variable pointing to this sub field
            */
            PCVariable * GetPOUGlobalVariable(string NestedFieldName);

            //! Registers a POU for internal book keeping
            void RegisterPoUVariable(string VariableName,
                                    std::unique_ptr<PCVariable> Var);

        public :
            PCConfigurationImpl * __configuration; /*!< Associated configuration */
            std::unique_ptr<Clock> clock;   /*!< Assocaited clock */
            PCVariable * __CurrentResult;   /*!< CR register for the resource */
            InsnRegistry *  __InsnRegistry; /*!< Stores all registered instructions */
            SFCRegistry * __SFCRegistry; /*!< Stores all registered SFCs */
            SFBRegistry * __SFBRegistry; /*!< Stores executors for all SFBs without
                Code body */
            
            //!Constructor
            /*!
                \param configuration    Associated configuration object
                \param ResourceName Name of the resource
                \param InputMemSize Input memory size in bytes
                \param OutputMemSize    Output memory size in bytes
            */
            PCResourceImpl(PCConfigurationImpl * configuration,
                string ResourceName, int InputMemSize, int OutputMemSize);

            //! Initializes clock associated with the resource.
            void InitializeClock() ;

            //! Register and initialize all POU variables defined within the resource
            void InitializeAllPoUVars();

            void InitializeAllSFBVars();

            //! Resolve external fields of all POU variables
            void ResolveAllExternalFields();

            //! Initialize all Tasks associated with the resource
            void InitializeAllTasks();

            //! Called on resource start-up
            void OnStartup();

            //! Adds a new task for internal book-keeping
            void AddTask(std::unique_ptr<Task> Tsk);

            //! Returns a task with the specified task name
            Task * GetTask(string TskName);

            //! Get any active interrupt tasks to execute
            Task * GetInterruptTaskToExecute();

            //! Returns the interval task to execute at the specified time
            Task * GetIntervalTaskToExecuteAt(double schedule_time);

            //! Executes an instruction with the specified instruction name
            /*!
                \param InsnName Name of the instruction
                \param  Ops  List of operands to the instruction. The __CurrentResult
                            is always a default operand.
            */
            void ExecuteInsn(string InsnName, std::vector<PCVariable*>& Ops);

            //! Queues a task
            void QueueTask(Task* Tsk);   

            //! Returns a variable pointing to the subfield of a global/directly rep field
            PCVariable * GetExternVariable(string NestedFieldName);

            //! Returns a POU defined in the resource with the specified name
            /*!
                \param PoUName  Name of the POU defined in the resource
                \return POU variable or nullptr if POU is not found 
            */
            PCVariable * GetPOU(string PoUName);

            //! Given location details, returns a PCVariable pointing to that location
            /*!
                \param MemType  Memory Type (INPUT or OUTPUT or RAM given in pc_specification::MemTypes)
                \param ByteOffset Byte offset within the specified memory
                \param BitOffset BitOffset within the specified memory. If 
                                 VariableDataTypeName is not BOOL, it is ignored
                \param VariableDataTypeName Initializes a variable of this datatype
                                            on the specified memory location
                \return The initialized variable is returned.
            */
            PCVariable * GetVariablePointerToMem(int MemType, int ByteOffset,
                                int BitOffset, string VariableDataTypeName);

            //! Creates a temporary variable of the specified data type and initializes it
            /*!
                The tmp variable is initialized with the specified InitialValue
            */
            PCVariable * GetTmpVariable(string VariableDataTypeName,
                                        string InitialValue);

            //! Creates a temporary variable of the data type encoded in the operand value
            /*!
                The tmp variable is initialized with the value specified in the operand
            */
            PCVariable * GetVariableForImmediateOperand(string OperandValue);

            //! Creates a new code container for a POU and registers it
            PoUCodeContainer * CreateNewCodeContainer(PCDataType* PoUDataType);

            //! Returns a code container for a specific POU or nullptr if not found
            PoUCodeContainer * GetCodeContainer(string PoUDataTypeName);

            //! Cleans up allocated memory and temporay variables
            void Cleanup();
    };

}

#endif