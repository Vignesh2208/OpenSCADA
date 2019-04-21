#include "src/pc_emulator/include/sfb/ton.h"
#include "src/pc_emulator/include/utils.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

void TON::Execute(PCVariable * __CurrentResult, PCVariable * SFB) {
    assert(SFB != nullptr && SFB->__VariableDataType->__DataTypeName
        == __SFBName);
}