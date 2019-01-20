#ifndef __PC_EMULATOR_INCLUDE_PC_UTILS_H__
#define __PC_EMULATOR_INCLUDE_PC_UTILS_H__


#include <iostream>
#include <vector>
#include <climits>
#include <cstdlib>
#include <assert.h>
#include <unordered_map>
#include "pc_emulator/proto/configuration.pb.h"

using namespace std;
using namespace pc_specification;


namespace pc_emulator {

    class PCDataType;
    class PCVariable;
    class PCConfiguration;

    class Utils {

        public:
        static void InitializeDataType(PCConfiguration * __configuration,
                                PCDataType * __new_data_type,
                                const pc_specification::DataType& DataTypeSpec);

        static void InitializeAccessDataType(PCConfiguration * __configuration,
                                PCDataType * __new_data_type,
                                const pc_specification::DataType& DataTypeSpec);
        static bool ExtractFromStorageSpec(string StorageSpec, 
                                            int * MemType, int * ByteOffset,
                                            int * BitOffset);

        static bool ExtractFromAccessStorageSpec(
            PCConfiguration * __configuration, string StorageSpec, 
                                            int * MemType, int * ByteOffset,
                                            int * BitOffset);
    };
}   

#endif