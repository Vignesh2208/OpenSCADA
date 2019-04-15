#ifndef __PC_EMULATOR_INCLUDE_INSNS_GT_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_GT_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {

    //! Greater than instruction
    class GT_Insn: public Insn {
        public:
            GT_Insn(PCResourceImpl * AssociatedResource) {
                __AssociatedResource = AssociatedResource;
                __InsnName = "GT";
            };

           //! Called to execute the instruction
            /*!
                \param Operands     Operands to the instruction
                \param isNegated    Should operands be negated before instruction operation
            */
            void Execute(std::vector<PCVariable*>& Operands,
                    bool isNegated);
    };
}

#endif