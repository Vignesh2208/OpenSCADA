#ifndef __PC_EMULATOR_INCLUDE_INSNS_ADD_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_ADD_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {

    //! ADD instruction
    class ADD_Insn: public Insn {
        public:
            ADD_Insn(PCResourceImpl * AssociatedResource, bool isNegated) {
                __AssociatedResource = AssociatedResource;
                IsNegated = isNegated;
                __InsnName = "ADD";
            };

            //! Called to execute the instruction
            /*!
                \param Operands     Operands to the instruction
            */
            void Execute(std::vector<PCVariable*>& Operands);
    };
}

#endif