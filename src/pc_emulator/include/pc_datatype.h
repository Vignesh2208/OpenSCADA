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



typedef long long s64;

namespace pc_emulator {
    class PCConfiguration;
    class PCDataType;

    //! Class to hold information about a data type field
    class PCDataTypeField {
        public:
            string __FieldName; /*!< Name of the field */
            string __FieldTypeName; /*!< Name of the data type of the field */
            int __FieldInterfaceType;   /*!< Interface type of field (pc_specification::FieldIntfType) */
            DataTypeCategory __FieldTypeCategory; /*!< Category of the data type of the field */
            s64 __RangeMin, __RangeMax; /*!< Max value of the field. Only valid for numeric fields */
            string __InitialValue; /*!< Initial value of the field */
            PCDataType * __FieldTypePtr;    /*!< DataType pointer for the field's data type */
            int __FieldQualifier;   /*!< Qualifier associated with the field: R_EDGE, F_EDGE, READ_ONLY
                READ_WRITE or NONE (pc_specification::FieldQualifiers)*/
            string __AssociatedResourceName; /*!< ResourceName associated with the field.
                Only valid for directly rep fields defined over INPUT and OUTPUT memory */
            int __StorageMemType;   /*!< Storage memory type. Only valid for directly rep fields */
            int __StorageByteOffset; /*!< Byte offset within the memory. Only valid for directly rep fields */
            int __StorageBitOffset; /*!< Bit offset within the byte. Only valid for directly rep field */
            string __FullStorageSpec; /*!< The storage spec (MemType, Byte offset and Bit offset)
                stored as a string */
            string __HoldingPoUType; /*!< used internally for validation checks */
            
            int __NDimensions; /*!< Number of dimensions. Only valied if FieldTypeCategory is ARRAY */
            int __Dimension1; /*!< Size of first dimension. Only valied if FieldTypeCategory is ARRAY */
            int __Dimension2; /*!< Size of second dimension. Only valied if FieldTypeCategory is ARRAY */
             

        //! Default constructor
        PCDataTypeField() : __FieldName(""),
                        __FieldTypeName(""),
                        __RangeMin(0),
                        __RangeMax(0),
                        __InitialValue(""), 
                        __FieldTypeCategory(static_cast<DataTypeCategory>(0)),
                        __FieldInterfaceType(FieldInterfaceType::NA),
                        __FieldTypePtr(nullptr),
                        __FieldQualifier(FieldQualifiers::NONE),
                        __AssociatedResourceName("NONE"),
                        __StorageMemType(-1), 
                        __StorageByteOffset(-1),
                        __StorageBitOffset(-1),
                        __NDimensions(-1), __Dimension1(-1),
                        __Dimension2(-1),
                        __FullStorageSpec("NONE"),
                        __HoldingPoUType("__NONE__") {};      

        //! Constructor
        PCDataTypeField(string FieldName,
                        string FieldTypeName,
                        DataTypeCategory FieldTypeCategory,
                        int FieldQualifier,
                        s64 RangeMin,
                        s64 RangeMax,
                        string InitialValue,
                        int FieldInterfaceType,
                        PCDataType * FieldTypePtr,
                        string ResourceName="NONE",
                        string FullStorageSpec="NONE")

            : __FieldName(FieldName),
            __FieldTypeName(FieldTypeName), 
            __RangeMin(RangeMin),
            __RangeMax(RangeMax), 
            __InitialValue(InitialValue),
            __FieldTypeCategory(FieldTypeCategory),
            __FieldInterfaceType(FieldInterfaceType),
            __FieldQualifier(FieldQualifier),
            __FieldTypePtr(FieldTypePtr),
            __AssociatedResourceName(ResourceName),
            __StorageMemType(-1), 
            __StorageByteOffset(-1),
            __StorageBitOffset(-1),
            __FullStorageSpec(FullStorageSpec),
            __NDimensions(-1),
            __Dimension1(-1),
            __Dimension2(-1),
            __HoldingPoUType("__NONE__") {};

