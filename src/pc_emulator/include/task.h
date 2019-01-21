#ifndef __PC_EMULATOR_INCLUDE_TASK_H__
#define __PC_EMULATOR_INCLUDE_TASK_H__

#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <unordered_map>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "pc_logger.h"
#include "pc_mem_unit.h"
#include "executor.h"


#include "pc_emulator/proto/configuration.pb.h"

using namespace std;

namespace pc_emulator {

    class PCConfiguration;
    class PCVariable;

    class Task {
        public :



    };
}

#endif