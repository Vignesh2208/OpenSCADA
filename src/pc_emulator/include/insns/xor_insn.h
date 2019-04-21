#ifndef __PC_EMULATOR_INCLUDE_INSNS_XOR_INSN_H
#define __PC_EMULATOR_INCLUDE_INSNS_XOR_INSN_H


#include "src/pc_emulator/include/insns/insn.h"

namespace pc_emulator {

    //! Bitwise XOR instruction
    class XOR_Insn: public Insn {
        public:
            XOR_Insn(PCResourceImpl * AssociatedResource, bool isNegated) {
                __AssociatedResource = AssociatedResource;
                __InsnName = "XOR";
                IsNegated = isNegated;
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