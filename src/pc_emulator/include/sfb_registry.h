#ifndef __PC_EMULATOR_INCLUDE_PC_SFB_REGISTRY_H__
#define __PC_EMULATOR_INCLUDE_PC_SFB_REGISTRY_H__

#include <iostream>
#include <unordered_map>
#include "pc_configuration.h"
#include "pc_resource.h"
#include "sfb/sfb.h"
#include "sfb/tp.h"
#include "sfb/ton.h"
#include "sfb/tof.h"


using namespace std;


namespace pc_emulator{

    //! Class which registers and tracks all valid SFB executors without code body
    class SFBRegistry {
        private:
            
            std::unordered_map<std::string, std::unique_ptr<SFB>> 
                __SFB; /*!< Hash map of SFB name, SFBExec obj */
        public:

            //! Constructor
            SFBRegistry(PCResourceImpl * AssociatedResource);

            //! Retrieve SFB object with the specified SFB name
            /*!
                \param SFBName Name of the instruction
                \return SFB object
            */
            SFB* GetSFB(string SFBName);
    };
}

#endif