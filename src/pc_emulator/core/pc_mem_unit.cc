
#include <assert.h>
#include <unistd.h>
#include <sys/file.h>
#include "src/pc_emulator/include/utils.h"
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

int PCMemUnit::__acquireFlock(
	std::string lockFilePath) {
	int lockFd = open(lockFilePath.c_str(), O_RDWR | O_CREAT, 0666);
	int rc = 0;
	do {
		 rc = flock(lockFd, LOCK_EX | LOCK_NB);
		 if (rc != 0)
			usleep(10000);
	} while (rc != 0);
	return lockFd;
}

void PCMemUnit::__releaseFlock(int lockFd) {
	flock(lockFd, LOCK_UN);
	close(lockFd);
}

void PCMemUnit::AllocateSharedMemory(int MemSize, string mmap_file_name,
                        string lock_name) {
    if (!__Initialized) {
        //__BaseStorageLocation = make_mmap_shared<char>(MemSize, fd_mem);

        bool file_exists = Utils::does_file_exist(mmap_file_name.c_str());
        //std::cout << "Opening sem lock: " << lock_name << std::endl;
        assert((__sem_lock = sem_open(lock_name.c_str(), O_CREAT, 0644, 1))
                != SEM_FAILED);
        
        __sem_name = lock_name;
        int lockFd = __acquireFlock("/tmp/OpenSCADA/" + lock_name + ".lock");
        auto ptr = Utils::make_mmap_shared(MemSize, mmap_file_name);
        __releaseFlock(lockFd);
        
        __BaseStorageLocation = std::shared_ptr<char>(new(ptr) char[MemSize],
                [](char *p) {});
        __MemUnitSizeBytes = MemSize;

        std::cout << "MMAP memory allocated size: " << MemSize << std::endl;
        assert((char *)ptr != (char *)-1);

        if (!file_exists) {
            for (int i = 0; i < MemSize; i++) {
                __BaseStorageLocation.get()[i] = '\0';
            }
        }
        
    }
    __mmap_file_name = mmap_file_name;
    __Initialized = true;
    __isMemControllerActive = true;
}

void PCMemUnit::ReallocateStaticMemory(int MemSize) {

    assert(!__isMemControllerActive);
    __BaseStorageLocation.reset();
    __BaseStorageLocation = std::shared_ptr<char>(new char[MemSize], 
                    [](char *p) { delete[] p; });
    __MemUnitSizeBytes = MemSize;

    for (int i = 0; i < MemSize; i++) {
        __BaseStorageLocation.get()[i] = '\0';
    }
    __Initialized = true;
}

std::shared_ptr<char> PCMemUnit::GetStorageLocation() {
    return __BaseStorageLocation;
}

int PCMemUnit::GetMemUnitSize() {
    return __MemUnitSizeBytes;
}

void PCMemUnit::Cleanup() {
    if (__isMemControllerActive) {
        sem_close(__sem_lock);
        sem_unlink(__sem_name.c_str());
        //std::cout << "MunMapping ... " << std::endl;
        munmap(__BaseStorageLocation.get(), __MemUnitSizeBytes);

        std::remove((__mmap_file_name).c_str());
    }
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

    if (!__Initialized) {
        __BaseStorageLocation = std::shared_ptr<char>(
            new char[__MemUnitSizeBytes], [](char *p) { delete[] p; });
        __Initialized = true;
        __MemUnitSizeBytes = From->GetMemUnitSize();
        for (int i = 0; i < __MemUnitSizeBytes; i++) {
            __BaseStorageLocation.get()[i] = '\0';
        }
        __isMemControllerActive = false;

    }


    if (From->__isMemControllerActive) {

        char * tmp = new char[CopySizeBytes];
        assert(tmp != nullptr);

        sem_wait(From->__sem_lock);
        for(int i = 0; i < CopySizeBytes; i++)
            tmp[i] = From->GetStorageLocation().get()[FromStartOffset + i];
        sem_post(From->__sem_lock);

        for (int i = ToStartOffset, j = 0; 
                            i < ToStartOffset + CopySizeBytes; i++, j++) {
            if (__isMemControllerActive)
                sem_wait(__sem_lock);
            __BaseStorageLocation.get()[i] = tmp[j];

            if (__isMemControllerActive)
                sem_post(__sem_lock);
        }
    } else {
        for (int i = ToStartOffset, j = FromStartOffset; 
                                i < ToStartOffset + CopySizeBytes; i++, j++) {
            if (__isMemControllerActive)
                sem_wait(__sem_lock);
            __BaseStorageLocation.get()[i] 
                = From->GetStorageLocation().get()[j];

            if (__isMemControllerActive)
                sem_post(__sem_lock);
        }
    }

}
