#ifndef __PC_EMULATOR_INCLUDE_SFC_MUX_H
#define __PC_EMULATOR_INCLUDE_SFC_MUX_H
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

    //! Definition of MUX SFC
    class MUX: public SFC {
        public:

            MUX(PCResourceImpl * AssociatedResource) {
                __AssociatedResource = AssociatedResource;
                __SfcName = "MUX";
            }
            //! Called to execute the sfc
            /*!
                \param Operands     Operands to the sfc
            */
            void Execute(std::vector<PCVariable*>& Operands);
    };

}


#endif