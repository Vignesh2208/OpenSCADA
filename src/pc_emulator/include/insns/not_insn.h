#ifndef __PC_EMULATOR_INCLUDE_INSNS_NOT_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_NOT_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {

    //! NOT instruction
    class NOT_Insn: public Insn {
        public:
            NOT_Insn(PCResourceImpl * AssociatedResource) {
                __AssociatedResource = AssociatedResource;
                __InsnName = "NOT";
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