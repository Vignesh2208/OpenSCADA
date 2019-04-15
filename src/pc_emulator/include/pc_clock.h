#ifndef __PC_EMULATOR_INCLUDE_PC_CLOCK_H__
#define __PC_EMULATOR_INCLUDE_PC_CLOCK_H__



#include <iostream>
#include <vector>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <ratio>
#include <chrono>

using namespace std;
using namespace std::chrono;

namespace pc_emulator {
    class PCResourceImpl;

    //! Clock associated with a resource
    class Clock {
        
        public:
            double __time;  /*!< Current time */
            bool  __is_virtual; /*!< Is clock virtual ? */
            double __expected_time;
            PCResourceImpl * __AssociatedResource;  /*!< Associated Resource */

            //!Constructor
            Clock(bool is_virtual, PCResourceImpl * AssociatedResource);

            //! Returns current time
            double GetCurrentTime() {
                if (__is_virtual)
                    return __time;
                
                __time  = (duration_cast<milliseconds >(
                                system_clock::now()
                                .time_since_epoch())).count()/1000.0;
                return __time;
            }

            //!Increments current time by the specifiede amount
            void UpdateCurrentTime(double inc_amount) ;

            //! Sleep for specified duration in microseconds
            void SleepFor(int sleep_duration_us) ;
    };
}
#endif