        //! Sets the field storage information
        void SetExplicitStorageConstraints(int MemType, int ByteOffset,
                                            int BitOffset);
        //! Deep copy of two objects of this class
        void Copy(PCDataTypeField& V) {
            __FieldName = V.__FieldName;
            __FieldTypeName = V.__FieldTypeName; 
            __RangeMin = V.__RangeMin;
            __RangeMax = V.__RangeMax;
            __InitialValue = V.__InitialValue;
            __FieldTypeCategory = V.__FieldTypeCategory;
            __FieldInterfaceType = V.__FieldInterfaceType;
            __FieldQualifier = V.__FieldQualifier;
            __FieldTypePtr = V.__FieldTypePtr;
            __StorageMemType = V.__StorageMemType;
            __StorageByteOffset = V.__StorageByteOffset;
            __StorageBitOffset = V.__StorageBitOffset;
            __NDimensions = V.__NDimensions;
            __Dimension1 = V.__Dimension1;
            __Dimension2 = V.__Dimension2;
        };
    };

    //! Class which describes a PCDataType
    class PCDataType {
        private:

            //! Checks if a field exists in this data type
            /*!
                \param NestedFields a list of fields
                \param StartPos     a particular field described by this index
                \param Current  The data type to check in
                \return True if the field is present, else False
            */
            bool CheckRemFields(std::vector<string>& NestedFields,
                            int StartPos,
                            PCDataType * Current);

            //! Checks if a field exists in this data type
            /*!
                \param NestedFields a list of fields
                \param StartPos     a particular field described by this index
                \param Current  The data type to check in
                \param Result   Stores details of the field in this parameter
                \return True if the field is present, else False
            */
            bool CheckRemFields(std::vector<string>& NestedFields,
                            int StartPos,
                            PCDataType * Current,
                            PCDataTypeField& Result);

            //! Sets Initial Value, RangeMin and RangeMax if this data type is elementary
            void SetElementaryDataTypeAttributes(string InitialValue,
                                            s64 RangeMin,
                                            s64 RangeMax);

            //! Used internally to get Field details of an element of an array
            /*!
                \param DefinedField Field whose datatype category is ARRAY
                \param idx1 index of the interested array element
                \param idx2 If DefinedField is a 2-D array, then this value defines
                    the second index
                \param Result   Field details of the interested array element. This
                    is returned.
                \return True upon success
            */
            bool GetPCDataTypeFieldOfArrayElement(
                PCDataTypeField& DefinedField,
                PCDataTypeField& Result,
                int idx1,
                int idx2=-1);
        
        public:
            string __AliasName; /*!< Stored for datatypes which are type defs of others */
            string __DataTypeName;  /*!< The resolved data type name. Will be different
                from AliasName only for typedeffed data types */
            PCConfiguration * __configuration; /*!< Associated configuration */
            unordered_map<int, std::vector<PCDataTypeField>> 
            __FieldsByInterfaceType; /*!< Hashmap of fields organized by interface type */
            DataTypeCategory __DataTypeCategory; /*!< Category of the data type */
            int __PoUType;  /*!< If the data type category is POU, describes whether
                it is a FB, FC or a PROGRAM */
            int __SizeInBits;   /*!< Total size of the data type */
            int __NFields;  /*!< Number of fields in the data type */
            s64 __RangeMin, __RangeMax; /*!< Min and max ranges. Only valid for numeric
                data types */
            string __InitialValue; /*!< Initial value for the data type */
            std::vector<int> __DimensionSizes; /*!< Length of dimensions if the
                data type category is ARRAY. Only 1-D or 2-D arrays are supported */

        //! Returns true if the nested field is present in this data type
        bool IsFieldPresent(string NestedFieldName);

        //! Returns the data type pointer with the specified name
        PCDataType* LookupDataType(string DataTypeName);

        //! Adds a new field to the data type
        /*!
            \param FieldName    Name of the field
            \param FieldTypeName    Data type name of the field
            \param InitialValue     Initial value assigned to the field in this data type
            \param FieldInterfaceType   InterfaceType of the field. Cannot be VAR_EXPLICIT_STORAGE
            \param FieldQualifier   Qualifier of type pc_specification::FieldQualifiers
            \param RangeMin     Min value of this field in an instantiation of the data type
            \param RangeMax     Max value of this field in an instantiation of the data type
            \param FullStorageSpec  Relevant only if FieldInterfaceType is ACCESS. Helps with book keeping
        */
        void AddDataTypeField(string FieldName,
            string FieldTypeName,
            string InitialValue,
            int FieldInterfaceType, 
            int FieldQualifier,
            s64 RangeMin,
            s64 RangeMax,
            string FullStorageSpec="NONE");

