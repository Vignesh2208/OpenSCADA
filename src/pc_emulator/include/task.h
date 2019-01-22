#ifndef __PC_EMULATOR_INCLUDE_TASK_H__
#define __PC_EMULATOR_INCLUDE_TASK_H__

#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <unordered_map>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "pc_logger.h"
#include "pc_mem_unit.h"
#include "pc_variable.h"
#include "executor.h"


#include "pc_emulator/proto/configuration.pb.h"

using namespace std;
using namespace pc_specification;

namespace pc_emulator {

    class PCConfiguration;
    class PCVariable;
    class PCResource;
    class Task;
    


    class ProgramContainer {
        public:
            string __ProgramName;
            PCResource * __AssociatedResource;
            PCVariable * __ExecPoUVariable;
            Executor * __AssociatedExecutor;
            std::vector<ProgramVariableInitialization> __initialization_map;

            ProgramContainer(PCResource * AssociatedResource, 
                const ProgramSpecification& program_spec);

            void Cleanup();
    };

    class Task {
        public :
            string __TaskName;
            int __priority;
            TaskType type;
            int __interval_ms;
            PCConfiguration * __configuration;
            PCResource * __AssociatedResource;
            string __trigger_variable_name;
            float __nxt_schedule_time_us;
            std::vector<ProgramContainer*> __AssociatedPrograms;

            Task(PCConfiguration * configuration,
                PCResource * AssociatedResource,
                const TaskSpecification& task_spec):
                         __configuration(configuration),
                         __AssociatedResource(AssociatedResource) {
                __TaskName = task_spec.task_name();
                __priority = task_spec.priority();
                type = task_spec.type();

                if (task_spec.has_interval_task_params()) {
                    assert(type == TaskType::INTERVAL);
                    __interval_ms 
                            = task_spec.interval_task_params().interval_ms();
                } else if (task_spec.has_interrupt_task_params()) {
                    assert(type == TaskType::INTERRUPT);
                    __trigger_variable_name 
                        = task_spec.interrupt_task_params()
                                    .trigger_variable_field();
                }

                __nxt_schedule_time_us = 0;
            };

            void SetNextScheduleTime(float nxt_schedule_time_us) {
                __nxt_schedule_time_us = nxt_schedule_time_us;
            };

            void AddProgramToTask(const ProgramSpecification& program_spec);

            void Execute();

            void Cleanup();

    };
}

#endif