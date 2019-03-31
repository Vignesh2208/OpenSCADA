#ifndef __PC_EMULATOR_INCLUDE_INSNS_LT_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_LT_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {
    class LT_Insn: public Insn {
        public:
            LT_Insn(PCResource * AssociatedResource) {
                __AssociatedResource = AssociatedResource;
                __InsnName = "LT";
            };

            void Execute(std::vector<PCVariable*>& Operands,
                    bool isNegated);
    };
}

#endif