        //! Adds a new 1-D array field to the data type
        /*!
            \param FieldName    Name of the field
            \param FieldTypeName    Data type name of the field
            \param DimensionSize    Length of the 1-D array
            \param InitialValue     Initial value assigned to the field in this data type
            \param FieldInterfaceType   InterfaceType of the field. Cannot be VAR_EXPLICIT_STORAGE
            \param FieldQualifier   Qualifier of type pc_specification::FieldQualifiers
            \param RangeMin     Min value of this field in an instantiation of the data type
            \param RangeMax     Max value of this field in an instantiation of the data type
            \param FullStorageSpec  Relevant only if FieldInterfaceType is ACCESS. Helps with book keeping
        */
        void AddArrayDataTypeField(string FieldName,
            string FieldTypeName,
            int DimensionSize,
            string InitialValue,
            int FieldInterfaceType,
            int FieldQualifier,
            s64 RangeMin,
            s64 RangeMax,
            string FullStorageSpec="NONE");
        
        //! Adds a new 2-D array field to the data type
        /*!
            \param FieldName    Name of the field
            \param FieldTypeName    Data type name of the field
            \param DimensionSize1    Length of the first dimension of the array
            \param DimensionSize2   Length of the second dimension of the array
            \param InitialValue     Initial value assigned to the field in this data type
            \param FieldInterfaceType   InterfaceType of the field. Cannot be VAR_EXPLICIT_STORAGE
            \param FieldQualifier   Qualifier of type pc_specification::FieldQualifiers
            \param RangeMin     Min value of this field in an instantiation of the data type
            \param RangeMax     Max value of this field in an instantiation of the data type
            \param FullStorageSpec  Relevant only if FieldInterfaceType is ACCESS. Helps with book keeping
        */
        void AddArrayDataTypeField(string FieldName,
            string FieldTypeName,
            int DimensionSize1,
            int DimesionSize2,
            string InitialValue,
            int FieldInterfaceType,
            int FieldQualifier,
            s64 RangeMin,
            s64 RangeMax,
            string FullStorageSpec="NONE");



        //! Adds a new field to the data type
        /*!
            \param FieldName    Name of the field
            \param FieldTypeName    Data type name of the field
            \param FieldTypeCategory    Category of the data type of the field
            \param InitialValue     Initial value assigned to the field in this data type
            \param FieldInterfaceType   InterfaceType of the field. Cannot be VAR_EXPLICIT_STORAGE
            \param FieldQualifier   Qualifier of type pc_specification::FieldQualifiers
            \param RangeMin     Min value of this field in an instantiation of the data type
            \param RangeMax     Max value of this field in an instantiation of the data type
            \param FullStorageSpec  Relevant only if FieldInterfaceType is ACCESS. Helps with book keeping
        */
        void AddDataTypeField(string FieldName,
            string FieldTypeName,
            DataTypeCategory FieldTypeCategory,
            string InitialValue,
            int FieldInterfaceType,
            int FieldQualifier,
            s64 RangeMin,
            s64 RangeMax,
            string FullStorageSpec="NONE");


        
        //! Adds a new directly represented field to the data type
        /*!
            \param FieldName    Name of the field
            \param FieldTypeName    Data type name of the field
            \param InitialValue     Initial value assigned to the field in this data type
            \param FieldInterfaceType   InterfaceType of the field. Cannot be VAR_EXPLICIT_STORAGE
            \param FieldQualifier   Qualifier of type pc_specification::FieldQualifiers
            \param RangeMin     Min value of this field in an instantiation of the data type
            \param RangeMax     Max value of this field in an instantiation of the data type
            \param MemType Type of the memory
            \param ByteOffset   Offset within the memory
            \param BitOffset    Offset bit within the byte
            \param ResourceName  Only relevant if MemType is INPUT or OUTPUT. Only this specific
                resource's input/output memory would be considered at the time of initialization
            \param FullStorageSpec  Relevant only if FieldInterfaceType is ACCESS. Helps with book keeping
        */
        void AddDataTypeFieldAT(string FieldName,
            string FieldTypeName,
            string InitialValue,
            int FieldQualifier,
            s64 RangeMin,
            s64 RangeMax,
            int MemType,
            int ByteOffset,
            int BitOffset,
            string ResourceName="NONE",
            string FullStorageSpec="NONE");

