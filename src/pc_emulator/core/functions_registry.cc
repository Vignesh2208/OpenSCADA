#include <assert.h>
#include "src/pc_emulator/include/functions_registry.h"
#include "src/pc_emulator/include/pc_logger.h"
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_resource.h"

using namespace pc_emulator;
using namespace std;
using namespace pc_specification;

FunctionsRegistry::FunctionsRegistry(PCResourceImpl * AssociatedResource) {
    assert(AssociatedResource != nullptr);

    __AssociatedResource = AssociatedResource;

    for(auto it = __AssociatedResource->__ResourcePoUVars.begin();
        it != __AssociatedResource->__ResourcePoUVars.end(); it++) {
        PCVariable * PoU = it->second.get();

        if (PoU->__VariableDataType->__PoUType 
            == pc_specification::PoUType::FC) {
            auto new_func_var = std::unique_ptr<PCVariable>(
                new PCVariable(__AssociatedResource->__configuration,
                    __AssociatedResource,
                    PoU->__VariableName,
                    PoU->__VariableDataType->__DataTypeName));
            new_func_var->AllocateAndInitialize();
            __FunctionsRegistry.insert(std::make_pair(
                PoU->__VariableDataType->__DataTypeName,
                std::move(new_func_var))); 
        }
    }
}

PCVariable* FunctionsRegistry::GetFunction(string FnName) {
    auto got = __FunctionsRegistry.find(FnName);
    if (got == __FunctionsRegistry.end())
        return nullptr;
    
    return got->second.get();
}