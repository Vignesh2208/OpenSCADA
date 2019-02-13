#ifndef __PC_EMULATOR_INCLUDE_PC_DATATYPE_H__
#define __PC_EMULATOR_INCLUDE_PC_DATATYPE_H__

#include <iostream>
#include <vector>
#include <climits>
#include <cstdlib>
#include <assert.h>
#include <unordered_map>

#include "src/pc_emulator/include/elementary_datatypes.h"
#include "src/pc_emulator/proto/configuration.pb.h"

using namespace std;
using namespace pc_specification;

using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldInterfaceType = pc_specification::FieldInterfaceType;



typedef long long int s64;

namespace pc_emulator {
    class PCConfiguration;
    class PCDataType;

    class PCDataTypeField {
        public:
            string __FieldName;
            string __FieldTypeName;
            int __FieldInterfaceType;
            DataTypeCategory __FieldTypeCategory;
            s64 __RangeMin, __RangeMax;
            string __InitialValue; 
            PCDataType * __FieldTypePtr;
            int __StorageMemType;
            int __StorageByteOffset;
            int __StorageBitOffset;
            // These are set only if __FieldTypeCategory is an ARRAY
            int __NDimensions;
            int __Dimension1;
            int __Dimension2;

        PCDataTypeField() : __FieldName(""), __FieldTypeName(""),
                        __RangeMin(0), __RangeMax(0),
                        __InitialValue(""), 
                        __FieldTypeCategory(static_cast<DataTypeCategory>(0)),
                        __FieldInterfaceType(-1),
                        __FieldTypePtr(nullptr), __StorageMemType(-1), 
                        __StorageByteOffset(-1), __StorageBitOffset(-1),
                        __NDimensions(-1), __Dimension1(-1),
                        __Dimension2(-1) {};      

        PCDataTypeField(string FieldName, string FieldTypeName,
                        DataTypeCategory FieldTypeCategory,
                        s64 RangeMin, s64 RangeMax, string InitialValue,
                        int FieldInterfaceType, PCDataType * FieldTypePtr)
            : __FieldName(FieldName), __FieldTypeName(FieldTypeName), 
            __RangeMin(RangeMin), __RangeMax(RangeMax), 
            __InitialValue(InitialValue), __FieldTypeCategory(FieldTypeCategory),
            __FieldInterfaceType(FieldInterfaceType),
            __FieldTypePtr(FieldTypePtr), __StorageMemType(-1), 
            __StorageByteOffset(-1), __StorageBitOffset(-1),
            __NDimensions(-1), __Dimension1(-1),
            __Dimension2(-1) {};

        void SetExplicitStorageConstraints(int MemType, int ByteOffset,
                                            int BitOffset);
    };

    class PCDataType {
        private:
            bool CheckRemFields(std::vector<string>& NestedFields, int StartPos,
                            PCDataType * Current);
            bool CheckRemFields(std::vector<string>& NestedFields, int StartPos,
                            PCDataType * Current, PCDataTypeField& Result);
            void SetElementaryDataTypeAttributes(string InitialValue,
                                            s64 RangeMin, s64 RangeMax);
            bool GetPCDataTypeFieldOfArrayElement(
                PCDataTypeField& DefinedField,
                PCDataTypeField& Result, int idx1, int idx2=-1);
        
        public:
            string __AliasName;
            string __DataTypeName;
            PCConfiguration * __configuration;
            unordered_map<int, std::vector<PCDataTypeField>> __FieldsByInterfaceType;
            DataTypeCategory __DataTypeCategory;
            int __PoUType;
            int __SizeInBits;
            int __NFields;
            s64 __RangeMin, __RangeMax;
            string __InitialValue;
            std::vector<int> __DimensionSizes;

        bool IsFieldPresent(string NestedFieldName);

        PCDataType* LookupDataType(string DataTypeName);

        void AddDataTypeField(string FieldName, string FieldTypeName,
            string InitialValue, int FieldInterfaceType, 
            s64 RangeMin, s64 RangeMax);

