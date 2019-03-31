#ifndef __PC_EMULATOR_INCLUDE_INSNS_XOR_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_XOR_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {
    class XOR_Insn: public Insn {
        public:
            XOR_Insn(PCResource * AssociatedResource) {
                __AssociatedResource = AssociatedResource;
                __InsnName = "XOR";
            };

            void Execute(std::vector<PCVariable*>& Operands,
                    bool isNegated);
    };
}

#endif