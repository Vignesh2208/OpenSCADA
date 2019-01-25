
#ifndef __PC_EMULATOR_INCLUDE_PC_LOGGER_H__
#define __PC_EMULATOR_INCLUDE_PC_LOGGER_H__
#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>

#include "pc_emulator/proto/configuration.pb.h"

using namespace std;
using namespace pc_specification;

namespace pc_emulator {
    class PCConfiguration;

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

                        assert (LogLevel == LogLevels::LOG_NONE
                                || LogLevel == LogLevels::LOG_ERROR
                                || LogLevel == LogLevels::LOG_INFO 
                                || LogLevel == LogLevels::LOG_VERBOSE);
                    };
            void LogMessage(int LogLevel, std::string Message);
            void RaiseException(std::string ErrorMessage);
            void ShutDown();
    };
}

#endif