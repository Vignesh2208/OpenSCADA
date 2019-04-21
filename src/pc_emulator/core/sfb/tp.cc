#include "src/pc_emulator/include/sfb/tp.h"
#include "src/pc_emulator/include/utils.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

void TP::Execute(PCVariable * __CurrentResult, PCVariable * SFB) {
    assert(SFB != nullptr && SFB->__VariableDataType->__DataTypeName
        == __SFBName);
}