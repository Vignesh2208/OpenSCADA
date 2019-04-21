#ifndef __PC_EMULATOR_INCLUDE_INSNS_LT_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_LT_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {

    //! Lesser than instruction
    class LT_Insn: public Insn {
        public:
            LT_Insn(PCResourceImpl * AssociatedResource, bool isNegated) {
                __AssociatedResource = AssociatedResource;
                IsNegated = isNegated;
                __InsnName = "LT";
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