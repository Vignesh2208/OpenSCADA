#ifndef __PC_EMULATOR_INCLUDE_SFC_TAN_H
#define __PC_EMULATOR_INCLUDE_SFC_TAN_H
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

    //! Definition of TAN SFC
    class TAN: public SFC {
        public:

            TAN(PCResourceImpl * AssociatedResource) {
                __AssociatedResource = AssociatedResource;
                __SfcName = "TAN";
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