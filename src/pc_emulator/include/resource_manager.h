#ifndef __PC_EMULATOR_INCLUDE_RESOURCE_MANAGER_H__
#define __PC_EMULATOR_INCLUDE_RESOURCE_MANAGER_H__

using namespace std;

#include <thread>
#include "pc_datatype.h"
#include "pc_variable.h"
#include "pc_resource.h"

namespace pc_emulator {
    class ResourceManager {
        public :
            PCResourceImpl * __AssociatedResource;

            ResourceManager(PCResourceImpl * AssociatedResource) : 
                    __AssociatedResource(AssociatedResource) {};

            void ExecuteResource();
            void ExecuteResourceManager();

            std::thread LaunchResource();
            std::thread LaunchResourceManager();

    };
}



#endif