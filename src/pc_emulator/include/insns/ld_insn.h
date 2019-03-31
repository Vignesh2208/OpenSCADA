#ifndef __PC_EMULATOR_INCLUDE_INSNS_LD_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_LD_INSN_H

#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {
    class LD_Insn: public Insn {
        public:
            LD_Insn(PCResource * AssociatedResource) {
                __AssociatedResource = AssociatedResource;
                __InsnName = "LD";
            };
            void Execute(std::vector<PCVariable*>& Operands,
                    bool isNegated);
    };
}

#endif