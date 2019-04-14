#ifndef __PC_EMULATOR_INCLUDE_INSNS_MUL_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_MUL_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {
    class MUL_Insn: public Insn {
        public:
            MUL_Insn(PCResourceImpl * AssociatedResource) {
                __AssociatedResource = AssociatedResource;
                __InsnName = "MUL";
            };

            void Execute(std::vector<PCVariable*>& Operands,
                    bool isNegated);
    };
}

#endif