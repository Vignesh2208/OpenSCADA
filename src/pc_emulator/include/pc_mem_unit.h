#ifndef __PC_EMULATOR_INCLUDE_PC_MEM_UNIT_H__
#define __PC_EMULATOR_INCLUDE_PC_MEM_UNIT_H__
#include <iostream>
#include <memory>

using namespace std;

namespace pc_emulator {


    class PCMemUnit {
        private:
            std::shared_ptr<char> __BaseStorageLocation;
            bool __Initialized;
            int __MemUnitSizeBytes;
        public:
            PCMemUnit() 
                : __MemUnitSizeBytes(0), __Initialized(false) {};
            void AllocateStaticMemory(int SizeBytes);
            std::shared_ptr<char> GetStorageLocation();
            bool IsInitialized();
            int GetMemUnitSize();
            void SetMemUnitLocation(PCMemUnit * From);
            void * GetPointerToMemory(int Offset);
            void CopyFromMemUnit(PCMemUnit * From, int FromStartOffset,
                                int CopySizeBytes, int ToStartOffset);

            void operator=(PCMemUnit& MemUnit);

    };
}

#endif