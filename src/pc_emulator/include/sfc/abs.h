#ifndef __PC_EMULATOR_INCLUDE_SFC_ABS_H
#define __PC_EMULATOR_INCLUDE_SFC_ABS_H
#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <unordered_map>
#include "src/pc_emulator/proto/configuration.pb.h"
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_resource.h"
#include "src/pc_emulator/include/pc_logger.h"
#include "src/pc_emulator/include/sfc/sfc.h"

using namespace std;
using namespace pc_specification;

namespace pc_emulator {

    //! Definition of ABS SFC
    class ABS: public SFC {
        public:

            ABS(PCResourceImpl * AssociatedResource) {
                __AssociatedResource = AssociatedResource;
                __SfcName = "ABS";
            }
            //! Called to execute the sfc
            /*!
                \param Operands     Operands to the sfc
            */
            void Execute(PCVariable *CurrentResult,
                std::vector<PCVariable*>& Operands);
    };

}


#endif