        //! Adds a new directly represented field to the data type
        /*!
            \param FieldName    Name of the field
            \param FieldTypeName    Data type name of the field
            \param FieldTypeCategory    Data type category of the field's data type
            \param InitialValue     Initial value assigned to the field in this data type
            \param FieldInterfaceType   InterfaceType of the field. Cannot be VAR_EXPLICIT_STORAGE
            \param FieldQualifier   Qualifier of type pc_specification::FieldQualifiers
            \param RangeMin     Min value of this field in an instantiation of the data type
            \param RangeMax     Max value of this field in an instantiation of the data type
            \param MemType Type of the memory
            \param ByteOffset   Offset within the memory
            \param BitOffset    Offset bit within the byte
            \param ResourceName  Only relevant if MemType is INPUT or OUTPUT. Only this specific
                resource's input/output memory would be considered at the time of initialization
            \param FullStorageSpec  Relevant only if FieldInterfaceType is ACCESS. Helps with book keeping
        */
        void AddDataTypeFieldAT(string FieldName,
            string FieldTypeName,
            DataTypeCategory FieldTypeCategory,
            string InitialValue,
            int FieldQualifier,
            s64 RangeMin,
            s64 RangeMax,
            int MemType,
            int ByteOffset,
            int BitOffset,
            string ResourceName="NONE",
            string FullStorageSpec="NONE");


        //! Adds a new directly represented 1-D array to the data type
        /*!
            \param FieldName    Name of the field
            \param FieldTypeName    Data type name of the field
            \param DimensionSize    Length of the 1-D array
            \param InitialValue     Initial value assigned to the field in this data type
            \param FieldInterfaceType   InterfaceType of the field. Cannot be VAR_EXPLICIT_STORAGE
            \param FieldQualifier   Qualifier of type pc_specification::FieldQualifiers
            \param RangeMin     Min value of this field in an instantiation of the data type
            \param RangeMax     Max value of this field in an instantiation of the data type
            \param MemType Type of the memory
            \param ByteOffset   Offset within the memory
            \param BitOffset    Offset bit within the byte
            \param ResourceName  Only relevant if MemType is INPUT or OUTPUT. Only this specific
                resource's input/output memory would be considered at the time of initialization
            \param FullStorageSpec  Relevant only if FieldInterfaceType is ACCESS. Helps with book keeping
        */
        void AddArrayDataTypeFieldAT(string FieldName,
            string FieldTypeName,
            int DimensionSize,
            string InitialValue,
            int FieldQualifier,
            s64 RangeMin,
            s64 RangeMax,
            int MemType,
            int ByteOffset,
            int BitOffset,
            string ResourceName="NONE",
            string FullStorageSpec="NONE");


        //! Adds a new directly represented 2-D array to the data type
        /*!
            \param FieldName    Name of the field
            \param FieldTypeName    Data type name of the field
            \param DimensionSize1    Length of the first dimension of the array
            \param DimensionSize2   Length of the second dimension of the array
            \param InitialValue     Initial value assigned to the field in this data type
            \param FieldInterfaceType   InterfaceType of the field. Cannot be VAR_EXPLICIT_STORAGE
            \param FieldQualifier   Qualifier of type pc_specification::FieldQualifiers
            \param RangeMin     Min value of this field in an instantiation of the data type
            \param RangeMax     Max value of this field in an instantiation of the data type
            \param MemType Type of the memory
            \param ByteOffset   Offset within the memory
            \param BitOffset    Offset bit within the byte
            \param ResourceName  Only relevant if MemType is INPUT or OUTPUT. Only this specific
                resource's input/output memory would be considered at the time of initialization
            \param FullStorageSpec  Relevant only if FieldInterfaceType is ACCESS. Helps with book keeping
        */
        void AddArrayDataTypeFieldAT(string FieldName,
            string FieldTypeName,
            int DimensionSize1,
            int DimesionSize2,
            string InitialValue,
            int FieldQualifier,
            s64 RangeMin,
            s64 RangeMax,
            int MemType,
            int ByteOffset,
            int BitOffset,
            string ResourceName="NONE",
            string FullStorageSpec="NONE");


        
        //! Constructor to initialize a non-array data type
        /*!
            \param configuration    Associated configuration
            \param AliasName    For a type-def data type this is the new name
            \param DataTypeName Name of the data type. Equals alias name unless
                the data type is a type def of a previously defined data type
            \param Category Data type category
            \param InitialValue initial value assigned to the data type
            \param RangeMin Minimum range of data type values. Only valid for numeric data types
            \param RangeMax Maximum range of data type values. Only valid for numeric data types
        */
        PCDataType(PCConfiguration* configuration, 
                    string AliasName,
                    string DataTypeName,
                    DataTypeCategory Category = DataTypeCategory::NOT_ASSIGNED,
                    string InitialValue="",
                    s64 RangeMin = LLONG_MIN,
                    s64 RangeMax = LLONG_MAX);

