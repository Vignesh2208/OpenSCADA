#ifndef __PC_EMULATOR_INCLUDE_RESOURCE_MANAGER_H__
#define __PC_EMULATOR_INCLUDE_RESOURCE_MANAGER_H__

using namespace std;

#include <thread>
#include "pc_datatype.h"
#include "pc_variable.h"
#include "pc_resource.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#endif
#include <unistd.h>       
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)

namespace pc_emulator {
    class ResourceManager {
        public :
            PCResourceImpl * __AssociatedResource;

            std::mutex m;
            pid_t ResourceThreadPid;
            void set_tid() {
                std::lock_guard<std::mutex> l(m);
                ResourceThreadPid = syscall(SYS_gettid);
            }

            pid_t get_tid() {
                while(true) {
                    std::lock_guard<std::mutex> l(m);
                    if(ResourceThreadPid != 0)
                        break;
                }
                return ResourceThreadPid;
            }

            ResourceManager(PCResourceImpl * AssociatedResource) : 
                    __AssociatedResource(AssociatedResource) {
                       ResourceThreadPid = 0; 
                    };

            void ExecuteResource();
            void ExecuteResourceManager();

            std::thread LaunchResource();
            std::thread LaunchResourceManager();

    };
}



#endif