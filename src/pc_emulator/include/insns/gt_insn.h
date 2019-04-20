#ifndef __PC_EMULATOR_INCLUDE_INSNS_GT_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_GT_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {

    //! Greater than instruction
    class GT_Insn: public Insn {
        public:
            GT_Insn(PCResourceImpl * AssociatedResource, bool isNegated) {
                __AssociatedResource = AssociatedResource;
                IsNegated = isNegated;
                __InsnName = "GT";
            };

           //! Called to execute the instruction
            /*!
                \param Operands     Operands to the instruction
            */
            void Execute(std::vector<PCVariable*>& Operands);
    };
}

#endif