#include "pc_emulator/include/pc_logger.h"
#include "pc_emulator/include/pc_configuration.h"

using namespace std;
using namespace pc_emulator;

void Logger::LogMessage(int LogLevel, std::string Message) {
    string Qualifier;
    
    if (LogLevel != LOG_LEVELS::LOG_NONE && LogLevel <= __LogLevel) {
        if (LogLevel == LOG_LEVELS::LOG_INFO) {
            Qualifier = "INFO";
        } else if (LogLevel == LOG_LEVELS::LOG_ERROR) {
            Qualifier = "ERROR";
        } else {
            Qualifier = "VERBOSE";
        }
        if (!__LogFile.empty()){
            __ofs << Qualifier << " >> " << Message << std::endl;
        } else {
            std::cout << Qualifier << " >> " << Message << std::endl;
        }
    }
}

void Logger::RaiseException(std::string Message) {
    if (!__LogFile.empty()){
        __ofs << " EXCEPTION >> " << Message << std::endl;
    } else {
        std::cout << " EXCEPTION >> " << Message << std::endl;
    }
    this->ShutDown();
}

void Logger::ShutDown() {
    LogMessage(LOG_LEVELS::LOG_INFO, "Shutting Down ....");
    __configuration->Cleanup();
    exit(0);
}