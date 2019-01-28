#include <assert.h>
#include "src/pc_emulator/include/pc_resource_registry.h"
#include "src/pc_emulator/include/pc_logger.h"
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_resource.h"

using namespace pc_emulator;
using namespace std;
using namespace pc_specification;

using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldIntfType = pc_specification::FieldInterfaceType;
using LogLevels = pc_specification::LogLevels;


void ResourceRegistry::RegisterResource(string ResourceName,
                                        PCResource* Resource) {
    assert (__configuration != nullptr);
    assert (__configuration->PCLogger != nullptr);
    Logger * PCLogger = __configuration->PCLogger;
    std::string LogMsg;

    std::unordered_map<std::string, PCResource*>::const_iterator got = 
                        __Registry.find (ResourceName);
    if (got != __Registry.end()) {
        LogMsg = "Resource " + ResourceName + " Already Defined !";
        PCLogger->RaiseException(LogMsg);
    } else {
        __Registry.insert(std::make_pair(ResourceName,
                                        Resource));
        LogMsg =  "Registered New Resource: " + ResourceName;
        PCLogger->LogMessage(LogLevels::LOG_INFO, LogMsg);
    }
}

PCResource * ResourceRegistry::GetResource(string ResourceName) {
    assert (__configuration != nullptr);
    assert (__configuration->PCLogger != nullptr);
    Logger * PCLogger = __configuration->PCLogger;
    std::string LogMsg;

    std::unordered_map<std::string, PCResource*>::const_iterator got = 
                        __Registry.find (ResourceName);
    if (got == __Registry.end()) {
        return nullptr;
    } else {
        return got->second;
    }
}

void ResourceRegistry::Cleanup() {
    for ( auto it = __Registry.begin(); it != __Registry.end(); 
            ++it ) {
            PCResource * __AccessedResource = it->second;
            __AccessedResource->Cleanup();
            delete __AccessedResource;
    }
}