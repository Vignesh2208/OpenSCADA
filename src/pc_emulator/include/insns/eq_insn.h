#ifndef __PC_EMULATOR_INCLUDE_INSNS_EQ_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_EQ_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {

    //! Equality checking instruction
    class EQ_Insn: public Insn {
        public:
            EQ_Insn(PCResourceImpl * AssociatedResource, bool isNegated) {
                __AssociatedResource = AssociatedResource;
                IsNegated = isNegated;
                __InsnName = "EQ";
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