#ifndef __PC_EMULATOR_INCLUDE_PC_FUNCTIONS_REGISTRY_H__
#define __PC_EMULATOR_INCLUDE_PC_FUNCTIONS_REGISTRY_H__

#include <iostream>
#include <unordered_map>
#include "pc_configuration.h"
#include "pc_resource.h"
#include "pc_variable.h"
#include "pc_datatype.h"

using namespace std;


namespace pc_emulator{

    //! Class which registers and tracks all valid Functions with a Code Body
    class FunctionsRegistry {
        private:
            
            std::unordered_map<std::string, std::unique_ptr<PCVariable>> 
                __FunctionsRegistry; /*!< Hash map of function name, variable obj */
            PCResourceImpl * __AssociatedResource; /*!< Resource associated with
                this registry */
        public:

            //! Constructor
            FunctionsRegistry(PCResourceImpl * AssociatedResource);

            //! Retrieve Function variable object with the specified name
            /*!
                \param FnName Name of the Function
                \return Function variable object
            */
            PCVariable* GetFunction(string FnName);
    };
}

#endif