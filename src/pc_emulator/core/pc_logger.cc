#include "src/pc_emulator/include/pc_logger.h"
#include "src/pc_emulator/include/pc_configuration.h"

using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldIntfType = pc_specification::FieldInterfaceType;
using LogLevels = pc_specification::LogLevels;


void Logger::LogMessage(int LogLevel, std::string Message) {
    string Qualifier = "";
    
    if (LogLevel != LogLevels::LOG_NONE && LogLevel <= __LogLevel) {
        if (LogLevel == LogLevels::LOG_INFO) {
            Qualifier = "INFO";
        } else if (LogLevel == LogLevels::LOG_ERROR) {
            Qualifier = "ERROR";
        } else {
            Qualifier = "VERBOSE";
        }

        if (!__LogFile.empty()){
            __ofs << Qualifier << " >> " << Message << std::endl;
        } else {
            
        }
    }

    std::cout << Qualifier << " >> " << Message << std::endl;
}

void Logger::RaiseException(std::string Message) {
    if (!__LogFile.empty()){
        __ofs << " EXCEPTION >> " << Message << std::endl;
    } else {
        std::cout << " EXCEPTION >> " << Message << std::endl;
    }
    this->ShutDown();
}

// this will kill one of the resource threads
void Logger::ShutDown() {
    LogMessage(LogLevels::LOG_INFO, "Shutting Down Resource Thread ....");
    exit(0);
}