#ifndef __PC_EMULATOR_INCLUDE_INSNS_DIV_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_DIV_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {
    class DIV_Insn: public Insn {
        public:
            DIV_Insn(PCResourceImpl * AssociatedResource) {
                __AssociatedResource = AssociatedResource;
                __InsnName = "DIV";
            };

            void Execute(std::vector<PCVariable*>& Operands,
                    bool isNegated);
    };
}

#endif