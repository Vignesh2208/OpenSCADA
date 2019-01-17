#include <iostream>
#include <cstdint>
#include <cstring>

#include "pc_emulator/include/pc_variable.h"
#include "pc_emulator/include/pc_datatype.h"
#include "pc_emulator/include/pc_configuration.h"
#include "pc_emulator/include/pc_resource.h"

using namespace std;
using namespace pc_emulator;

PCVariable * PCConfiguration::GetVariablePointerToMem(int MemType,
                            int ByteOffset, string VariableDataTypeName) {

    assert(ByteOffset > 0 && BitOffset >= 0 && BitOffset < 8);
    assert(MemType == MEM_TYPE::RAM_MEM);
    string VariableName = __ConfigurationName + std::to_string(MemType)
                            + "." + std::to_string(ByteOffset)
                            + "." _ std::to_string(BitOffset);
    // need to track and delete this variable later on
    PCVariable* V = new PCVariable(this, nullptr, VariableName,
                                VariableDataTypeName);
    assert(V != nullptr);

    
    V->__MemoryLocation.SetMemUnitLocation(__RAMMemory);
    V->__ByteOffset = ByteOffset;
    V->__BitOffset = BitOffset;

    return V;
}