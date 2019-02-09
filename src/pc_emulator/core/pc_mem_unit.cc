
#include <assert.h>
#include "src/pc_emulator/include/pc_mem_unit.h"

using namespace std;
using namespace pc_emulator;



void PCMemUnit::AllocateStaticMemory(int MemSize) {

    if (!__Initialized) {
        __BaseStorageLocation = std::shared_ptr<char>(new char[MemSize], 
                    [](char *p) { delete[] p; });
        __MemUnitSizeBytes = MemSize;

        for (int i = 0; i < MemSize; i++) {
            __BaseStorageLocation.get()[i] = '\0';
        }
    }
    __Initialized = true;
}

std::shared_ptr<char> PCMemUnit::GetStorageLocation() {
    return __BaseStorageLocation;
}

int PCMemUnit::GetMemUnitSize() {
    return __MemUnitSizeBytes;
}

void PCMemUnit::SetMemUnitLocation(PCMemUnit * From) {
    assert (From != nullptr);
    __BaseStorageLocation = From->GetStorageLocation();
    __MemUnitSizeBytes = From->GetMemUnitSize();
    __Initialized = true;

}

void * PCMemUnit::GetPointerToMemory(int Offset) {
    if (__Initialized) {
        assert(Offset < __MemUnitSizeBytes);
        return (void *)&__BaseStorageLocation.get()[Offset];
    }

    return nullptr;
}

bool PCMemUnit::IsInitialized() {
    return __Initialized;
}

void PCMemUnit::CopyFromMemUnit(PCMemUnit * From, int FromStartOffset,
                                int CopySizeBytes, int ToStartOffset) {
    assert (From != nullptr);
    assert (From->GetMemUnitSize() > FromStartOffset);
    assert (ToStartOffset + CopySizeBytes <= __MemUnitSizeBytes);
    assert (FromStartOffset + CopySizeBytes <= From->GetMemUnitSize());

    for (int i = ToStartOffset, j = 0; i < CopySizeBytes; i++, j++) {
        __BaseStorageLocation.get()[i] 
            = From->GetStorageLocation().get()[j + FromStartOffset];
    }

}

void PCMemUnit::operator=(PCMemUnit& MemUnit) {
    this->__Initialized = MemUnit.IsInitialized();
    this->__MemUnitSizeBytes = MemUnit.GetMemUnitSize();
    this->__BaseStorageLocation = MemUnit.GetStorageLocation();
}