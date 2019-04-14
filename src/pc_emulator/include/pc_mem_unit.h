#ifndef __PC_EMULATOR_INCLUDE_PC_MEM_UNIT_H__
#define __PC_EMULATOR_INCLUDE_PC_MEM_UNIT_H__
#include <iostream>
#include <memory>
#include <semaphore.h>



using namespace std;

namespace pc_emulator {

    /*
    template <typename T> std::shared_ptr<T> make_mmap_shared(
                int nElements, int fd)
    {
        constexpr auto PROT_RW = PROT_READ | PROT_WRITE;
        constexpr auto MAP_ALLOC = MAP_SHARED;

        assert (nElements > 0 && fd != -1);
        
        if (auto ptr = mmap(0,
                        sizeof(T)*nElements, PROT_RW, MAP_ALLOC, fd, 0)) {
            return {
                new (ptr) T[nElements],
                [](T*p) {
                }
            };
        }
        std::cout << "MMAP failed !" << std::endl;
        throw std::bad_alloc();
    }*/


    class PCMemUnit {
        private:
            std::shared_ptr<char> __BaseStorageLocation;
            bool __Initialized;
            int __MemUnitSizeBytes;
            
        public:
            bool __isMemControllerActive;
            sem_t * __sem_lock;
            string __sem_name;
            string __mmap_file_name;

            PCMemUnit() 
                : __MemUnitSizeBytes(0), __Initialized(false),
                __isMemControllerActive(false), __sem_lock(nullptr),
                __mmap_file_name("") {};
            void AllocateStaticMemory(int SizeBytes);
            void AllocateSharedMemory(int SizeBytes, string mmap_file_name,
                                string lock_name);
            std::shared_ptr<char> GetStorageLocation();
            bool IsInitialized();
            int GetMemUnitSize();
            void SetMemUnitLocation(PCMemUnit * From);
            void * GetPointerToMemory(int Offset);
            void CopyFromMemUnit(PCMemUnit * From, int FromStartOffset,
                                int CopySizeBytes, int ToStartOffset);
            void ReallocateStaticMemory(int MemSize);
            void Cleanup();
            
            friend bool operator==(const PCMemUnit& MemUnit1,
                                    const PCMemUnit& MemUnit2) {
                return MemUnit1.__BaseStorageLocation.get()
                            == MemUnit2.__BaseStorageLocation.get();
            }

    };
}

#endif