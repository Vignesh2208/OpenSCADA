#include <iostream>
#include <cstdint>
#include <cstring>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>


#include "src/pc_emulator/include/kronos_api.h"
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_resource.h"
#include "src/pc_emulator/include/executor.h"
#include "src/pc_emulator/include/task.h"
#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/include/pc_clock.h"
#include "src/pc_emulator/include/resource_manager.h"

using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldIntfType = pc_specification::FieldInterfaceType;


// Resource Must have been startup before this call
void ResourceManager::ExecuteResource() {

    __AssociatedResource->OnStartup();
    long run_time_secs 
    = __AssociatedResource->__configuration->__specification.run_time_secs();
    bool is_virtual = __AssociatedResource->clock->__is_virtual;

    auto start_time = __AssociatedResource->clock->GetCurrentTime();

    __AssociatedResource->__configuration->PCLogger->LogMessage(
                LogLevels::LOG_NOTICE, "STARTED Resource: " +
             __AssociatedResource->__ResourceName );

    if (is_virtual) {
        set_tid();
        auto cmd = __AssociatedResource->FromResourceManager.pop();
        if (cmd == "STOP") {
            __AssociatedResource->__configuration->PCLogger->LogMessage(
                LogLevels::LOG_NOTICE, "STOPPING Resource: " +
             __AssociatedResource->__ResourceName );
            return;
        }

        __AssociatedResource->clock->__expected_time = std::stod(cmd);
        std::cout << "Starting expected time: " << std::stod(cmd) << std::endl;
    }
    while (true) {

        if (__AssociatedResource->clock->__stop) {
            break;
        }

        auto curr_time = __AssociatedResource->clock->GetCurrentTime();
        Task * NxtTask = __AssociatedResource->GetInterruptTaskToExecute();
        if (!NxtTask) {
            NxtTask = 
                __AssociatedResource->GetIntervalTaskToExecuteAt(
                    curr_time*1000.0);
            if (!NxtTask) {
                __AssociatedResource->clock->SleepFor(US_IN_MS);
            } else {
                __AssociatedResource->__configuration->PCLogger->LogMessage(
                LogLevels::LOG_NOTICE,
                std::to_string(curr_time) +
                " >> Resource: " 
                + __AssociatedResource->__ResourceName +  " Executing Task: "
                + NxtTask->__TaskName);
                assert (NxtTask->type == TaskType::INTERVAL);
                NxtTask->Execute();
                NxtTask->SetNextScheduleTime(curr_time*1000.0 
                    + NxtTask->__interval_ms);
            }
        } else {
            __AssociatedResource->__configuration->PCLogger->LogMessage(
                LogLevels::LOG_NOTICE,
                std::to_string(curr_time) +
                " >> Resource: " 
                + __AssociatedResource->__ResourceName +  " Executing Task: "
                + NxtTask->__TaskName);
            assert (NxtTask->type == TaskType::INTERRUPT);
            NxtTask->Execute();
        }

        if (!is_virtual && (curr_time - start_time > run_time_secs)) {
            __AssociatedResource->__configuration->PCLogger->LogMessage(
                LogLevels::LOG_NOTICE, "STOPPING Resource: " +
             __AssociatedResource->__ResourceName );
            break;

        }

        if (!is_virtual && __AssociatedResource->__configuration->stop) {
            __AssociatedResource->__configuration->PCLogger->LogMessage(
                LogLevels::LOG_NOTICE, "STOPPING Resource: " +
             __AssociatedResource->__ResourceName );
            break;
        }

    }

}


std::thread ResourceManager::LaunchResource() {
    return std::thread( [this] { this->ExecuteResource(); } );
}

std::thread ResourceManager::LaunchResourceManager() {
    return std::thread( [this] { this->ExecuteResourceManager(); } );
}


void ResourceManager::ExecuteResourceManager() {
    
    bool is_virtual;
    string nxt_command = "0";
    if (__AssociatedResource->__configuration->enable_kronos) {
        is_virtual = true;
    } else {
        is_virtual = false;
    }

    if (is_virtual) {
        // register with kronos here as a tracer
        // launch execute resource function in a separate thread
        __AssociatedResource->__configuration->PCLogger->LogMessage(
                LogLevels::LOG_NOTICE, "STARTING Resource: " +
             __AssociatedResource->__ResourceName );

        auto ResourceThread = LaunchResource();


        auto per_round_advance_ns 
            = (float)__AssociatedResource->__configuration->per_round_inc_ns;
        
        
        auto nxt_round_inc = per_round_advance_ns;
        auto curr_time = 0.0;
        long run_time_secs 
            = __AssociatedResource->__configuration
                ->__specification.run_time_secs();

        pid_t thread_pid = get_tid();
        std::cout << "####### Thread Pid = " 
            << thread_pid << " My Pid = " << gettid() << std::endl;
        
        
        std::cout << "Registering Tracer for Resource: " 
            << __AssociatedResource->__ResourceName 
            << std::endl;

        addToExp_child(1.0, 
            (int)per_round_advance_ns, thread_pid);
        GetNxtCommand(nxt_command);
        while (true) {
            // get each round params from kronos, send it to Resource thread
            // through some shared queue and wait until completion of round.

            // if stop command is received from kronos, send exit command to
            // resource thread and break;
            if (nxt_command == "STOP") {
                break;
            }
            
            __AssociatedResource->FromResourceManager.push(std::to_string(
                            curr_time + nxt_round_inc));
            auto recv = __AssociatedResource->ToResourceManager.pop();
	    

            if (recv == "WAIT_FOR_IO") {
                curr_time = __AssociatedResource->clock->__time;
                while (recv != "FINISHED_IO") {
                    
                    nxt_command = "0";
                    GetNxtCommand(nxt_command);
                    if (nxt_command == "STOP") {
                        break;
                    }
                    if (!__AssociatedResource->ToResourceManager.IsEmpty()) {
                        recv = __AssociatedResource->ToResourceManager.pop();
                    } else {
                        __AssociatedResource->clock->__time 
                            += per_round_advance_ns;
                    }
                    curr_time = __AssociatedResource->clock->__time;
                    
                }
                
            } else {
                // recv contains overshoot error

                if (recv == "0") {
                    nxt_round_inc = per_round_advance_ns;
                    
                } else {
                    nxt_round_inc = per_round_advance_ns - std::stod(recv);
                }
                nxt_command = recv;
                GetNxtCommand(nxt_command);
                if (nxt_command == "STOP") {
                    break;
                }
                curr_time = __AssociatedResource->clock->__time;
            }
            
        }
        std::cout << "Stopping Resource: " << 
                __AssociatedResource->__ResourceName << std::endl;
        __AssociatedResource->FromResourceManager.push("STOP");
        std::cout << "Waiting for resrouce thread to finish " << std::endl;
        ResourceThread.join();
    } else {
        this->ExecuteResource();
    }
}
