#ifndef __PC_EMULATOR_INCLUDE_SFC_SFC_H
#define __PC_EMULATOR_INCLUDE_SFC_SFC_H
#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <unordered_map>
#include <math.h>
#include "src/pc_emulator/proto/configuration.pb.h"
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_resource.h"
#include "src/pc_emulator/include/pc_logger.h"

using namespace std;
using namespace pc_specification;

namespace pc_emulator {

    //! Generic abstract class for an IL instruction
    class SFC {
        public:

            PCResourceImpl * __AssociatedResource; /*!< Associated resource */
            string __SfcName;  /*!<  Name of the system function */

            //! Called to execute the sfc
            /*!
                \param Operands     Operands to the sfc
            */
            virtual void Execute(std::vector<PCVariable*>& Operands) = 0;
    };

}


#endif