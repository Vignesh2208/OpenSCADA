#include <assert.h>
#include "pc_emulator/include/pc_resource_registry.h"
#include "pc_emulator/include/pc_logger.h"
#include "pc_emulator/include/pc_configuration.h"
#include "pc_emulator/include/pc_resource.h"

using namespace pc_emulator;
using namespace std;

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
        PCLogger->LogMessage(LOG_LEVELS::LOG_INFO, LogMsg);
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