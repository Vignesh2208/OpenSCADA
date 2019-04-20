#ifndef __PC_EMULATOR_INCLUDE_INSNS_NE_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_NE_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {

    //! Test for not equal instruction
    class NE_Insn: public Insn {
        public:
            NE_Insn(PCResourceImpl * AssociatedResource, bool isNegated) {
                __AssociatedResource = AssociatedResource;
                IsNegated = isNegated;
                __InsnName = "NE";
            };

           //! Called to execute the instruction
            /*!
                \param Operands     Operands to the instruction
            */
            void Execute(std::vector<PCVariable*>& Operands);
    };
}

#endif