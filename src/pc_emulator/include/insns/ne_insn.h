#ifndef __PC_EMULATOR_INCLUDE_INSNS_NE_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_NE_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {
    class NE_Insn: public Insn {
        public:
            NE_Insn(PCResourceImpl * AssociatedResource) {
                __AssociatedResource = AssociatedResource;
                __InsnName = "NE";
            };

            void Execute(std::vector<PCVariable*>& Operands,
                    bool isNegated);
    };
}

#endif