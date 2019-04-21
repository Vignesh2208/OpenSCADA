#ifndef __PC_EMULATOR_INCLUDE_INSNS_MUL_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_MUL_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {

    //! Multiply instruction
    class MUL_Insn: public Insn {
        public:
            MUL_Insn(PCResourceImpl * AssociatedResource, bool isNegated) {
                __AssociatedResource = AssociatedResource;
                IsNegated = isNegated;
                __InsnName = "MUL";
            };

           //! Called to execute the instruction
            /*!
                \param Operands     Operands to the instruction
            */
            void Execute(PCVariable *CurrentResult,
                std::vector<PCVariable*>& Operands);
    };
}

#endif