
#ifndef __PC_EMULATOR_INCLUDE_PC_LOGGER_H__
#define __PC_EMULATOR_INCLUDE_PC_LOGGER_H__
#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include "src/pc_emulator/proto/configuration.pb.h"
using namespace std;
using namespace pc_specification;

using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldInterfaceType = pc_specification::FieldInterfaceType;


namespace pc_emulator {
    class PCConfiguration;

    //!Class which can log information to stdout or to a file
    class Logger {
        private:
            PCConfiguration * __configuration; /*!< Associated Configuration */
            std::string __LogFile;  /*!< Name of the log file */
            int __LogLevel; /*!< Level of logging. One of pc_specfication::LogLevels */
            std::ofstream __ofs;    /*!< To help write to the log file */

        public:

            //!Constructor
            /*!
                \param configuration Associated configuration object
                \param LogFile Full path to file. If empty, STDOUT is chosen
                \param LogLevel: One of pc_specification::LogLevels 
            */
            Logger(PCConfiguration * configuration, std::string LogFile,
                    int LogLevel) : __configuration(configuration), 
                                    __LogFile(LogFile), __LogLevel(LogLevel) {

                        if (!LogFile.empty())
                            __ofs.open(LogFile, std::ofstream::out 
                                            | std::ofstream::app );

                        assert (LogLevel == LogLevels::LOG_NONE
                                || LogLevel == LogLevels::LOG_NOTICE
                                || LogLevel == LogLevels::LOG_ERROR
                                || LogLevel == LogLevels::LOG_INFO 
                                || LogLevel == LogLevels::LOG_VERBOSE);
                    };

            //! Logs a new message with the indicated LogLevel
            void LogMessage(int LogLevel, std::string Message);

            //! Raises an exception which calls ShutDown
            void RaiseException(std::string ErrorMessage);

            //! Shuts down the programmable controller
            void ShutDown();
    };
}

#endif