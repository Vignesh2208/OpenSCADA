#ifndef __PC_EMULATOR_INCLUDE_INSNS_OR_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_OR_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {

    //! Bitwise OR instruction
    class OR_Insn: public Insn {
        public:
            OR_Insn(PCResourceImpl * AssociatedResource, bool isNegated) {
                __AssociatedResource = AssociatedResource;
                IsNegated = isNegated;
                __InsnName = "OR";
            };

           //! Called to execute the instruction
            /*!
                \param CurrentResult    The CurrentResult register
                    of the task executing this Insn
                \param Operands     Operands to the instruction
            */
            void Execute(PCVariable *CurrentResult,
                std::vector<PCVariable*>& Operands);
    };
}

#endif