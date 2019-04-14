
#include <iostream>
#include <vector>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <ratio>
#include <chrono>
#include <thread>

#include "src/pc_emulator/include/pc_clock.h"
#include "src/pc_emulator/include/pc_resource.h"
#include "src/pc_emulator/include/task.h"

using namespace std;
using namespace std::chrono;
using namespace pc_emulator;



Clock::Clock(bool is_virtual, PCResourceImpl * AssociatedResource) 
    : __is_virtual(is_virtual), 
    __AssociatedResource(AssociatedResource) {
        if (__is_virtual) {
            // register with kronos here

            __time = 0.0;
        }
};

void Clock::UpdateCurrentTime(double inc_amount) {
    if (__is_virtual && inc_amount > 0.0) {
        __time += inc_amount;
        if (__time > __expected_time) {
        // optionally perform app-vt calls with kronos here to block on round completion
        // reset expected time also.
        }
    }
}

void Clock::SleepFor(int sleep_duration_us) {
    if (!__is_virtual) {
        std::this_thread::sleep_for(
            std::chrono::microseconds(sleep_duration_us));
    }
}