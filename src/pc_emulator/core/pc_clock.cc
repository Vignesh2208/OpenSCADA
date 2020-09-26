
#include <iostream>
#include <vector>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <ratio>
#include <chrono>
#include <thread>
#include <unistd.h>

#include "src/pc_emulator/include/pc_clock.h"
#include "src/pc_emulator/include/pc_logger.h"
#include "src/pc_emulator/include/pc_resource.h"
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/task.h"

using namespace std;
using namespace std::chrono;
using namespace pc_emulator;



Clock::Clock(bool is_virtual, PCResourceImpl * AssociatedResource) 
    : __is_virtual(is_virtual), 
    __AssociatedResource(AssociatedResource) {
    if (__is_virtual) {
        __time = 0.0;
    }

    if (AssociatedResource->__configuration
        ->__specification.machine_spec().has_random_number_seed()) {
        generator.seed(AssociatedResource->__configuration
                        ->__specification.machine_spec().random_number_seed());
    } else {
        generator.seed(1234);
    }

    __stop = false;
};


void Clock::UpdateCurrentTime(string ExecutedFn) {
    double inc_amount;

    auto got = __AssociatedResource->__ExecTimes.find(ExecutedFn);

    if (got == __AssociatedResource->__ExecTimes.end()) {
        
        UpdateCurrentTime(1000.0);
        __AssociatedResource->__configuration->PCLogger->LogMessage(
            LogLevels::LOG_INFO,
            "Executed: " + ExecutedFn + ". Using default execution time of 1us. "
            "Curr Time: "
            + std::to_string(GetCurrentTime()));
        return;
    }

    auto rn_dist = got->second.get();
    inc_amount = (*rn_dist)(generator);
    inc_amount = (int) inc_amount;
    
    if (inc_amount < 0.0)
        inc_amount = 1000.0;
    UpdateCurrentTime(inc_amount);

    if (__is_virtual) {
        __AssociatedResource->__configuration->PCLogger->LogMessage(
                    LogLevels::LOG_INFO,
                    "Executed: " + ExecutedFn + ". Using an increment of " 
                    + std::to_string(inc_amount) + "(ns) Curr Time: "
                    + std::to_string(GetCurrentTime()));
    }
};

void Clock::UpdateCurrentTime(double inc_amount_ns) {

     
    if (__is_virtual && inc_amount_ns > 0.0) {
        __time += inc_amount_ns;
        if (__time >= __expected_time) {
            // optionally perform app-vt calls with kronos here to block on round completion
            // reset expected time also.
            __time = __expected_time;
            
            __AssociatedResource->ToResourceManager.push(
                    std::to_string(__time - __expected_time));
            auto cmd =  __AssociatedResource->FromResourceManager.pop();
            if (cmd == "STOP") {
                __AssociatedResource->__configuration->PCLogger->LogMessage(
                    LogLevels::LOG_INFO,
                    "Shutting down Resource thread: " 
                    + __AssociatedResource->__ResourceName);
               
                __stop = true;
                return;
            }
            __expected_time = std::stod(cmd);
        }
    }
}

void Clock::SleepFor(int sleep_duration_us) {
    if (!__is_virtual) {
        std::this_thread::sleep_for(
            std::chrono::microseconds(sleep_duration_us));
    } else {
        __time = __expected_time;
        __AssociatedResource->ToResourceManager.push("WAIT_FOR_IO");
        std::this_thread::sleep_for(
            std::chrono::microseconds(sleep_duration_us));
        __AssociatedResource->ToResourceManager.push("FINISHED_IO");
        auto cmd = __AssociatedResource->FromResourceManager.pop();
        if (cmd == "STOP") {
            __AssociatedResource->__configuration ->PCLogger->LogMessage(
                LogLevels::LOG_INFO,
                "Shutting down Resource thread: " 
                + __AssociatedResource->__ResourceName);
            
            __stop = true;
            return;
        }
        __expected_time = std::stod(cmd);
    }
}
