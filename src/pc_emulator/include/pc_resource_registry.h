#ifndef __PC_EMULATOR_INCLUDE_PC_RESOURCE_REGISTRY_H__
#define __PC_EMULATOR_INCLUDE_PC_RESOURCE_REGISTRY_H__

#include <iostream>
#include <unordered_map>

#include "pc_resource.h"


using namespace std;


namespace pc_emulator {
    class PCResource;
    class PCConfiguration;

    //! Registers and tracks resources
    class ResourceRegistry {
        private:
            PCConfiguration * __configuration; /*!< Associated configuration */
            std::unordered_map<std::string, std::unique_ptr<PCResource>> 
                __Registry; /*!< A hashmap of resource name, resource obj */
        public:
            //! Constructor
            ResourceRegistry(PCConfiguration* configuration) : 
                    __configuration(configuration) {};

            //!Register new resource
            void RegisterResource(string ResourceName,
                        std::unique_ptr<PCResource> Resource);

            //!Retrieves resource with the specified resource name
            PCResource * GetResource(string ResourceName);

            //! Clean's up all registered resources
            void Cleanup();
    };
}

#endif