        //! Constructor to initialize a 1-D array data type
        /*!
            \param configuration    Associated configuration
            \param AliasName    For a type-def data type this is the new name
            \param DataTypeName Name of the data type. Equals alias name unless
                the data type is a type def of a previously defined data type
            \param DimSize  Length of the 1-D array
            \param Category Data type category (should be ARRAY)
            \param InitialValue initial value assigned to the data type
            \param RangeMin Minimum range of data type values. Only valid for numeric data types
            \param RangeMax Maximum range of data type values. Only valid for numeric data types
        */
        PCDataType(PCConfiguration* configuration, 
                    string AliasName,
                    string DataTypeName,
                    int DimSize, 
                    DataTypeCategory Category,
                    string InitialValue="",
                    s64 RangeMin = LLONG_MIN,
                    s64 RangeMax = LLONG_MAX);

        //! Constructor to initialize a 2-D array data type
        /*!
            \param configuration    Associated configuration
            \param AliasName    For a type-def data type this is the new name
            \param DataTypeName Name of the data type. Equals alias name unless
                the data type is a type def of a previously defined data type
            \param Dim1Size     Size of dimension1 of the array
            \param Dim2Size     Size of dimension2 of the array
            \param Category Data type category (should be ARRAY)
            \param InitialValue initial value assigned to the data type
            \param RangeMin Minimum range of data type values. Only valid for numeric data types
            \param RangeMax Maximum range of data type values. Only valid for numeric data types
        */
        PCDataType(PCConfiguration* configuration, 
                    string AliasName,
                    string DataTypeName,
                    int Dim1Size,
                    int Dim2Size, 
                    DataTypeCategory Category,
                    string InitialValue="", 
                    s64 RangeMin = LLONG_MIN,
                    s64 RangeMax = LLONG_MAX);

        //! Gets field details given a nested field name
        /*!
            \param NestedFieldName      Name of the nested field to query
            \param  Result      Returned field details
            \return True if nested field name exists
        */
        bool GetPCDataTypeField(string NestedFieldName,
                                PCDataTypeField& Result);

        //! Clean up temporarily allocated memory
        void Cleanup();
    };

    //! Util class for converting passed strings to elementary data types
    class DataTypeUtils {
        public:

            //! Convert passed value to bool. Return True on success
            static bool ValueToBool(string Value, bool& BoolValue);

            //! Convert passed value to byte. Return True on success
            static bool ValueToByte(string Value, uint8_t & ByteValue);

            //! Convert passed value to word. Return True on success
            static bool ValueToWord(string Value, uint16_t & WordValue);

            //! Convert passed value to dword. Return True on success
            static bool ValueToDWord(string Value, uint32_t& DWordValue);

            //! Convert passed value to lword. Return True on success
            static bool ValueToLWord(string Value, uint64_t & LWordValue);

            //! Convert passed value to char. Return True on success
            static bool ValueToChar(string Value, char & CharValue);

            //! Convert passed value to int. Return True on success
            static bool ValueToInt(string Value, int16_t& IntValue);

            //! Convert passed value to sint. Return True on success
            static bool ValueToSint(string Value, int8_t & SintValue);

            //! Convert passed value to dint. Return True on success
            static bool ValueToDint(string Value, int32_t & DintValue);

            //! Convert passed value to lint. Return True on success
            static bool ValueToLint(string Value, int64_t& LintValue);

            //! Convert passed value to uint. Return True on success
            static bool ValueToUint(string Value, uint16_t & UintValue);

            //! Convert passed value to usint. Return True on success
            static bool ValueToUsint(string Value, uint8_t & UsintValue);

            //! Convert passed value to udint. Return True on success
            static bool ValueToUdint(string Value, uint32_t & UdintValue);

            //! Convert passed value to ulint. Return True on success
            static bool ValueToUlint(string Value, uint64_t & UlintValue);

