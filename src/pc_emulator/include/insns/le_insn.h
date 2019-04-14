#ifndef __PC_EMULATOR_INCLUDE_INSNS_LE_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_LE_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {
    class LE_Insn: public Insn {
        public:
            LE_Insn(PCResourceImpl * AssociatedResource) {
                __AssociatedResource = AssociatedResource;
                __InsnName = "LE";
            };

            void Execute(std::vector<PCVariable*>& Operands,
                    bool isNegated);
    };
}

#endif