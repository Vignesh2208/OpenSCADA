#ifndef __PC_EMULATOR_INCLUDE_INSNS_GT_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_GT_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {
    class GT_Insn: public Insn {
        public:
            GT_Insn(PCResource * AssociatedResource) {
                __AssociatedResource = AssociatedResource;
                __InsnName = "GT";
            };

            void Execute(std::vector<PCVariable*>& Operands,
                    bool isNegated);
    };
}

#endif