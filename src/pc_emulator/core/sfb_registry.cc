#include <assert.h>
#include "src/pc_emulator/include/insn_registry.h"
#include "src/pc_emulator/include/pc_logger.h"
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_resource.h"
#include "src/pc_emulator/include/sfb_registry.h"

using namespace pc_emulator;
using namespace std;
using namespace pc_specification;

SFBRegistry::SFBRegistry(PCResourceImpl * AssociatedResource) {
    __SFB.insert(std::make_pair("TP", 
        std::unique_ptr<TP>(new TP(AssociatedResource))));

    __SFB.insert(std::make_pair("TON", 
        std::unique_ptr<TON>(new TON(AssociatedResource))));
    
    __SFB.insert(std::make_pair("TOF", 
        std::unique_ptr<TOF>(new TOF(AssociatedResource))));
}

SFB* SFBRegistry::GetSFB(string SFBName) {
    auto got = __SFB.find(SFBName);

    if (got == __SFB.end())
        return nullptr; // Unknown SFB
    else {
        return got->second.get();
    }
}