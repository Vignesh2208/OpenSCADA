#ifndef __PC_EMULATOR_INCLUDE_PC_RESOURCE_REGISTRY_H__
#define __PC_EMULATOR_INCLUDE_PC_RESOURCE_REGISTRY_H__

#include <iostream>
#include <unordered_map>

#include "pc_resource.h"


using namespace std;


namespace pc_emulator {
    class PCResource;
    class PCConfiguration;

    class ResourceRegistry {
        private:
            PCConfiguration * __configuration;
            std::unordered_map<std::string, 
                            std::unique_ptr<PCResource>> __Registry;
        public:
            ResourceRegistry(PCConfiguration* configuration) : 
                    __configuration(configuration) {};
            void RegisterResource(string ResourceName,
                        std::unique_ptr<PCResource> Resource);
            PCResource * GetResource(string ResourceName);

            void Cleanup();
    };
}

#endif