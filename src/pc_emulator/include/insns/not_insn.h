#ifndef __PC_EMULATOR_INCLUDE_INSNS_NOT_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_NOT_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {
    class NOT_Insn: public Insn {
        public:
            NOT_Insn(PCResourceImpl * AssociatedResource) {
                __AssociatedResource = AssociatedResource;
                __InsnName = "NOT";
            };

            void Execute(std::vector<PCVariable*>& Operands,
                    bool isNegated);
    };
}

#endif