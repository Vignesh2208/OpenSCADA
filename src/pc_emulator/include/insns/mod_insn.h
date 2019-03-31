#ifndef __PC_EMULATOR_INCLUDE_INSNS_MOD_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_MOD_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {
    class MOD_Insn: public Insn {
        public:
            MOD_Insn(PCResource * AssociatedResource) {
                __AssociatedResource = AssociatedResource;
                __InsnName = "MOD";
            };

            void Execute(std::vector<PCVariable*>& Operands,
                    bool isNegated);
    };
}

#endif