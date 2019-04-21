#ifndef __PC_EMULATOR_INCLUDE_INSNS_NOT_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_NOT_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {

    //! NOT instruction
    class NOT_Insn: public Insn {
        public:
            NOT_Insn(PCResourceImpl * AssociatedResource, bool isNegated) {
                __AssociatedResource = AssociatedResource;
                IsNegated = isNegated;
                __InsnName = "NOT";
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