#ifndef __PC_EMULATOR_INCLUDE_PC_INSN_REGISTRY_H__
#define __PC_EMULATOR_INCLUDE_PC_INSN_REGISTRY_H__

#include <iostream>
#include <unordered_map>
#include "pc_configuration.h"
#include "pc_resource.h"
#include "insns/insn.h"
#include "insns/ld_insn.h"
#include "insns/st_insn.h"
#include "insns/add_insn.h"
#include "insns/div_insn.h"
#include "insns/eq_insn.h"
#include "insns/ge_insn.h"
#include "insns/gt_insn.h"
#include "insns/le_insn.h"
#include "insns/lt_insn.h"
#include "insns/st_insn.h"
#include "insns/mod_insn.h"
#include "insns/mul_insn.h"
#include "insns/ne_insn.h"
#include "insns/not_insn.h"
#include "insns/or_insn.h"
#include "insns/sub_insn.h"
#include "insns/and_insn.h"
#include "insns/xor_insn.h"

using namespace std;


namespace pc_emulator{

    //! Class which registers and tracks all valid instructions
    class InsnRegistry {
        private:
            
            std::unordered_map<std::string, std::unique_ptr<Insn>> 
                __InsnRegistry; /*!< Hash map of instruction name, Insn obj */
        public:

            //! Constructor
            InsnRegistry(PCResourceImpl * AssociatedResource);

            //! Retrieve Insn object with the specified instruction name
            /*!
                \param InsnName Name of the instruction
                \return Insn object
            */
            Insn* GetInsn(string InsnName);
    };
}

#endif