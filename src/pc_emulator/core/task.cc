#include <iostream>
#include <cstdint>
#include <cstring>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>

#include "src/pc_emulator/include/pc_datatype.h"
#include "src/pc_emulator/include/pc_variable.h"
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_resource.h"
#include "src/pc_emulator/include/executor.h"
#include "src/pc_emulator/include/task.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldIntfType = pc_specification::FieldInterfaceType;


ProgramContainer::ProgramContainer(PCResource * AssociatedResource,
    const ProgramSpecification& program_spec, Task * AssociatedTask) {
    __ProgramName = program_spec.program_name();
    __AssociatedResource = AssociatedResource;
    assert(__AssociatedResource != nullptr);
    __ExecPoUVariable = __AssociatedResource->GetPoUVariable(
                                program_spec.pou_variable_type());
    assert(__ExecPoUVariable != nullptr);
    for (auto initialization_map : program_spec.initialization_maps()) {
        __initialization_map.push_back(initialization_map);
    }
    __AssociatedTask = __AssociatedTask;
    __AssociatedExecutor = std::unique_ptr<Executor>(
        new Executor(__AssociatedResource->__configuration,
                    __AssociatedResource, AssociatedTask)); 

    __AssociatedExecutor->SetExecPoUVariable(__ExecPoUVariable);   
}

void ProgramContainer::Cleanup() {
}

void Task::AddProgramToTask(const ProgramSpecification& program_spec) {
    auto ProgContainer 
            = std::unique_ptr<ProgramContainer>(new ProgramContainer(
                __AssociatedResource, program_spec, this));
    
    __AssociatedPrograms.push_back(std::move(ProgContainer));
}

void Task::Execute() {

    std::vector<int> output_vars;
    for (int i = 0; i < (int)__AssociatedPrograms.size(); i++) {
        ProgramContainer * container = __AssociatedPrograms[i].get();
        output_vars.clear();
        for (auto map : container->__initialization_map) {
            DataTypeFieldAttributes Attributes;
            container->__ExecPoUVariable->GetFieldAttributes(
                    map.pou_variable_field_name(), Attributes);

            
            
            if (Attributes.FieldInterfaceType 
                        == FieldIntfType::VAR_INPUT) {

                // Copy to mapped input variables
                PCVariable * mappedVariable 
                = __configuration->GetVariable(map.mapped_variable_field_name());

                if (mappedVariable == nullptr) {
                    mappedVariable 
                    = __AssociatedResource->GetPOUGlobalVariable(
                        map.mapped_variable_field_name());
                    if(!mappedVariable) {
                        __configuration->PCLogger->RaiseException(
                            "Mapped variable"
                            + map.mapped_variable_field_name() + " not found!");
                    }
                }
                
                container->__ExecPoUVariable->SetField(
                                map.pou_variable_field_name(), mappedVariable);
                output_vars.push_back(0);
                
            } else if (Attributes.FieldInterfaceType 
                        == FieldIntfType::VAR_OUTPUT) {
                output_vars.push_back(1);

            } else if (Attributes.FieldInterfaceType 
                        == FieldIntfType::VAR_IN_OUT) {

                // Set pointers to some inout variables
                PCVariable * mappedVariable 
                = __configuration->GetVariable(map.mapped_variable_field_name());

                if (mappedVariable == nullptr) {
                    mappedVariable 
                    = __AssociatedResource->GetPOUGlobalVariable(
                        map.mapped_variable_field_name());
                    if(!mappedVariable) {
                        __configuration->PCLogger->RaiseException(
                            "Mapped variable"
                            + map.mapped_variable_field_name() + " not found!");
                    }
                }
                
                container->__ExecPoUVariable->SetField(
                        map.pou_variable_field_name(), mappedVariable);
                output_vars.push_back(0);
            } else {
                __configuration->PCLogger->RaiseException(
                        "Cannot map to a field which is not IN, OUT or INOUT");
            }
        } 

        container->__AssociatedExecutor->Run();

        //Gather output from some output variables and set it to mapped ones
        assert (output_vars.size() == container->__initialization_map.size());
        for (int i = 0; i < (int)container->__initialization_map.size(); i++) {

            if (output_vars[i] == 1) {
                auto map = container->__initialization_map[i];
                PCVariable * mappedVariable 
                = __configuration->GetVariable(map.mapped_variable_field_name());

                if (mappedVariable == nullptr) {
                    mappedVariable 
                    = __AssociatedResource->GetPOUGlobalVariable(
                        map.mapped_variable_field_name());
                    if(!mappedVariable) {
                        __configuration->PCLogger->RaiseException(
                            "Mapped variable"
                            + map.mapped_variable_field_name() + " not found!");
                    }
                }

                PCVariable * ptrToOutputField 
                    = container->__ExecPoUVariable->GetPtrToField(
                            map.pou_variable_field_name());
                mappedVariable->SetField("", ptrToOutputField);
            }
        }

    }

    if (type == TaskType::INTERRUPT)
        __IsReady = false;
}

void Task::Cleanup() {
    for (int i = 0; i <  (int)__AssociatedPrograms.size(); i++) {
            __AssociatedPrograms[i]->Cleanup();
    }
    __AssociatedPrograms.clear();
}


