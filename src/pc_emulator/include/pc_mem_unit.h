#ifndef __PC_EMULATOR_INCLUDE_PC_MEM_UNIT_H__
#define __PC_EMULATOR_INCLUDE_PC_MEM_UNIT_H__
#include <iostream>
#include <memory>
#include <semaphore.h>



using namespace std;

namespace pc_emulator {



    //! Describes a memory unit and provides api to access it
    class PCMemUnit {
        private:
            std::shared_ptr<char> 
                __BaseStorageLocation; /*!< Data of this memory unit is stored her */ 
            bool __Initialized; /*!< Set to true iff initialized */
            int __MemUnitSizeBytes; /*!< Size of memory unit in bytes */

	    int __acquireFlock(std::string lockFilePath);
	    void __releaseFlock(int lockFd);
            
        public:
            bool __isMemControllerActive; /*!< Set to true iff MMAP'ed memory is allocated */
            sem_t * __sem_lock; /*!< A semaphore lock used to access MMAP'ed memory */
            string __sem_name;  /*!< Name of the semaphore */
            string __mmap_file_name;    /*!< Name of the mmaped file */

            //! Constructor
            PCMemUnit() 
                : __MemUnitSizeBytes(0), __Initialized(false),
                __isMemControllerActive(false), __sem_lock(nullptr),
                __mmap_file_name("") {};

            //!Allocate ordinary memory with specified size
            void AllocateStaticMemory(int SizeBytes);

            //!Allocate MMAP'ed memory
            /*!
                \param SizeBytes Size of memory to be allocated in bytes
                \param mmap_file_path   Absolute path to mmap file location
                \param lock_name    Name assigned to semaphore
                \sa __Initialized is set to true
            */
            void AllocateSharedMemory(int SizeBytes, string mmap_file_path,
                                string lock_name);

            //! Returns a pointer to the base storage location
            std::shared_ptr<char> GetStorageLocation();

            //! Returns true if memory unit is initialized
            bool IsInitialized();

            //! Returns the size of the memory unit
            int GetMemUnitSize();

            //! Assigns memory location from another memory unit to this one
            /*!
                \param From base storage location of this unit is made to point
                    to the base storage location of "From"
            */
            void SetMemUnitLocation(PCMemUnit * From);

            //! Returns a pointer to memory location with specified offset
            void * GetPointerToMemory(int Offset);

            //! Copies data from another memory unit
            /*!
                Copies data starting from the specified "FromStartOffset" and stores
                it to locations starting from the specified "ToStartOffset"

                \param From MemUnit to copy from
                \param FromStartOffset  Offset added to From's MemUnit
                \param CopySizeBytes    Amount of data to copy
                \param ToStartOffset    Offset added to this MemUnit
            */
            void CopyFromMemUnit(PCMemUnit * From, int FromStartOffset,
                                int CopySizeBytes, int ToStartOffset);

            //! Frees memory already allocated and reallocates it
            void ReallocateStaticMemory(int MemSize);

            //! Calls MunMap and destroyes the created semaphore
            void Cleanup();
            
            //! Checks if two MemUnits are equal
            /*!
                \param MemUnit1 Memory Unit 1
                \param MemUnit2 Memory Unit 2
                \return true iff both have the same base storage location
            */
            friend bool operator==(const PCMemUnit& MemUnit1,
                                    const PCMemUnit& MemUnit2) {
                return MemUnit1.__BaseStorageLocation.get()
                            == MemUnit2.__BaseStorageLocation.get();
            }

    };
}

#endif
