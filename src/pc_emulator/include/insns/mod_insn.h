#ifndef __PC_EMULATOR_INCLUDE_INSNS_MOD_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_MOD_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {

    //! MOD instruction
    class MOD_Insn: public Insn {
        public:
            MOD_Insn(PCResourceImpl * AssociatedResource, bool isNegated) {
                __AssociatedResource = AssociatedResource;
                IsNegated = isNegated;
                __InsnName = "MOD";
            };

           //! Called to execute the instruction
            /*!
                \param Operands     Operands to the instruction
            */
            void Execute(std::vector<PCVariable*>& Operands);
    };
}

#endif