            //! Convert passed value to real. Return True on success
            static bool ValueToReal(string Value, float & RealValue);

            //! Convert passed value to lreal. Return True on success
            static bool ValueToLReal(string Value, double & LRealValue);

            //! Convert passed value to time. Return True on success
            static bool ValueToTime(string Value, TimeType & Time);

            //! Convert passed value to time of day. Return True on success
            static bool ValueToTOD(string Value, TODType & TOD);

            //! Convert passed value to date and time. Return True on success
            static bool ValueToDT(string Value, DateTODDataType & Dt);

            //! Convert passed value to date. Return True on success
            static bool ValueToDate(string Value, DateType& Date);


            //! Convert passed bool Value to a equivalent string description of specified data type category
            static bool BoolToAny(bool Value, int DataTypeCategory,
                    string& Result);

            //! Convert passed byte Value to a equivalent string description of specified data type category
            static bool ByteToAny(uint8_t Value, int DataTypeCategory,
                    string& Result);

            //! Convert passed word Value to a equivalent string description of specified data type category
            static bool WordToAny(uint16_t Value, int DataTypeCategory,
                    string& Result);

            //! Convert passed dword Value to a equivalent string description of specified data type category
            static bool DWordToAny(uint32_t Value, int DataTypeCategory,
                    string& Result);

            //! Convert passed lword Value to a equivalent string description of specified data type category
            static bool LWordToAny(uint64_t Value, int DataTypeCategory,
                    string& Result);

            //! Convert passed char Value to a equivalent string description of specified data type category
            static bool CharToAny(char Value, int DataTypeCategory,
                    string& Result);

            //! Convert passed int Value to a equivalent string description of specified data type category
            static bool IntToAny(int16_t Value, int DataTypeCategory,
                    string& Result);

            //! Convert passed sint Value to a equivalent string description of specified data type category
            static bool SIntToAny(int8_t Value, int DataTypeCategory,
                    string& Result);

            //! Convert passed dint Value to a equivalent string description of specified data type category
            static bool DIntToAny(int32_t Value, int DataTypeCategory,
                    string& Result);

            //! Convert passed lint Value to a equivalent string description of specified data type category
            static bool LIntToAny(int64_t Value, int DataTypeCategory,
                    string& Result);

           //! Convert passed uint Value to a equivalent string description of specified data type category
            static bool UIntToAny(uint16_t Value, int DataTypeCategory,
                    string& Result);

            //! Convert passed usint Value to a equivalent string description of specified data type category
            static bool USintToAny(uint8_t Value, int DataTypeCategory,
                    string& Result);

            //! Convert passed udint Value to a equivalent string description of specified data type category
            static bool UDintToAny(uint32_t Value, int DataTypeCategory,
                    string& Result);

            //! Convert passed ulint Value to a equivalent string description of specified data type category
            static bool UlintToAny(uint64_t Value, int DataTypeCategory,
                    string& Result);

            //! Convert passed real Value to a equivalent string description of specified data type category
            static bool RealToAny(float Value, int DataTypeCategory,
                    string& Result);

            //! Convert passed lreal Value to a equivalent string description of specified data type category
            static bool LRealToAny(double Value, int DataTypeCategory,
                    string& Result);

            static bool TimeToAny(TimeType time_value, int DataTypeCategory,
                    string& Result);

            //! Converts a DateTime object into a DT string
            static string DTToDTString(DateTODDataType& dt);

            //! Converts a Date object into a DT string
            static string DateToDTString(DateDataType& date1);

            //! Adds time with a date time object
            static void AddToDT(DateTODDataType& Dt, TimeType& Time);

            //! Adds time with a time of day object
            static void AddToTOD(TODDataType& tod, TimeType& Time);

            //! Subtracts time from a date time object
            static void SubFromDT(DateTODDataType& Dt, TimeType& Time);

            //! Subtracts time from a time of day object
            static void SubFromTOD(TODDataType& tod, TimeType& Time);

            //! Subtracts two date time objects and returns a time object
            static TimeType SubDTs(DateTODDataType& Dt1,
                    DateTODDataType& Dt2);

            //! Subtracts two time of day objects and returns a time object
            static TimeType SubTODs(TODDataType& tod1,
                    TODDataType& tod2);
        
            //! Subtracts two date objects and returns a time object
            static TimeType SubDATEs(DateDataType& date1, DateDataType& date2);
    };
}
#endif