        void AddArrayDataTypeField(string FieldName, string FieldTypeName,
            int DimensionSize, string InitialValue, int FieldInterfaceType,
            s64 RangeMin, s64 RangeMax);
        
        void AddArrayDataTypeField(string FieldName, string FieldTypeName,
            int DimensionSize1, int DimesionSize2, string InitialValue,
            int FieldInterfaceType, s64 RangeMin, s64 RangeMax);

        void AddDataTypeField(string FieldName, string FieldTypeName,
            DataTypeCategory FieldTypeCategory, string InitialValue,
            int FieldInterfaceType, s64 RangeMin, s64 RangeMax);


        // For adding fields grounded at specified locations
        void AddDataTypeFieldAT(string FieldName, string FieldTypeName,
            string InitialValue, s64 RangeMin, s64 RangeMax,
            int MemType, int ByteOffset, int BitOffset);

        void AddDataTypeFieldAT(string FieldName, string FieldTypeName,
            DataTypeCategory FieldTypeCategory, string InitialValue,
            s64 RangeMin, s64 RangeMax, int MemType, int ByteOffset,
            int BitOffset);

        void AddArrayDataTypeFieldAT(string FieldName, string FieldTypeName,
            int DimensionSize, string InitialValue, s64 RangeMin, s64 RangeMax,
            int MemType, int ByteOffset, int BitOffset);
        
        void AddArrayDataTypeFieldAT(string FieldName, string FieldTypeName,
            int DimensionSize1, int DimesionSize2, string InitialValue,
            s64 RangeMin, s64 RangeMax, int MemType, int ByteOffset,
            int BitOffset);


        
        // Non-Array
        PCDataType(PCConfiguration* configuration, 
                    string AliasName, string DataTypeName,
                    DataTypeCategory Category = DataTypeCategory::NOT_ASSIGNED,
                    string InitialValue="", s64 RangeMin = LLONG_MIN,
                    s64 RangeMax = LLONG_MAX);

        // 1-D Array
        PCDataType(PCConfiguration* configuration, 
                    string AliasName, string DataTypeName, int DimSize, 
                    DataTypeCategory Category,
                    string InitialValue="", s64 RangeMin = LLONG_MIN,
                    s64 RangeMax = LLONG_MAX);
        // 2-D Array
        PCDataType(PCConfiguration* configuration, 
                    string AliasName, string DataTypeName,
                    int Dim1Size, int Dim2Size, 
                    DataTypeCategory Category,
                    string InitialValue="", 
                    s64 RangeMin = LLONG_MIN, s64 RangeMax = LLONG_MAX);

        bool GetPCDataTypeField(string NestedFieldName,
                                    PCDataTypeField& Result);
        void Cleanup();
    };


    class DataTypeUtils {
        public:

            
            static bool ValueToBool(string Value, bool& BoolValue);
            static bool ValueToByte(string Value, uint8_t & ByteValue);
            static bool ValueToWord(string Value, uint16_t & WordValue);
            static bool ValueToDWord(string Value, uint32_t& DWordValue);
            static bool ValueToLWord(string Value, uint64_t & LWordValue);
            static bool ValueToChar(string Value, char & CharValue);
            static bool ValueToInt(string Value, int16_t& IntValue);
            static bool ValueToSint(string Value, int8_t & SintValue);
            static bool ValueToDint(string Value, int32_t & DintValue);
            static bool ValueToLint(string Value, int64_t& LintValue);
            static bool ValueToUint(string Value, uint16_t & UintValue);
            static bool ValueToUsint(string Value, uint8_t & UsintValue);
            static bool ValueToUdint(string Value, uint32_t & UdintValue);
            static bool ValueToUlint(string Value, uint64_t & UlintValue);
            static bool ValueToReal(string Value, float & RealValue);
            static bool ValueToLReal(string Value, double & LRealValue);
            static bool ValueToTime(string Value, TimeType & Time);
            static bool ValueToTOD(string Value, TODType & TOD);
            static bool ValueToDT(string Value, DateTODDataType & Dt);
            static bool ValueToDate(string Value, DateType& Date);

    };
}
#endif