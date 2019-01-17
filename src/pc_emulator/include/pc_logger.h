
#ifndef __PC_EMULATOR_INCLUDE_PC_LOGGER_H__
#define __PC_EMULATOR_INCLUDE_PC_LOGGER_H__
#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>

using namespace std;

namespace pc_emulator {
    class PCConfiguration;
    enum LOG_LEVELS {
        LOG_NONE,
        LOG_INFO,
        LOG_ERROR,
        LOG_VERBOSE
    };

    class Logger {
        private:
            PCConfiguration * __configuration;
            std::string __LogFile;
            int __LogLevel;
            std::ofstream __ofs;

        public:
            Logger(PCConfiguration * configuration, std::string LogFile,
                    int LogLevel) : __configuration(configuration), 
                                    __LogFile(LogFile), __LogLevel(LogLevel) {

                        if (!LogFile.empty())
                            __ofs.open(LogFile, std::ofstream::out 
                                            | std::ofstream::app );

                        assert (LogLevel == LOG_LEVELS::LOG_NONE
                                || LogLevel == LOG_LEVELS::LOG_ERROR
                                || LogLevel == LOG_LEVELS::LOG_INFO 
                                || LogLevel == LOG_LEVELS::LOG_VERBOSE);
                    };
            void LogMessage(int LogLevel, std::string Message);
            void RaiseException(std::string ErrorMessage);
            void ShutDown();
    };
}

#endif