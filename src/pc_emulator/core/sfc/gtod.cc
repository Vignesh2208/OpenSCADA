#include "src/pc_emulator/include/sfc/gtod.h"
#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/include/sfc_registry.h"
#include "src/pc_emulator/include/sfc/any_to_any.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

void GTOD::Execute(PCVariable * __CurrentResult,
    std::vector<PCVariable*>& MOperands) {
    auto configuration = __AssociatedResource->__configuration;
    auto CR = __CurrentResult;
    auto Tmp = __AssociatedResource->GetTmpVariable("TIME", "t#0s");
    *CR = *Tmp;
    auto curr_time = __AssociatedResource->clock->GetCurrentTime();
    CR->SetField("", "t#" + std::to_string(curr_time) + "s");
}