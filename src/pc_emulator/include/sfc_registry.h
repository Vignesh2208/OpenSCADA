#ifndef __PC_EMULATOR_INCLUDE_PC_SFC_REGISTRY_H__
#define __PC_EMULATOR_INCLUDE_PC_SFC_REGISTRY_H__

#include <iostream>
#include <unordered_map>
#include "pc_configuration.h"
#include "pc_resource.h"
#include "sfc/sfc.h"
#include "sfc/abs.h"
#include "sfc/acos.h"
#include "sfc/any_to_any.h"
#include "sfc/asin.h"
#include "sfc/atan.h"
#include "sfc/cos.h"
#include "sfc/exp.h"
#include "sfc/limit.h"
#include "sfc/ln.h"
#include "sfc/log.h"
#include "sfc/max.h"
#include "sfc/min.h"
#include "sfc/mux.h"
#include "sfc/sel.h"
#include "sfc/sin.h"
#include "sfc/sqrt.h"
#include "sfc/tan.h"


using namespace std;


namespace pc_emulator{

    //! Class which registers and tracks all valid SFCs
    class SFCRegistry {
        private:
            
            std::unordered_map<std::string, std::unique_ptr<SFC>> 
                __SFC; /*!< Hash map of SFC name, SFC obj */
        public:

            //! Constructor
            SFCRegistry(PCResourceImpl * AssociatedResource);

            //! Retrieve SFC object with the specified SFC name
            /*!
                \param SFCName Name of the instruction
                \return SFC object
            */
            SFC* GetSFC(string SFCName);
    };
}

#endif