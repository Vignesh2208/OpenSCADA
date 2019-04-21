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

    //! Generic abstract class for an IL instruction
    class Insn {
        public:

            PCResourceImpl * __AssociatedResource; /*!< Associated resource */
            string __InsnName;  /*!< Instruction name */
            bool IsNegated; /*!< Should each operand be negated */

            //! Called to execute the instruction
            /*!
                \param Operands     Operands to the instruction
            */
            virtual void Execute(PCVariable *CurrentResult,
                std::vector<PCVariable*>& Operands) = 0;
    };

}


#endif