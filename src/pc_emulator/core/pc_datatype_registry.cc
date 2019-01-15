#include "pc_emulator/include/pc_datatype_registry.h"
#include "pc_emulator/include/pc_logger.h"
#include "pc_emulator/include/pc_configuration.h"
#include "pc_emulator/include/pc_datatype.h"

using namespace pc_emulator;
using namespace std;

void DataTypeRegistry::RegisterDataType(string DataTypeName,
                                        PCDataType* DataType) {
    assert (__configuration != nullptr);
    assert (__configuration->PCLogger != nullptr);
    Logger * PCLogger = __configuration->PCLogger;
    std::string LogMsg;

    std::unordered_map<std::string, PCDataType*>::const_iterator got = 
                        __Registry.find (DataTypeName);
    if (got != __Registry.end()) {
        LogMsg = "Data Type " + DataTypeName + " Already Defined !";
        PCLogger->RaiseException(LogMsg);
    } else {
        __Registry.insert(std::make_pair(DataTypeName,
                                        DataType));
        LogMsg =  "Registered New Data Type: " + DataTypeName;
        PCLogger->LogMessage(LOG_LEVELS::LOG_INFO, LogMsg);
    }
}

PCDataType * DataTypeRegistry::GetDataType(string DataTypeName) {
    assert (__configuration != nullptr);
    assert (__configuration->PCLogger != nullptr);
    Logger * PCLogger = __configuration->PCLogger;
    std::string LogMsg;

    std::unordered_map<std::string, PCDataType*>::const_iterator got = 
                        __Registry.find (DataTypeName);
    if (got == __Registry.end()) {
        LogMsg = "Data Type " + DataTypeName + " Not Found !";
        PCLogger->RaiseException(LogMsg);
    } else {
        return got->second;
    }
}