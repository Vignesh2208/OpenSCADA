#ifndef __PC_EMULATOR_INCLUDE_INSNS_SUB_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_SUB_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {

    //! Subtraction instruction
    class SUB_Insn: public Insn {
        public:
            SUB_Insn(PCResourceImpl * AssociatedResource, bool isNegated) {
                __AssociatedResource = AssociatedResource;
                __InsnName = "SUB";
                IsNegated = isNegated;
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