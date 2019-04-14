#ifndef __PC_EMULATOR_INCLUDE_INSNS_AND_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_AND_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {
    class AND_Insn: public Insn {
        public:
            AND_Insn(PCResourceImpl * AssociatedResource) {
                __AssociatedResource = AssociatedResource;
                __InsnName = "AND";
            };

            void Execute(std::vector<PCVariable*>& Operands,
                    bool isNegated);
    };
}

#endif