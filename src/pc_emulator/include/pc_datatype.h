#ifndef __PC_EMULATOR_INCLUDE_PC_DATATYPE_H__
#define __PC_EMULATOR_INCLUDE_PC_DATATYPE_H__

#include <iostream>
#include <vector>
#include <climits>
#include <cstdlib>
#include <assert.h>
#include <unordered_map>

using namespace std;

typedef long long int s64;

namespace pc_emulator {
    class PCConfiguration;
    class PCDataType;

    struct DateDataType {
        int Year;
        int Month;
        int Day;

        bool operator>(const DateDataType& a) {
            if (Year > a.Year)
                return true;
            if (Year == a.Year && Month > a.Month)
                return true;
            if (Year == a.Year && Month == a.Month && Day > a.Day)
                return true;
            return false;
        }

        bool operator>=(const DateDataType& a) {
            if (Year > a.Year)
                return true;
            if (Year == a.Year && Month > a.Month)
                return true;
            if (Year == a.Year && Month == a.Month && Day >= a.Day)
                return true;
            return false;
        }

        bool operator<(const DateDataType& a) {
            if (Year < a.Year)
                return true;
            if (Year == a.Year && Month < a.Month)
                return true;
            if (Year == a.Year && Month == a.Month && Day < a.Day)
                return true;
            return false;
        }

        bool operator<=(const DateDataType& a) {
            if (Year < a.Year)
                return true;
            if (Year == a.Year && Month < a.Month)
                return true;
            if (Year == a.Year && Month == a.Month && Day <= a.Day)
                return true;
            return false;
        }

        bool operator==(const DateDataType& a) {
            if (Year == a.Year && Month == a.Month && Day == a.Day)
                return true;
            return false;
        }

