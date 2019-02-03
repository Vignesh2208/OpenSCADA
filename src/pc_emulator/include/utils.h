#ifndef __PC_EMULATOR_INCLUDE_PC_UTILS_H__
#define __PC_EMULATOR_INCLUDE_PC_UTILS_H__


#include <iostream>
#include <vector>
#include <climits>
#include <cstdlib>
#include <assert.h>
#include <unordered_map>
#include "src/pc_emulator/proto/configuration.pb.h"


using namespace std;
using namespace pc_specification;

#define US_IN_MS 1000
#define US_IN_SEC 1000000
#define NS_IN_US 1000
#define NS_IN_MS 1000000
#define NS_IN_SEC 1000000000
#define MS_IN_SEC 1000

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

        static string GetElementaryDataTypeName(int DataTypeCategory);

        static int GetVarOpType(int varop);

        static bool ExtractFromAccessStorageSpec(
            PCConfiguration * __configuration, string StorageSpec, 
                                            int * MemType, int * ByteOffset,
                                            int * BitOffset);

        static string GetInstallationDirectory();

        static string ResolveAliasName(string AliasName,
                                PCConfiguration * __configuration);
    };
}   

#endif