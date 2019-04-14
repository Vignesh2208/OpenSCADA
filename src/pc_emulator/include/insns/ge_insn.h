#ifndef __PC_EMULATOR_INCLUDE_INSNS_GE_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_GE_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {
    class GE_Insn: public Insn {
        public:
            GE_Insn(PCResourceImpl * AssociatedResource) {
                __AssociatedResource = AssociatedResource;
                __InsnName = "GE";
            };

            void Execute(std::vector<PCVariable*>& Operands,
                    bool isNegated);
    };
}

#endif