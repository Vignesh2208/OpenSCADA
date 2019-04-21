#include "src/pc_emulator/include/sfc/sel.h"
#include "src/pc_emulator/include/utils.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

void SEL::Execute(PCVariable * __CurrentResult,
    std::vector<PCVariable*>& MOperands) {
    auto configuration = __AssociatedResource->__configuration;
    auto CR = __CurrentResult;

    std::vector<PCVariable*> Operands;
    for(int i = 0; i < MOperands.size(); i++) {
        if (MOperands[i]->__IsVariableContentTypeAPtr) {
            auto Tmp = MOperands[i]->GetPtrStoredAtField("");
            assert(Tmp != nullptr);
            Operands.push_back(Tmp);
        } else {
            Operands.push_back(MOperands[i]);
        }
    }
    if (CR->__VariableDataType->__DataTypeCategory
        != DataTypeCategory::BOOL) {
        configuration->PCLogger->RaiseException("SEL SFC error: CR is not "
            " a boolean");
    }

    if (Operands.size() != 2) {
        configuration->PCLogger->RaiseException("SEL SFC error: "
            "Exactly 2 operands needed!");
    }

    
    bool SelValue = CR->GetValueStoredAtField<bool>("", DataTypeCategory::BOOL);

    std::cout << "Selection Value: " << SelValue << std::endl;

    if(SelValue == false) {
        *CR = *Operands[0];
    } else {
        *CR = *Operands[1];      
    }
    
}