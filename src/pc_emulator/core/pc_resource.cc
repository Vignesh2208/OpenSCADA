#include <assert>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <regex>

#include "pc_emulator/include/pc_datatype.h"
#include "pc_emulator/include/pc_variable.h"
#include "pc_emulator/include/pc_resource.h"
#include "pc_emulator/include/pc_configuration.h"

using namespace std;
using namespace pc_emulator;

PCVariable * PCResource::GetVariablePointerToMem(int MemType, int ByteOffset,
                        int BitOffset, string VariableDataTypeName) {

    assert(ByteOffset > 0 && BitOffset >= 0 && BitOffset < 8);
    assert(MemType == MEM_TYPE::INPUT_MEM || MemType == MEM_TYPE::OUTPUT_MEM);
    string VariableName = __ResourceName + std::to_string(MemType)
                            + "." + std::to_string(ByteOffset)
                            + "." _ std::to_string(BitOffset);

    // need to track and delete this variable later on
    PCVariable* V = new PCVariable(__configuration, this, VariableName,
                                VariableDataTypeName);
    assert(V != nullptr);

    if(MemType == MEM_TYPE::INPUT_MEM)
        V->__MemoryLocation.SetMemUnitLocation(__InputMemory);
    else 
        V->__MemoryLocation.SetMemUnitLocation(__OutputMemory);

    V->__ByteOffset = ByteOffset;
    V->__BitOffset = BitOffset;

    return V;     
}