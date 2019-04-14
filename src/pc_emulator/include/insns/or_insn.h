#ifndef __PC_EMULATOR_INCLUDE_INSNS_OR_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_OR_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {
    class OR_Insn: public Insn {
        public:
            OR_Insn(PCResourceImpl * AssociatedResource) {
                __AssociatedResource = AssociatedResource;
                __InsnName = "OR";
            };

            void Execute(std::vector<PCVariable*>& Operands,
                    bool isNegated);
    };
}

#endif