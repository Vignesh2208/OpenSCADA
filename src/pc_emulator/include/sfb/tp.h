#ifndef __PC_EMULATOR_INCLUDE_SFB_TP_H
#define __PC_EMULATOR_INCLUDE_SFB_TP_H
#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <unordered_map>
#include "src/pc_emulator/proto/configuration.pb.h"
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_resource.h"
#include "src/pc_emulator/include/pc_logger.h"
#include "src/pc_emulator/include/sfb/sfb.h"

using namespace std;
using namespace pc_specification;

namespace pc_emulator {

    //! Definition of TP SFB
    class TP: public SFB {
        public:

            TP(PCResourceImpl * AssociatedResource) {
                __AssociatedResource = AssociatedResource;
                __SFBName = "TP";
            }
            //! Called to execute the sfb
            /*!
                \param SFB     
            */
            void Execute(PCVariable * SFB);
    };

}


#endif