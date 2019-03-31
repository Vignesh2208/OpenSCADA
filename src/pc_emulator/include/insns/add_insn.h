#ifndef __PC_EMULATOR_INCLUDE_INSNS_ADD_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_ADD_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {
    class ADD_Insn: public Insn {
        public:
            ADD_Insn(PCResource * AssociatedResource) {
                __AssociatedResource = AssociatedResource;
                __InsnName = "ADD";
            };

            void Execute(std::vector<PCVariable*>& Operands,
                    bool isNegated);
    };
}

#endif