#ifndef __PC_EMULATOR_INCLUDE_INSNS_SHR_H
#define __PC_EMULATOR_INCLUDE_INSNS_SHR_H
#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <unordered_map>
#include "src/pc_emulator/include/insns/insn.h"


using namespace std;
using namespace pc_specification;

namespace pc_emulator {

    //! Generic abstract class for an Shift right instruction
    class SHR: public Insn {
        public:

            SHR(PCResourceImpl* AssociatedResource) {
                __InsnName = "SHR";
                __AssociatedResource = AssociatedResource;
            }
            //! Called to execute the instruction
            /*!
                \param Operands     Operands to the instruction
                \param isNegated    Should operands be negated before instruction operation
            */
            void Execute(std::vector<PCVariable*>& Operands,
                    bool isNegated);
    };

}


#endif