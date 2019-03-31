#ifndef __PC_EMULATOR_INCLUDE_INSNS_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_INSN_H
#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <unordered_map>
#include "src/pc_emulator/proto/configuration.pb.h"
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_resource.h"
#include "src/pc_emulator/include/pc_logger.h"

using namespace std;
using namespace pc_specification;

namespace pc_emulator {
    class Insn {
        public:
            PCResource * __AssociatedResource;
            string __InsnName;
            virtual void Execute(std::vector<PCVariable*>& Operands,
                    bool isNegated) = 0;
    };

}


#endif