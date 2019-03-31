#ifndef __PC_EMULATOR_INCLUDE_INSNS_SUB_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_SUB_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {
    class SUB_Insn: public Insn {
        public:
            SUB_Insn(PCResource * AssociatedResource) {
                __AssociatedResource = AssociatedResource;
                __InsnName = "SUB";
            };

            void Execute(std::vector<PCVariable*>& Operands,
                    bool isNegated);
    };
}

#endif