        void operator+(const DateDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator-(const DateDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator/(const DateDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator%(const DateDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator*(const DateDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator&(const DateDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator|(const DateDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator^(const DateDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator<<(const DateDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator>>(const DateDataType& a) {
            assert(false); // Not implemented, Raise error!
        }
    };

    typedef struct DateDataType DateType;

    struct TODDataType {
        int Hr;
        int Min;
        int Sec;

        bool operator>(const TODDataType& a) {
            auto tot1_secs = Hr*3600 + Min*60 + Sec;
            auto tot2_secs = a.Hr*3600 + a.Min*60 + a.Sec;

            return tot1_secs > tot2_secs;
        }

        bool operator>=(const TODDataType& a) {
            auto tot1_secs = Hr*3600 + Min*60 + Sec;
            auto tot2_secs = a.Hr*3600 + a.Min*60 + a.Sec;

            return tot1_secs > tot2_secs;
        }

        bool operator<(const TODDataType& a) {
            auto tot1_secs = Hr*3600 + Min*60 + Sec;
            auto tot2_secs = a.Hr*3600 + a.Min*60 + a.Sec;

            return tot1_secs < tot2_secs;
        }

        bool operator<=(const TODDataType& a) {
            auto tot1_secs = Hr*3600 + Min*60 + Sec;
            auto tot2_secs = a.Hr*3600 + a.Min*60 + a.Sec;

            return tot1_secs <= tot2_secs;
        }

        bool operator==(const TODDataType& a) {
            auto tot1_secs = Hr*3600 + Min*60 + Sec;
            auto tot2_secs = a.Hr*3600 + a.Min*60 + a.Sec;

            return tot1_secs > tot2_secs;
        }

        TODDataType& operator+(const TODDataType& a) {
            auto tot2_secs = a.Hr*3600 + a.Min*60 + a.Sec;
            auto tot1_secs = Hr*3600 + Min*60 + Sec;
            tot1_secs += tot2_secs;
            Hr = tot1_secs / 3600;
            Min = (tot1_secs - Hr*3600)/60;
            Sec = (tot1_secs - Hr*3600 - Min*60);
            return *this;
        }

        TODDataType& operator-(const TODDataType& a) {
            auto tot2_secs = a.Hr*3600 + a.Min*60 + a.Sec;
            auto tot1_secs = Hr*3600 + Min*60 + Sec;
            tot1_secs -= tot2_secs;
            Hr = tot1_secs / 3600;
            Min = (tot1_secs - Hr*3600)/60;
            Sec = (tot1_secs - Hr*3600 - Min*60);
            return *this;
        }

        void operator/(const TODDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator%(const TODDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator*(const TODDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator&(const TODDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator|(const TODDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator^(const TODDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator<<(const TODDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator>>(const TODDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

    } TODType;

    typedef struct TODDataType TODType;

    struct DateTODDataType {
        DateType Date;
        TODType Tod;

        bool operator<(const DateTODDataType& a) {
            if (Date < a.Date)
                return true;
            if (Date == a.Date)
                return Tod < a.Tod;
            return false;
        }

        bool operator<=(const DateTODDataType& a) {
            if (Date < a.Date)
                return true;
            if (Date == a.Date)
                return Tod <= a.Tod;
            return false;
        }

        bool operator>(const DateTODDataType& a) {
            if (Date > a.Date)
                return true;
            if (Date == a.Date)
                return Tod > a.Tod;
            return false;
        }

        bool operator>=(const DateTODDataType& a) {
            if (Date > a.Date)
                return true;
            if (Date == a.Date)
                return Tod >= a.Tod;
            return false;
        }

        bool operator==(const DateTODDataType& a) {
            if (Date == a.Date)
                return Tod == a.Tod;
            return false;
        }

        void operator+(const DateTODDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator-(const DateTODDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator/(const DateTODDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator%(const DateTODDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator*(const DateTODDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator&(const DateTODDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator|(const DateTODDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator^(const DateTODDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator<<(const DateTODDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator>>(const DateTODDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

    } DateTODType;

    typedef struct DateTODDataType DateTODType;

    struct TIMEDataType {
        unsigned long SecsElapsed;

        bool operator>(const TIMEDataType& a) {
            return SecsElapsed > a.SecsElapsed;
        }

        bool operator>=(const TIMEDataType& a) {
            return SecsElapsed >= a.SecsElapsed;
        }

        bool operator<(const TIMEDataType& a) {
            return SecsElapsed < a.SecsElapsed;
        }

        bool operator<=(const TIMEDataType& a) {
            return SecsElapsed <= a.SecsElapsed;
        }

        bool operator==(const TIMEDataType& a) {
            return SecsElapsed == a.SecsElapsed;
        }

        TIMEDataType& operator+(const TIMEDataType& a) {
            SecsElapsed += a.SecsElapsed;
            return *this;
        }

        TIMEDataType& operator-(const TIMEDataType& a) {
            SecsElapsed -= a.SecsElapsed;
            return *this;
        }

        void operator/(const TIMEDataType& a) {
            assert(false);
        }

        void operator%(const TIMEDataType& a) {
            assert(false);
        }

        void operator*(const TIMEDataType& a) {
            assert(false);
        }

        void operator&(const TIMEDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator|(const TIMEDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        void operator^(const TIMEDataType& a) {
            assert(false); // Not implemented, Raise error!
        }

        TIMEDataType& operator<<(const int& a) {
            if(a)
                SecsElapsed = SecsElapsed << a;
            return *this;
        }

        TIMEDataType& operator>>(const int& a) {
            if(a)
                SecsElapsed = SecsElapsed >> a;
            return *this;
        }
        
    };

    typedef struct TIMEDataType TimeType;

    typedef struct DataTypeFieldAttributesStruct {
        unsigned long RelativeOffset;
        unsigned long SizeInBits;
        int FieldInterfaceType;
        PCDataType* FieldDataTypePtr;
        string NestedFieldName;
    } DataTypeFieldAttributes;

    enum DataTypeCategories {
        BOOL,
        BYTE,
        WORD,
        DWORD,
        LWORD,
        CHAR,
        INT,
        SINT,
        DINT,
        LINT,
        UINT,
        USINT,
        UDINT,
        ULINT,
        REAL,
        LREAL,
        TIME,
        DATE,
        TIME_OF_DAY,
        DATE_AND_TIME,
        ARRAY,
        DERIVED,
        POU,
        NOT_ASSIGNED
    };

    enum FIELD_INTERFACE_TYPES {
        VAR_INPUT,
        VAR_OUTPUT,
        VAR_IN_OUT,
        VAR,
        VAR_TEMP,
        VAR_EXTERNAL,
        VAR_GLOBAL,
        VAR_ACCESS,
        VAR_EXPLICIT_STORAGE,
        NA
    };

    enum MEM_TYPE {
        INPUT_MEM,
        OUTPUT_MEM,
        RAM_MEM
    };

    class PCDataTypeField {
        public:
            string __FieldName;
            string __FieldTypeName;
            int __FieldInterfaceType;
            DataTypeCategories __FieldTypeCategory;
            s64 __RangeMin, __RangeMax;
            string __InitialValue; 
            PCDataType * __FieldTypePtr;
            int __StorageMemType;
            int __StorageByteOffset;
            int __StorageBitOffset;      

        PCDataTypeField(string FieldName, string FieldTypeName,
                        DataTypeCategories FieldTypeCategory,
                        s64 RangeMin, s64 RangeMax, string InitialValue,
                        int FieldInterfaceType, PCDataType * FieldTypePtr)
            : __FieldName(FieldName), __FieldTypeName(FieldTypeName), 
            __RangeMin(RangeMin), __RangeMax(RangeMax), 
            __InitialValue(InitialValue), __FieldTypeCategory(FieldTypeCategory),
            __FieldInterfaceType(FieldInterfaceType),
            __FieldTypePtr(FieldTypePtr), __StorageMemType(-1), 
            __StorageByteOffset(-1), __StorageBitOffset(-1) {};

        void SetExplicitStorageConstraints(int MemType, int ByteOffset,
                                            int BitOffset);
    };

    class PCDataType {
        public:
            string __AliasName;
            string __DataTypeName;
            PCConfiguration * __configuration;
            unordered_map<int, std::vector<PCDataTypeField>> __FieldsByInterfaceType;
            DataTypeCategories __DataTypeCategory;
            int __SizeInBits;
            int __NFields;
            s64 __RangeMin, __RangeMax;
            string __InitialValue;
            std::vector<int> __DimensionSizes;

        void GetFieldAttributes(string NestedFieldName, 
                            DataTypeFieldAttributes& FieldAttributes);

        PCDataType* LookupDataType(string DataTypeName);

        // Data-Type has to be registered using the provided Alias Name
        void RegisterDataType();

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
            DataTypeCategories FieldTypeCategory, string InitialValue,
            int FieldInterfaceType, s64 RangeMin, s64 RangeMax);


        // For adding fields grounded at specified locations
        void AddDataTypeFieldAT(string FieldName, string FieldTypeName,
            string InitialValue, s64 RangeMin, s64 RangeMax,
            int MemType, int ByteOffset, int BitOffset);

        void AddDataTypeFieldAT(string FieldName, string FieldTypeName,
            DataTypeCategories FieldTypeCategory, string InitialValue,
            s64 RangeMin, s64 RangeMax, int MemType, int ByteOffset,
            int BitOffset);

        void AddArrayDataTypeFieldAT(string FieldName, string FieldTypeName,
            int DimensionSize, string InitialValue, s64 RangeMin, s64 RangeMax,
            int MemType, int ByteOffset, int BitOffset);
        
        void AddArrayDataTypeFieldAT(string FieldName, string FieldTypeName,
            int DimensionSize1, int DimesionSize2, string InitialValue,
            s64 RangeMin, s64 RangeMax, int MemType, int ByteOffset,
            int BitOffset);


        void SetElementaryDataTypeAttributes(string InitialValue,
                                            s64 RangeMin, s64 RangeMax);
        
        // Non-Array
        PCDataType(PCConfiguration* configuration, 
                    string AliasName, string DataTypeName,
                    DataTypeCategories Category = DataTypeCategories::NOT_ASSIGNED,
                    string InitialValue="", s64 RangeMin = LLONG_MIN,
                    s64 RangeMax = LLONG_MAX);

        // 1-D Array
        PCDataType(PCConfiguration* configuration, 
                    string AliasName, string DataTypeName, int DimSize, 
                    DataTypeCategories Category,
                    string InitialValue="", s64 RangeMin = LLONG_MIN,
                    s64 RangeMax = LLONG_MAX);
        // 2-D Array
        PCDataType(PCConfiguration* configuration, 
                    string AliasName, string DataTypeName,
                    int Dim1Size, int Dim2Size, 
                    DataTypeCategories Category,
                    string InitialValue="", 
                    s64 RangeMin = LLONG_MIN, s64 RangeMax = LLONG_MAX);
    };


    class DataTypeUtils {
        public:

            
            static bool ValueToBool(string Value, bool& BoolValue);
            static bool ValueToByte(string Value, int8_t & ByteValue);
            static bool ValueToWord(string Value, int16_t & WordValue);
            static bool ValueToDWord(string Value, int32_t& DWordValue);
            static bool ValueToLWord(string Value, int64_t & LWordValue);
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