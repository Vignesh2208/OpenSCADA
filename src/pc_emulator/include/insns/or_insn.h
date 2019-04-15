#ifndef __PC_EMULATOR_INCLUDE_INSNS_OR_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_OR_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {

    //! Bitwise OR instruction
    class OR_Insn: public Insn {
        public:
            OR_Insn(PCResourceImpl * AssociatedResource) {
                __AssociatedResource = AssociatedResource;
                __InsnName = "OR";
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