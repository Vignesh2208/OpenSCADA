
#include <assert.h>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <regex>
#include <vector>
#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>

#include "src/pc_emulator/include/pc_datatype.h"
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/proto/configuration.pb.h"

using namespace std;
using namespace pc_emulator;
using namespace pc_specification;


using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldInterfaceType = pc_specification::FieldInterfaceType;


void PCDataTypeField::SetExplicitStorageConstraints(int memType,
                                                    int ByteOffset,
                                                    int BitOffset) {
    assert(memType == MemType::INPUT_MEM ||
            memType == MemType::OUTPUT_MEM ||
            memType == MemType::RAM_MEM);
    assert(ByteOffset > 0 && BitOffset >= 0 && BitOffset < 8);
    __StorageMemType = memType;
    __StorageBitOffset = BitOffset;
    __StorageByteOffset = ByteOffset;
    __FieldInterfaceType = FieldInterfaceType::VAR_EXPLICIT_STORAGE;

}

void PCDataType::Cleanup() {
    
}

void PCDataType::AddDataTypeField(string FieldName,
                                string FieldTypeName,
                                string InitialValue,
                                int FieldIntfType,
                                int FieldQualifier,
                                s64 RangeMin,
                                s64 RangeMax,
                                string FullStorageSpec) {

    PCDataType * DataType = LookupDataType(FieldTypeName);
    if (DataType == nullptr) {
        __configuration->PCLogger->RaiseException(
            "Field Type Name " + FieldTypeName + " is not found !");
    }

    AddDataTypeField(FieldName, FieldTypeName,
                    DataType->__DataTypeCategory, InitialValue,
                    FieldIntfType, FieldQualifier, RangeMin, RangeMax,
                    FullStorageSpec);
}



void PCDataType::AddDataTypeField(string FieldName,
                                string FieldTypeName,
                                DataTypeCategory FieldTypeCategory,
                                string InitialValue, 
                                int FieldIntfType,
                                int FieldQualifier,
                                s64 RangeMin,
                                s64 RangeMax,
                                string FullStorageSpec) {


    PCDataType * DataType = LookupDataType(FieldTypeName);
    if (DataType == nullptr) {
        __configuration->PCLogger->RaiseException(
            "Field Type Name " + FieldTypeName + " is not found !");
    }
    if (DataType->__DataTypeCategory == DataTypeCategory::POU 
            || DataType->__DataTypeCategory == DataTypeCategory::DERIVED) {
        if (!InitialValue.empty()) {
            __configuration->PCLogger->RaiseException("Initial Values cannot "
                "be specified for complex data types");
        }
    } 
    
    if(__DataTypeCategory == DataTypeCategory::ARRAY) {
        __configuration->PCLogger->RaiseException("New fields cannot be "
            "added to a data type of ARRAY category");
    }

    if (DataType->__DataTypeCategory == DataTypeCategory::ARRAY) {
        
        switch(DataType->__DimensionSizes.size()) {
            case 1 : AddArrayDataTypeField(FieldName, DataType->__DataTypeName,
                        DataType->__DimensionSizes[0], InitialValue,
                        FieldIntfType, FieldQualifier, RangeMin, RangeMax,
                        FullStorageSpec);

                    
                    break;
            case 2 : AddArrayDataTypeField(FieldName, DataType->__DataTypeName,
                        DataType->__DimensionSizes[0], 
                        DataType->__DimensionSizes[1], InitialValue,
                        FieldIntfType, FieldQualifier, RangeMin, RangeMax,
                        FullStorageSpec);
                    break;
            default:  __configuration->PCLogger->RaiseException(
                        "Dimensions cannot exceed 2 "); 
        }

        
        
        return;
    }

    FieldTypeName = Utils::ResolveAliasName(FieldTypeName, __configuration);
    DataType = LookupDataType(FieldTypeName);
    assert(DataType != nullptr 
        && DataType->__DataTypeCategory == FieldTypeCategory);

    PCDataTypeField NewField(FieldName, FieldTypeName,
                            FieldTypeCategory, FieldQualifier,
                            RangeMin, RangeMax,
                            InitialValue, FieldIntfType, DataType,
                            "NONE", FullStorageSpec);

    __FieldsByInterfaceType[FieldIntfType].push_back(NewField);    
    if (FieldIntfType == FieldInterfaceType::VAR_IN_OUT ||
        FieldIntfType == FieldInterfaceType::VAR_EXTERNAL ||
        FieldIntfType == FieldInterfaceType::VAR_ACCESS ) {
        // this is a pointer
        __SizeInBits += sizeof (PCDataType *)*8;
    } else {
        __SizeInBits += DataType->__SizeInBits;

        if (__SizeInBits % 8 != 0)
            __SizeInBits += (8 - __SizeInBits%8);
    }
    
    __NFields ++;
    
    
}


void PCDataType::AddArrayDataTypeField( string FieldName,
                                        string FieldTypeName,
                                        int DimensionSize,
                                        string InitialValue,
                                        int FieldIntfType,
                                        int FieldQualifier,
                                        s64 RangeMin,
                                        s64 RangeMax,
                                        string FullStorageSpec) {

    PCDataType * DataType = LookupDataType(FieldTypeName);
    assert(DataType != nullptr 
        && DataType->__DataTypeCategory != DataTypeCategory::ARRAY);


    FieldTypeName = Utils::ResolveAliasName(FieldTypeName, __configuration);
    DataType = LookupDataType(FieldTypeName);
    assert(DataType != nullptr);

    if (DataType->__DataTypeCategory == DataTypeCategory::POU 
            || DataType->__DataTypeCategory == DataTypeCategory::DERIVED) {
        if (!InitialValue.empty()) {
            __configuration->PCLogger->RaiseException("Initial Values cannot "
                "be specified for complex data types");
        }
    }

    if(__DataTypeCategory == DataTypeCategory::ARRAY) {
        __configuration->PCLogger->RaiseException("New fields cannot be "
            "added to a data type of ARRAY category");
    }


    // Add a field with just the array name
    PCDataTypeField NewField(FieldName, DataType->__DataTypeName,
                        DataTypeCategory::ARRAY, FieldQualifier,
                        RangeMin, RangeMax,
                        InitialValue, FieldIntfType, DataType,
                        "NONE", FullStorageSpec);
    NewField.__NDimensions = 1;
    NewField.__Dimension1 = DimensionSize;
    NewField.__Dimension2 = 1;
    __FieldsByInterfaceType[FieldIntfType].push_back(NewField);

    if (FieldIntfType == FieldInterfaceType::VAR_IN_OUT ||
        FieldIntfType == FieldInterfaceType::VAR_EXTERNAL ||
        FieldIntfType == FieldInterfaceType::VAR_ACCESS ) {
        // this is a pointer
        __SizeInBits += sizeof (PCDataType *)*8;
    } else {
        __SizeInBits += DataType->__SizeInBits*NewField.__Dimension1;

        if (__SizeInBits % 8 != 0)
            __SizeInBits += (8 - __SizeInBits%8);
    }
    
}

void PCDataType::AddArrayDataTypeField(string FieldName,
                                        string FieldTypeName,
                                        int DimensionSize1,
                                        int DimensionSize2,
                                        string InitialValue,
                                        int FieldIntfType,
                                        int FieldQualifier,
                                        s64 RangeMin,
                                        s64 RangeMax,
                                        string FullStorageSpec) {

    PCDataType * DataType = LookupDataType(FieldTypeName);
    assert(DataType != nullptr 
        && DataType->__DataTypeCategory != DataTypeCategory::ARRAY);


    FieldTypeName = Utils::ResolveAliasName(FieldTypeName, __configuration);
    DataType = LookupDataType(FieldTypeName);
    assert(DataType != nullptr);

    if (DataType->__DataTypeCategory == DataTypeCategory::POU 
            || DataType->__DataTypeCategory == DataTypeCategory::DERIVED) {
        if (!InitialValue.empty()) {
            __configuration->PCLogger->RaiseException("Initial Values cannot "
                "be specified for complex data types");
        }
    }

    if(__DataTypeCategory == DataTypeCategory::ARRAY) {
        __configuration->PCLogger->RaiseException("New fields cannot be "
            "added to a data type of ARRAY category");
    }


    // Add a field with just the array name
    PCDataTypeField NewField(FieldName, DataType->__DataTypeName,
                        DataTypeCategory::ARRAY, FieldQualifier,
                        RangeMin, RangeMax,
                        InitialValue, FieldIntfType, DataType,
                        "NONE", FullStorageSpec);
    NewField.__NDimensions = 2;
    NewField.__Dimension1 = DimensionSize1;
    NewField.__Dimension2 = DimensionSize2;
    __FieldsByInterfaceType[FieldIntfType].push_back(NewField);

    if (FieldIntfType == FieldInterfaceType::VAR_IN_OUT ||
        FieldIntfType == FieldInterfaceType::VAR_EXTERNAL ||
        FieldIntfType == FieldInterfaceType::VAR_ACCESS ) {
        // this is a pointer
        __SizeInBits += sizeof (PCDataType *)*8;
    } else {
        __SizeInBits += DataType->__SizeInBits*NewField.__Dimension1
                                        *NewField.__Dimension2;

        if (__SizeInBits % 8 != 0)
            __SizeInBits += (8 - __SizeInBits%8);
    }
    
}

void PCDataType::AddDataTypeFieldAT(string FieldName,
                                    string FieldTypeName,
                                    string InitialValue,
                                    int FieldQualifier,
                                    s64 RangeMin,
                                    s64 RangeMax,
                                    int MemType,
                                    int ByteOffset,
                                    int BitOffset,
                                    string ResourceName,
                                    string FullStorageSpec) {
    PCDataType * DataType = LookupDataType(FieldTypeName);
    if (DataType == nullptr) {
        __configuration->PCLogger->RaiseException(
            "Field Type Name " + FieldTypeName + " is not found !");
    }

    // At fields can be added only to a PoU data type
    assert(__DataTypeCategory == DataTypeCategory::POU);

    AddDataTypeFieldAT(FieldName, DataType->__DataTypeName,
                    DataType->__DataTypeCategory, InitialValue, FieldQualifier,
                    RangeMin, RangeMax ,MemType, ByteOffset, BitOffset,
                    ResourceName, FullStorageSpec);
}

void PCDataType::AddDataTypeFieldAT(string FieldName,
                                    string FieldTypeName,
                                    DataTypeCategory FieldTypeCategory,
                                    string InitialValue,
                                    int FieldQualifier,
                                    s64 RangeMin,
                                    s64 RangeMax,
                                    int MemType,
                                    int ByteOffset,
                                    int BitOffset,
                                    string ResourceName,
                                    string FullStorageSpec) {

    assert(__DataTypeCategory == DataTypeCategory::POU);
    PCDataType * DataType = LookupDataType(FieldTypeName);
    assert(DataType != nullptr);




    // We don't support complex directly represented variables.
    //assert(DataType->__DataTypeCategory != DataTypeCategory::POU &&
    //    DataType->__DataTypeCategory != DataTypeCategory::DERIVED);

    assert(DataType->__DataTypeCategory != DataTypeCategory::POU);
    
    if(__DataTypeCategory == DataTypeCategory::ARRAY) {
        __configuration->PCLogger->RaiseException("New fields cannot be "
            "added to a data type of ARRAY category");
    }

    if (DataType->__DataTypeCategory == DataTypeCategory::ARRAY) {
        
        switch(DataType->__DimensionSizes.size()) {
            case 1 : AddArrayDataTypeFieldAT(FieldName, DataType->__DataTypeName,
                        DataType->__DimensionSizes[0], InitialValue,
                        FieldQualifier, RangeMin, RangeMax, MemType,
                        ByteOffset, BitOffset, ResourceName, FullStorageSpec);
                    break;
            case 2 : AddArrayDataTypeFieldAT(FieldName, DataType->__DataTypeName,
                        DataType->__DimensionSizes[0], 
                        DataType->__DimensionSizes[1], InitialValue,
                        FieldQualifier, RangeMin, RangeMax, MemType,
                        ByteOffset, BitOffset, ResourceName, FullStorageSpec);
                    break;
            default:  __configuration->PCLogger->RaiseException(
                        "Dimensions cannot exceed 2 "); 
        }

        
        return;
    }

    FieldTypeName = Utils::ResolveAliasName(FieldTypeName, __configuration);
    DataType = LookupDataType(FieldTypeName);
    assert(DataType != nullptr 
        && DataType->__DataTypeCategory == FieldTypeCategory);


    PCDataTypeField NewField(FieldName, FieldTypeName,
                            FieldTypeCategory, FieldQualifier,
                            RangeMin, RangeMax, InitialValue,
                            FieldInterfaceType::VAR_EXPLICIT_STORAGE,
                            DataType, ResourceName, FullStorageSpec);

    if (DataType->__DataTypeCategory == DataTypeCategory::BOOL)
        NewField.SetExplicitStorageConstraints(MemType, ByteOffset, BitOffset);
    else
        NewField.SetExplicitStorageConstraints(MemType, ByteOffset, 0);

    __FieldsByInterfaceType[FieldInterfaceType::VAR_EXPLICIT_STORAGE]
            .push_back(NewField);    
    
    // this is a pointer
    __SizeInBits += sizeof (PCDataType *)*8;
    __NFields ++;

}



void PCDataType::AddArrayDataTypeFieldAT(string FieldName,
                                        string FieldTypeName,
                                        int DimensionSize,
                                        string InitialValue,
                                        int FieldQualifier,
                                        s64 RangeMin,
                                        s64 RangeMax,
                                        int MemType,
                                        int ByteOffset,
                                        int BitOffset,
                                        string ResourceName,
                                        string FullStorageSpec) {

    PCDataType * DataType = LookupDataType(FieldTypeName);
    assert(DataType != nullptr 
        && DataType->__DataTypeCategory != DataTypeCategory::ARRAY);


    FieldTypeName = Utils::ResolveAliasName(FieldTypeName, __configuration);
    DataType = LookupDataType(FieldTypeName);
    assert(DataType != nullptr);

    if (DataType->__DataTypeCategory == DataTypeCategory::POU 
            || DataType->__DataTypeCategory == DataTypeCategory::DERIVED) {
        if (!InitialValue.empty()) {
            __configuration->PCLogger->RaiseException("Initial Values cannot "
                "be specified for complex data types");
        }
    }

    if(__DataTypeCategory == DataTypeCategory::ARRAY) {
        __configuration->PCLogger->RaiseException("New fields cannot be "
            "added to a data type of ARRAY category");
    }

    assert(__DataTypeCategory == DataTypeCategory::POU);

    PCDataTypeField NewField(FieldName, DataType->__DataTypeName,
                            DataTypeCategory::ARRAY,
                            FieldQualifier, RangeMin, RangeMax, InitialValue,
                            FieldInterfaceType::VAR_EXPLICIT_STORAGE,
                            DataType, ResourceName, FullStorageSpec);
    NewField.__NDimensions = 1;
    NewField.__Dimension1 = DimensionSize;
    NewField.__Dimension2 = 1;
    NewField.SetExplicitStorageConstraints(MemType, ByteOffset, 0);
    __FieldsByInterfaceType[FieldInterfaceType::VAR_EXPLICIT_STORAGE]
                .push_back(NewField);
    __SizeInBits += sizeof (PCDataType *)*8; 
}

void PCDataType::AddArrayDataTypeFieldAT(string FieldName,
                                        string FieldTypeName,
                                        int DimensionSize1,
                                        int DimensionSize2,
                                        string InitialValue,
                                        int FieldQualifier,
                                        s64 RangeMin,
                                        s64 RangeMax,
                                        int MemType,
                                        int ByteOffset,
                                        int BitOffset,
                                        string ResourceName,
                                        string FullStorageSpec) {

    PCDataType * DataType = LookupDataType(FieldTypeName);
    assert(DataType != nullptr 
        && DataType->__DataTypeCategory != DataTypeCategory::ARRAY);


    FieldTypeName = Utils::ResolveAliasName(FieldTypeName, __configuration);
    DataType = LookupDataType(FieldTypeName);
    assert(DataType != nullptr);

    if (DataType->__DataTypeCategory == DataTypeCategory::POU 
            || DataType->__DataTypeCategory == DataTypeCategory::DERIVED) {
        if (!InitialValue.empty()) {
            __configuration->PCLogger->RaiseException("Initial Values cannot "
                "be specified for complex data types");
        }
    }

    if(__DataTypeCategory == DataTypeCategory::ARRAY) {
        __configuration->PCLogger->RaiseException("New fields cannot be "
            "added to a data type of ARRAY category");
    }
    assert(__DataTypeCategory == DataTypeCategory::POU);

    PCDataTypeField NewField(FieldName, DataType->__DataTypeName,
                            DataTypeCategory::ARRAY,
                            FieldQualifier, RangeMin, RangeMax, InitialValue,
                            FieldInterfaceType::VAR_EXPLICIT_STORAGE,
                            DataType, ResourceName, FullStorageSpec);
    NewField.__NDimensions = 2;
    NewField.__Dimension1 = DimensionSize1;
    NewField.__Dimension2 = DimensionSize2;
    NewField.SetExplicitStorageConstraints(MemType, ByteOffset, 0);
    __FieldsByInterfaceType[FieldInterfaceType::VAR_EXPLICIT_STORAGE]
                .push_back(NewField); 
     __SizeInBits += sizeof (PCDataType *)*8;
    
}


void PCDataType::SetElementaryDataTypeAttributes(string InitialValue,
                                                s64 RangeMin,
                                                s64 RangeMax) {

    bool is_empty = InitialValue.empty();
    string InitValue, DataTypeName;
    switch(__DataTypeCategory) {
        case DataTypeCategory::BOOL :     
                            __SizeInBits = 1, DataTypeName = "BOOL";
                            InitValue = is_empty ? "0" : InitialValue;
                            break;
        case DataTypeCategory::BYTE :     
                            __SizeInBits = 8, DataTypeName = "BYTE";
                            InitValue = is_empty ? "16#0" : InitialValue;
                            break;
        case DataTypeCategory::WORD :     
                            __SizeInBits = 16, DataTypeName = "WORD";
                            InitValue = is_empty ? "16#0" : InitialValue;
                            break;
        case DataTypeCategory::DWORD :     
                            __SizeInBits = 32, DataTypeName = "DWORD";
                            InitValue = is_empty ? "16#0" : InitialValue;
                            break;
        case DataTypeCategory::LWORD :    
                            __SizeInBits = 64, DataTypeName = "LWORD";
                            InitValue = is_empty ? "16#0" : InitialValue;
                            break;
        case DataTypeCategory::CHAR :    
                            __SizeInBits = 8, DataTypeName = "CHAR";
                            InitValue = is_empty ? "" : InitialValue;
                            break;
        case DataTypeCategory::INT :      
                            __SizeInBits = 16, DataTypeName = "INT";
                            InitValue = is_empty ? "0" : InitialValue;
                            break;
        case DataTypeCategory::SINT :     
                            __SizeInBits = 8, DataTypeName = "SINT";
                            InitValue = is_empty ? "0" : InitialValue;
                            break;
        case DataTypeCategory::DINT :     
                            __SizeInBits = 32, DataTypeName = "DINT";
                            InitValue = is_empty ? "0" : InitialValue;
                            break;
        case DataTypeCategory::LINT :     
                            __SizeInBits = 64, DataTypeName = "LINT";
                            InitValue = is_empty ? "0" : InitialValue;
                            break;
        case DataTypeCategory::UINT :     
                            __SizeInBits = 16, DataTypeName = "UINT";
                            InitValue = is_empty ? "0" : InitialValue;
                            break;
        case DataTypeCategory::USINT :     
                            __SizeInBits = 8, DataTypeName = "USINT";
                            InitValue = is_empty ? "0" : InitialValue;
                            break;
        case DataTypeCategory::UDINT :     
                            __SizeInBits = 32, DataTypeName = "UDINT";
                            InitValue = is_empty ? "0" : InitialValue;
                            break;
        case DataTypeCategory::ULINT :     
                            __SizeInBits = 64, DataTypeName = "ULINT";
                            InitValue = is_empty ? "0" : InitialValue;
                            break;
        case DataTypeCategory::REAL :     
                            __SizeInBits = 32, DataTypeName = "REAL";
                            InitValue = is_empty ? "0.0" : InitialValue;
                            break;
        case DataTypeCategory::LREAL :     
                            __SizeInBits = 64, DataTypeName = "LREAL";
                            InitValue = is_empty ? "0.0" : InitialValue;
                            break;
        case DataTypeCategory::TIME :     
                            __SizeInBits 
                                = sizeof(pc_emulator::TimeType)*8, DataTypeName = "TIME";
                            InitValue = is_empty ? "t#0s" : InitialValue;
                            break;
        case DataTypeCategory::DATE :     
                            __SizeInBits 
                                = sizeof(pc_emulator::DateType)*8, DataTypeName = "DATE";
                            InitValue = is_empty ? "d#0001-01-01" : InitialValue;
                            break;
        case DataTypeCategory::TIME_OF_DAY :     
                            __SizeInBits 
                                = sizeof(pc_emulator::TODType)*8, DataTypeName = "TOD";
                            InitValue = is_empty ? "tod#00:00:00" : InitialValue;
                            break;
        case DataTypeCategory::DATE_AND_TIME :     
                            __SizeInBits 
                                = sizeof(pc_emulator::DateTODType)*8, DataTypeName = "DT";
                            InitValue = is_empty ? "dt#0001-01-01-00:00:00" 
                                            : InitialValue;
                            break;
        default :           return;
    }

    __NFields = 0; // This will be 0 only for elementary data types
    //AddDataTypeField("@", DataTypeName, DataTypeCategory, InitValue, 
    //                RangeMin, RangeMax);
    __RangeMax = RangeMax;
    __RangeMin = RangeMin;
    __InitialValue = InitValue;

    if (LookupDataType(DataTypeName) == nullptr)
    __configuration->PCLogger->LogMessage(LogLevels::LOG_INFO,
            "Registered Elementary DataType: " + DataTypeName);

}

PCDataType * PCDataType::LookupDataType(string DataTypeName) {
    return __configuration->RegisteredDataTypes->GetDataType(
                                            DataTypeName);
}



PCDataType::PCDataType(PCConfiguration* configuration, 
                    string AliasName, string DataTypeName,
                    DataTypeCategory Category,
                    string InitialValue, s64 RangeMin,
                    s64 RangeMax) {

    
    __configuration = configuration;
    __DataTypeName = DataTypeName;
    __NFields = 0, __SizeInBits = 0;
    __AliasName = AliasName;
    __InitialValue = InitialValue;
    __RangeMax = RangeMax;
    __RangeMin = RangeMin;
    __PoUType = pc_specification::PoUType::NOA; // It is set later for datatypes of POU category

    for (int IntfType = FieldInterfaceType::VAR_INPUT; 
            IntfType <= FieldInterfaceType::NA; IntfType ++) {
        __FieldsByInterfaceType.insert(
            std::pair<int,vector<PCDataTypeField> >(
                IntfType, vector<PCDataTypeField>()));
    }
    

    
    // Since size of dimensions is not specified, it cannot be an array or
    // string
    assert (Category != DataTypeCategory::ARRAY);

    if (LookupDataType(AliasName) != nullptr) {
            __configuration->PCLogger->RaiseException("Alias Name " + AliasName
                    + " has already been used !");
    }        
    

    if (Category == DataTypeCategory::NOT_ASSIGNED) {
        // Since DataTypeCategory is not provided, this DataTypeName must have
        // already been registered
        PCDataType * DataType = LookupDataType(DataTypeName);
        assert (DataType != nullptr);
        __DataTypeCategory = DataType->__DataTypeCategory;
        
        if (__DataTypeCategory == DataTypeCategory::DERIVED
            || __DataTypeCategory == DataTypeCategory::POU) {
                if (!InitialValue.empty()) {
                    __configuration->PCLogger->RaiseException(
                            "Cannot specify initial values for complex "
                            "data types");
                }

                for (int IntfType = FieldInterfaceType::VAR_INPUT; 
                    IntfType <= FieldInterfaceType::NA; IntfType ++) {

                    // This is like a Typedef of a derived data type/function block
                    // Copy all the fields of the derived data type/function block
                    for (auto& field: 
                            DataType->__FieldsByInterfaceType[IntfType]) {
                        PCDataType * FieldDataType = LookupDataType(
                                                        field.__FieldTypeName);
                        if (!FieldDataType) {
                            __configuration->PCLogger->RaiseException(
                                "Field DataType Not found for " 
                                + field.__FieldTypeName);
                        }
                        __FieldsByInterfaceType[IntfType].push_back(field);
                        __NFields ++;

                        if (IntfType 
                                == FieldInterfaceType::VAR_IN_OUT || 
                            IntfType 
                                == FieldInterfaceType::VAR_EXTERNAL ||
                            IntfType 
                                == FieldInterfaceType::VAR_ACCESS ) {
                            __SizeInBits += sizeof (PCDataType *)*8;
                        } else {

                            if (field.__FieldTypeCategory
                                == DataTypeCategory::ARRAY
                                && field.__FieldTypePtr->__DataTypeCategory
                                == DataTypeCategory::BOOL) {
                                // this field is a boolean array
                                // round the datatype size to be byte aligned
                                int TotalElements  = field.__Dimension1;
                                if (field.__NDimensions == 2)
                                    TotalElements *= field.__Dimension2;
                                
                                __SizeInBits += TotalElements;
                                if (TotalElements %8 != 0)
                                    __SizeInBits += (8 - TotalElements%8);
                                assert(__SizeInBits % 8 == 0);

                            } else if (field.__FieldTypePtr->__DataTypeCategory
                                        == DataTypeCategory::BOOL) {
                                // this field is a boolean variable
                                // round the datatype size to nearest byte
                                __SizeInBits += 8;
                            } else {
                                __SizeInBits += FieldDataType->__SizeInBits;
                            }
                        } 

                    }
                }           

        } else {
           __configuration->PCLogger->RaiseException("This should never happen!\n");
        }
    } else {
        __DataTypeCategory = Category;
        if (__DataTypeCategory != DataTypeCategory::DERIVED &&
            __DataTypeCategory != DataTypeCategory::POU) {

            __configuration->PCLogger->LogMessage(LogLevels::LOG_INFO,
                "Registering as an elementary datatype because category of "
                "datatype was not complex!");
                SetElementaryDataTypeAttributes(InitialValue,
                        RangeMin, RangeMax);
        } 
    }
    __DimensionSizes.push_back(1);
}


// For typedeffing an array datatype
/*
    AliasName:  Name of the newly created array datatype
    DataTypeName:   Array type name. can be INT, REAL, FunctionBlock etc..
    DimSize:    Size of the array
    InitialValue:  Initialvalue specified in string format: {val1, val2, ... valn}
*/
PCDataType::PCDataType(PCConfiguration* configuration, 
                    string AliasName, string DataTypeName, int DimSize, 
                    DataTypeCategory Category,
                    string InitialValue, s64 RangeMin,
                    s64 RangeMax) {

    __configuration = configuration;
    __DataTypeName = DataTypeName;
    __NFields = 0, __SizeInBits = 0;
    __AliasName = AliasName;
    __InitialValue = InitialValue;
    __PoUType = pc_specification::PoUType::NOA; // It is set later for datatypes of POU category
 

    for (int IntfType = FieldInterfaceType::VAR_INPUT; 
            IntfType <= FieldInterfaceType::NA; IntfType ++) {
        __FieldsByInterfaceType.insert(
            std::pair<int,vector<PCDataTypeField> >(
                IntfType, vector<PCDataTypeField>()));
    }

    if (LookupDataType(AliasName) != nullptr) {
            __configuration->PCLogger->RaiseException("Alias Name " + AliasName
                    + " has already been used !");
    }

    PCDataType * DataType = LookupDataType(DataTypeName);
    __DataTypeCategory = DataTypeCategory::ARRAY;
    assert(DataType != nullptr);
    if ((DataType->__DataTypeCategory == DataTypeCategory::DERIVED ||
            DataType->__DataTypeCategory == DataTypeCategory::POU)
        && !InitialValue.empty()) {
            __configuration->PCLogger->RaiseException("Initial Values cannot"
                " be specified for arrays of complex types!\n");
    }

    __RangeMax = (DataType->__RangeMax < RangeMax) ? DataType->__RangeMax :
                                                        RangeMax;

    __RangeMin = (DataType->__RangeMin > RangeMin) ? DataType->__RangeMin :
                                                        RangeMin;

    
    
    std::vector<std::string> InitialValues;
    boost::trim_if(InitialValue, boost::is_any_of("\t ,{}"));
    boost::split(InitialValues, InitialValue,
                boost::is_any_of(",{}"), boost::token_compress_on); 
    
    assert (InitialValue == "" ||
            (int)InitialValues.size() == DimSize);                                       


    __NFields = DimSize;
    __SizeInBits = DimSize*DataType->__SizeInBits;
    __DimensionSizes.push_back(DimSize);
}


PCDataType::PCDataType(PCConfiguration* configuration, 
                    string AliasName, string DataTypeName,
                    int Dim1Size, int Dim2Size, 
                    DataTypeCategory Category,
                    string InitialValue, 
                    s64 RangeMin, s64 RangeMax) {

    __configuration = configuration;
    __DataTypeName = DataTypeName;
    __NFields = 0, __SizeInBits = 0;
    __AliasName = AliasName;
    __InitialValue = InitialValue;
    __PoUType = pc_specification::PoUType::NOA; // It is set later for datatypes of POU category
    

    for (int IntfType = FieldInterfaceType::VAR_INPUT; 
            IntfType <= FieldInterfaceType::NA; IntfType ++) {
        __FieldsByInterfaceType.insert(
            std::pair<int,vector<PCDataTypeField> >(
                IntfType, vector<PCDataTypeField>()));
    }

    if (LookupDataType(AliasName) != nullptr) {
            __configuration->PCLogger->RaiseException("Alias Name " + AliasName
                    + " has already been used !");
    }

    PCDataType * DataType = LookupDataType(DataTypeName);
    assert(DataType != nullptr);
    if ((DataType->__DataTypeCategory == DataTypeCategory::DERIVED ||
            DataType->__DataTypeCategory == DataTypeCategory::POU)
        && !InitialValue.empty()) {
            __configuration->PCLogger->RaiseException("Initial Values cannot"
                " be specified for arrays of complex types!\n");
    }

    __DataTypeCategory = DataTypeCategory::ARRAY;

    __RangeMax = (DataType->__RangeMax < RangeMax) ? DataType->__RangeMax :
                                                        RangeMax;
    __RangeMin = (DataType->__RangeMin > RangeMin) ? DataType->__RangeMin :
                                                        RangeMin;

                                                   
    std::vector<std::string> InitialValues;
    boost::trim_if(InitialValue, boost::is_any_of("\t ,{}"));
    boost::split(InitialValues, InitialValue,
                boost::is_any_of(",{}"), boost::token_compress_on); 
    assert (InitialValue == "" ||
            (int)InitialValues.size() == Dim1Size*Dim2Size);                                       

   __NFields = Dim1Size*Dim2Size;
    __SizeInBits = __NFields*DataType->__SizeInBits;
    __DimensionSizes.push_back(Dim1Size);
    __DimensionSizes.push_back(Dim2Size);
}



bool PCDataType::CheckRemFields(std::vector<string>& NestedFields, int StartPos,
                            PCDataType * Current) {
    if (StartPos >= (int)NestedFields.size())
        return true;

    for (int i = StartPos ; i < (int)NestedFields.size(); i++) {
        string AccessedFieldName = NestedFields[i];
        for (int IntfType = FieldInterfaceType::VAR_INPUT; 
            IntfType <= FieldInterfaceType::NA; IntfType ++) {
            for(auto& DefinedField: Current->__FieldsByInterfaceType[IntfType]) {
                PCDataType * FieldDataType = DefinedField.__FieldTypePtr;
                if(AccessedFieldName == DefinedField.__FieldName) {
                    
                    if (DefinedField.__FieldTypeCategory 
                            != DataTypeCategory::ARRAY) {

                        assert(FieldDataType != nullptr);                
                        return CheckRemFields(NestedFields, i+1, FieldDataType);
        
                    } else {
                        if (i == (int)NestedFields.size() - 1)
                            return true;

                        int NDims = DefinedField.__NDimensions;

                        if (NDims == 1) {
                            assert(i + 1 < (int)NestedFields.size());
                            int idx = std::stoi(NestedFields[i+1]);
                            if (idx <= DefinedField.__Dimension1)
                                return CheckRemFields(NestedFields, i+2,
                                    LookupDataType(FieldDataType->__DataTypeName));
                            else 
                                return false;
                        } else if (NDims == 2) {
                            assert(i + 2 < (int)NestedFields.size());
                            int idx1 = std::stoi(NestedFields[i+1]);
                            int idx2 = std::stoi(NestedFields[i+2]);
                            if (idx1 <= DefinedField.__Dimension1
                                && idx2 <= DefinedField.__Dimension2)
                                return CheckRemFields(NestedFields, i+3,
                                    LookupDataType(FieldDataType->__DataTypeName));
                            else
                                return false;

                        } else {
                            return false;
                        }
                    } 
                }
            }
        }
    }
    return false;
}


bool PCDataType::IsFieldPresent(string NestedFieldName) {
    std::vector<std::string> NestedFields;
    if (NestedFieldName.empty())
        return true;

    boost::trim_if(NestedFieldName, boost::is_any_of("\t .[]"));
    boost::split(NestedFields, NestedFieldName,
                boost::is_any_of(".[]"), boost::token_compress_on);
    if (NestedFields.empty()) {
        return true;           
    }

    if (__DataTypeCategory == DataTypeCategory::ARRAY) {
        int NDims = __DimensionSizes.size();
        int idx1, idx2, nxt_nested_field;
        if (NDims == 1) {
            assert(0 < NestedFields.size());
            idx1 = std::stoi(NestedFields[0]);
            if (idx1 > __DimensionSizes[0])
                return false;
            nxt_nested_field = 1;
        } else if (NDims == 2) {
            assert(1 < NestedFields.size());
            idx1 = std::stoi(NestedFields[0]);
            idx2 = std::stoi(NestedFields[1]);

            if (idx1 > __DimensionSizes[0] || idx2 > __DimensionSizes[1])
                return false;
            nxt_nested_field = 2;
        } else {
            return false;
        }
        return CheckRemFields(NestedFields, nxt_nested_field, 
                                LookupDataType(__DataTypeName));
    }
    
    return CheckRemFields(NestedFields, 0, this);

}

bool PCDataType::GetPCDataTypeFieldOfArrayElement(
                PCDataTypeField& DefinedField,
                PCDataTypeField& Result, int idx1, int idx2) {


    int FieldStorageByteOffset;
    int FieldStorageBitOffset;
    std::vector<std::string> InitialValues;
    PCDataType * FieldDataType = DefinedField.__FieldTypePtr;

    string InitialValue = DefinedField.__InitialValue;
    string Init;
    boost::trim_if(InitialValue, boost::is_any_of("\t ,{}"));
    boost::split(InitialValues, InitialValue,
        boost::is_any_of(",{}"), boost::token_compress_on);

    if (FieldDataType->__DataTypeCategory == DataTypeCategory::POU)
        Result.__HoldingPoUType = FieldDataType->__DataTypeName;
    
    Result.__FieldInterfaceType
        = DefinedField.__FieldInterfaceType;
    Result.__RangeMax = DefinedField.__RangeMax;
    Result.__RangeMin = DefinedField.__RangeMin;
    Result.__FieldTypePtr = LookupDataType(
            FieldDataType->__DataTypeName);
    Result.__FieldTypeName = Result.__FieldTypePtr->__DataTypeName;
    Result.__NDimensions 
        = Result.__FieldTypePtr->__DimensionSizes.size();
    Result.__Dimension1 
        = Result.__FieldTypePtr->__DimensionSizes[0];
    if (Result.__NDimensions > 1)
        Result.__Dimension2 
        = Result.__FieldTypePtr->__DimensionSizes[1];
    Result.__StorageMemType 
            = DefinedField.__StorageMemType;
    Result.__FieldTypeCategory 
    = LookupDataType(FieldDataType->__DataTypeName)->__DataTypeCategory;
    
    int tot_offset;
    if (idx2 == -1){
        if (idx1 <= DefinedField.__Dimension1) {
            Result.__FieldName 
            = DefinedField.__FieldName 
            + "[" + std::to_string(idx1) + "]";
            tot_offset = idx1 -1;    
        } else 
            return false;
    } else {
        if (idx1 <= DefinedField.__Dimension1
            && idx2 <= DefinedField.__Dimension2) {
            tot_offset 
                = (idx1-1)*DefinedField.__Dimension2
                        + (idx2-1);
            Result.__FieldName 
                = DefinedField.__FieldName 
                + "[" + std::to_string(idx1) + "]"
                + "[" + std::to_string(idx2) + "]";

        } else
            return false;
    }
 
    if (Result.__FieldTypePtr->__DataTypeCategory
        == DataTypeCategory::BOOL) {
        FieldStorageByteOffset = 
            (DefinedField.__StorageByteOffset +
            tot_offset/8);
        FieldStorageBitOffset = 
            (DefinedField.__StorageBitOffset +
            tot_offset%8);
    } else {
        FieldStorageByteOffset = 
            (DefinedField.__StorageByteOffset +
            tot_offset*(Result
            .__FieldTypePtr->__SizeInBits/8)); 
        FieldStorageBitOffset = 0;
    }

    if (!InitialValue.empty() && 
        (tot_offset) < InitialValues.size()) {
        Init = InitialValues[tot_offset];
    }
    else { 
        auto got 
        = __configuration->__DataTypeDefaultInitialValues.find(
                Result.__FieldTypePtr->__DataTypeCategory);
        assert(got 
            != __configuration->__DataTypeDefaultInitialValues.end());
        Init = got->second;
    }
    Result.__StorageByteOffset = FieldStorageByteOffset;
    Result.__StorageBitOffset = FieldStorageBitOffset;
    Result.__InitialValue = Init;
    Result.__FieldQualifier = DefinedField.__FieldQualifier;
    Result.__AssociatedResourceName = DefinedField.__AssociatedResourceName;

    return true;

}

bool PCDataType::CheckRemFields(std::vector<string>& NestedFields, int StartPos,
                PCDataType * Current, PCDataTypeField& Result) {

    string HoldingPoUType = Result.__HoldingPoUType;
    if (StartPos >= (int)NestedFields.size())
        return true;

    for (int i = StartPos ; i < (int)NestedFields.size(); i++) {
        string AccessedFieldName = NestedFields[i];
        
        for (int IntfType = FieldInterfaceType::VAR_INPUT; 
            IntfType <= FieldInterfaceType::NA; IntfType ++) {
            for(auto& DefinedField: Current->__FieldsByInterfaceType[IntfType]) {
                PCDataType * FieldDataType = DefinedField.__FieldTypePtr;
                assert(FieldDataType != nullptr);
                if(AccessedFieldName == DefinedField.__FieldName) {

                    if (FieldDataType->__DataTypeCategory 
                        == DataTypeCategory::POU)
                        HoldingPoUType = FieldDataType->__DataTypeName;
                    
                    if (DefinedField.__FieldTypeCategory 
                        != DataTypeCategory::ARRAY) {
                        Result = DefinedField;
                        Result.__HoldingPoUType = HoldingPoUType;
                        assert(FieldDataType != nullptr);
                        return CheckRemFields(NestedFields, i+1, FieldDataType,
                                    Result);
                        
                    } else {
                        if (i == (int)NestedFields.size() - 1) {
                            Result = DefinedField;
                            Result.__HoldingPoUType = HoldingPoUType;
                            return true;
                        }

                        int NDims = DefinedField.__NDimensions;
                        int idx1, idx2;
                        int nxt_nested_field;
                        if (NDims == 1) {
                            assert(i + 1 < NestedFields.size());
                            idx1 = std::stoi(NestedFields[i+1]);
                            idx2 = -1;
                            nxt_nested_field = i + 2;
                        } else if (NDims == 2) {
                            assert(i + 2 < NestedFields.size());
                            idx1 = std::stoi(NestedFields[i+1]);
                            idx2 = std::stoi(NestedFields[i+2]);
                            nxt_nested_field = i + 3;
                        } else {
                            return false;
                        }

                        if (!GetPCDataTypeFieldOfArrayElement(
                            DefinedField, Result, idx1, idx2))
                            return false;
                        
                        return CheckRemFields(NestedFields,
                            nxt_nested_field,
                            LookupDataType(
                                FieldDataType->__DataTypeName), Result);

                    } 
                    return CheckRemFields(NestedFields, i+1, FieldDataType,
                                        Result);
                }
            }
        }
    }
    return false;
}

bool PCDataType::GetPCDataTypeField(string NestedFieldName,
                PCDataTypeField& Result) {

    std::vector<std::string> NestedFields;
    boost::trim_if(NestedFieldName, boost::is_any_of("\t .[]"));
    boost::split(NestedFields, NestedFieldName,
                boost::is_any_of(".[]"), boost::token_compress_on);

    if (__DataTypeCategory == DataTypeCategory::POU) {
        //std::cout << "Setting Holding POU Type for: " << NestedFieldName
        //        << " to: " << __DataTypeName << std::endl;
        Result.__HoldingPoUType = __DataTypeName;
    }

    if (NestedFields.empty()) {
        return true;           
    }

    

    int nxt_nested_field = 0;
    if (__DataTypeCategory == ARRAY) {
        int NDims = __DimensionSizes.size();
        int idx1, idx2, tot_offset;
        int FieldStorageByteOffset;
        int FieldStorageBitOffset;

        string InitialValue = __InitialValue;
        std::vector<std::string> InitialValues;
        string Init;
        boost::trim_if(InitialValue, boost::is_any_of("\t ,{}"));
        boost::split(InitialValues, InitialValue,
            boost::is_any_of(",{}"), boost::token_compress_on);

        
        Result.__FieldInterfaceType = FieldInterfaceType::NA;
        Result.__RangeMax = __RangeMax;
        Result.__RangeMin = __RangeMin;
        Result.__FieldTypePtr = LookupDataType(__DataTypeName);
        Result.__NDimensions 
            = Result.__FieldTypePtr->__DimensionSizes.size();
        Result.__Dimension1 
            = Result.__FieldTypePtr->__DimensionSizes[0];
        if (Result.__NDimensions > 1)
            Result.__Dimension2 
            = Result.__FieldTypePtr->__DimensionSizes[1];
        Result.__StorageMemType = -1;
        Result.__FieldTypeCategory = Result.__FieldTypePtr->__DataTypeCategory;
        Result.__FieldTypeName = Result.__FieldTypePtr->__DataTypeName;
        Result.__FieldQualifier = -1;
        Result.__AssociatedResourceName = "NONE";

        if (NDims == 1) {
            assert(0 < NestedFields.size());
            idx1 = std::stoi(NestedFields[0]);
            if (idx1 > __DimensionSizes[0])
                return false;
            tot_offset = idx1 - 1;
            idx2 = -1;
            nxt_nested_field = 1;
            Result.__FieldName  = "[" + std::to_string(idx1) + "]";
        } else if (NDims == 2) {
            assert(1 < NestedFields.size());
            idx1 = std::stoi(NestedFields[0]);
            idx2 = std::stoi(NestedFields[1]);

            if (idx1 > __DimensionSizes[0] || idx2 > __DimensionSizes[1])
                return false;

            tot_offset = (idx1 - 1)*__DimensionSizes[1] + (idx2 - 1);
            nxt_nested_field = 2;
            Result.__FieldName  = "[" + std::to_string(idx1) + "]"
                + "[" + std::to_string(idx2) + "]";
        } else {
            return false;
        }

        if (Result.__FieldTypePtr->__DataTypeCategory
            == DataTypeCategory::BOOL) {
            FieldStorageByteOffset = tot_offset/8;
            FieldStorageBitOffset = tot_offset%8;
        } else {
            FieldStorageByteOffset = tot_offset*(Result
                .__FieldTypePtr->__SizeInBits/8); 
            FieldStorageBitOffset = 0;
        }
        Result.__StorageByteOffset = FieldStorageByteOffset;
        Result.__StorageBitOffset = FieldStorageBitOffset;

        if (!InitialValue.empty() && 
            (tot_offset) < InitialValues.size()) {
            Init = InitialValues[tot_offset];
        }
        else { 
            auto got 
            = __configuration->__DataTypeDefaultInitialValues.find(
                    Result.__FieldTypePtr->__DataTypeCategory);
            assert(got 
                != __configuration->__DataTypeDefaultInitialValues.end());
            Init = got->second;
        }
        Result.__InitialValue = Init;
        return CheckRemFields(NestedFields, nxt_nested_field,
                Result.__FieldTypePtr, Result);
        
    }

    return CheckRemFields(NestedFields, nxt_nested_field, this, Result);
}

bool DataTypeUtils::ValueToBool(string Value, bool& BoolValue) {
    if(boost::iequals(Value, "True")
        || boost::iequals(Value, "1")
        || boost::iequals(Value, "TRUE")
        || boost::iequals(Value, "true")) {
        BoolValue = true;
        return true;
    }

    if(boost::iequals(Value, "False") 
        || boost::iequals(Value, "0")
        || boost::iequals(Value, "FALSE")
        || boost::iequals(Value, "false")) {
        BoolValue = false;
        return true;
    }

    return false;
}
bool DataTypeUtils::ValueToByte(string Value, uint8_t & ByteValue) {
    // Value can be between 16#0 to 16#FF
    if (boost::starts_with(Value, "16#")
            && Value.length() <= 5) {
        string BitString = "0x" + Value.substr(3,Value.length() - 3);
        int8_t Cast = std::stoul(BitString, nullptr, 16);
        ByteValue = Cast & 0x00FF;
        return true;
    }
    return false;
}

bool DataTypeUtils::ValueToWord(string Value, uint16_t & WordValue) {

    if (boost::starts_with(Value, "16#")
            && Value.length() <= 7) {
        string BitString = "0x" + Value.substr(3,Value.length() - 3);
        int16_t Cast = std::stoul(BitString, nullptr, 16);
        WordValue = Cast & 0x00FFFF;
        return true;
    }
    return false;

}
bool DataTypeUtils::ValueToDWord(string Value, uint32_t& DWordValue) {
    if (boost::starts_with(Value, "16#")
            && Value.length() <= 11) {
        string BitString = "0x" + Value.substr(3,Value.length() - 3);
        int32_t Cast = std::stoul(BitString, nullptr, 16);
        DWordValue = Cast & 0x00FFFFFFFF;
        return true;
    }
    return false;

}
bool DataTypeUtils::ValueToLWord(string Value, uint64_t & LWordValue) {
    if (boost::starts_with(Value, "16#")
            && Value.length() <= 19) {
        string BitString = "0x" + Value.substr(3,Value.length() - 3);
        int64_t Cast = std::stoul(BitString, nullptr, 16);
        LWordValue = Cast & 0x00FFFFFFFFFFFFFFFF;
        return true;
    }
    return false;
}
bool DataTypeUtils::ValueToChar(string Value, char & CharValue){
    if(Value.length() == 0) {
        CharValue = '\0';
        return true;
    }

    CharValue = Value[0];
    return true;
}
bool DataTypeUtils::ValueToInt(string Value, int16_t& IntValue){
    IntValue = boost::lexical_cast<int>(Value);
    return true;
}

bool DataTypeUtils::ValueToSint(string Value, int8_t & SintValue){
    SintValue = boost::lexical_cast<short int>(Value);
    return true;
}

bool DataTypeUtils::ValueToDint(string Value, int32_t & DintValue){
    DintValue = boost::lexical_cast<int>(Value);
    return true;
}

bool DataTypeUtils::ValueToLint(string Value, int64_t& LintValue){
    LintValue = boost::lexical_cast<int64_t>(Value);
    return true;

}
bool DataTypeUtils::ValueToUint(string Value, uint16_t & UintValue){
    UintValue = boost::lexical_cast<int>(Value);
    return true;
}
bool DataTypeUtils::ValueToUsint(string Value, uint8_t & UsintValue){
    UsintValue = boost::lexical_cast<short int>(Value);
    return true;
}
bool DataTypeUtils::ValueToUdint(string Value, uint32_t & UdintValue){
    UdintValue = boost::lexical_cast<int>(Value);
    return true;
}
bool DataTypeUtils::ValueToUlint(string Value, uint64_t & UlintValue){
    UlintValue = boost::lexical_cast<int64_t>(Value);
    return true;

}
bool DataTypeUtils::ValueToReal(string Value, float & RealValue){
    //RealValue = boost::lexical_cast<double>(Value);
    RealValue = strtof((Value).c_str(),0);
    return true;
}

bool DataTypeUtils::ValueToLReal(string Value, double & LRealValue){
    LRealValue = boost::lexical_cast<double>(Value);
    return true;
}

bool DataTypeUtils::ValueToTime(string Value, TimeType & Time){
    if(boost::starts_with(Value, "t#")) {
        Time.SecsElapsed = boost::lexical_cast<int>(
                    Value.substr(2,Value.length() - 3));
        return true;
    }
    return false;
}
bool DataTypeUtils::ValueToTOD(string Value, TODType & TOD){
    if(boost::starts_with(Value, "tod#")
        && std::regex_match(Value.substr(4,8),
            std::regex("([01]?\\d|2[0-3]):([0-5]?\\d):([0-5]?\\d)"))){
        TOD.Hr = boost::lexical_cast<int>(Value.substr(4,2));
        TOD.Min = boost::lexical_cast<int>(Value.substr(7,2));
        TOD.Sec = boost::lexical_cast<int>(Value.substr(10,2));
        return true;
    }
    return false;

}
bool DataTypeUtils::ValueToDT(string Value, DateTODDataType & Dt){
    if(boost::starts_with(Value, "dt#")
        && std::regex_match(Value.substr(3,10),            
            std::regex("([012]\\d{3}-(0[1-9]|1[0-2])-(0[1-9]|[12]\\d|3[01]))"))
        && std::regex_match(Value.substr(14,8),
            std::regex("([01]?\\d|2[0-3]):([0-5]?\\d):([0-5]?\\d)"))){
        Dt.Date.Year = boost::lexical_cast<int>(Value.substr(3,4));
        Dt.Date.Month = boost::lexical_cast<int>(Value.substr(8,2));
        Dt.Date.Day = boost::lexical_cast<int>(Value.substr(11,2));
        Dt.Tod.Hr = boost::lexical_cast<int>(Value.substr(14,2));
        Dt.Tod.Min = boost::lexical_cast<int>(Value.substr(17,2));
        Dt.Tod.Sec = boost::lexical_cast<int>(Value.substr(20,2));
        return true;
    }
    return false;

}
bool DataTypeUtils::ValueToDate(string Value, DateType& Date){
    if(boost::starts_with(Value, "d#")
        && std::regex_match(Value.substr(2,Value.length() - 2),            
            std::regex("([012]\\d{3}-(0[1-9]|1[0-2])-(0[1-9]|[12]\\d|3[01]))"))){
        Date.Year = boost::lexical_cast<int>(Value.substr(2,4));
        Date.Month = boost::lexical_cast<int>(Value.substr(7,2));
        Date.Day = boost::lexical_cast<int>(Value.substr(10,2));
        return true;
    }
    return false;
}

     

bool DataTypeUtils::BoolToAny(bool Value, int Category,
        string& Result) {

    string Temp;
    switch(Category) {
        case DataTypeCategory::BOOL : Result = (Value == true ? "1" : "0"); 
                                        return true;
        case DataTypeCategory::BYTE :
        case DataTypeCategory::WORD :
        case DataTypeCategory::DWORD :
        case DataTypeCategory::LWORD :  Result = (Value == true ? 
                                                "16#1" : "16#0"); 
                                        return true;
        case DataTypeCategory::CHAR :   
        case DataTypeCategory::INT :
        case DataTypeCategory::SINT :
        case DataTypeCategory::DINT :
        case DataTypeCategory::LINT :
        case DataTypeCategory::UINT :
        case DataTypeCategory::USINT :
        case DataTypeCategory::UDINT :  
        case DataTypeCategory::ULINT :  Result = (Value == true ? "1" : "0"); 
                                        break;
        case DataTypeCategory::REAL :
        case DataTypeCategory::LREAL :  Result = (Value == true ? "1.0" : "0.0"); 
                                        return true;
        default     : return false;
        
    }

    return true;
}


bool DataTypeUtils::ByteToAny(uint8_t Value, int DataTypeCategory,
        string& Result) {

    return LWordToAny((uint64_t)Value, DataTypeCategory, Result);
}


bool DataTypeUtils::WordToAny(uint16_t Value, int DataTypeCategory,
        string& Result) {
    return LWordToAny((uint64_t)Value, DataTypeCategory, Result);
}


bool DataTypeUtils::DWordToAny(uint32_t Value, int DataTypeCategory,
        string& Result) {
    return LWordToAny((uint64_t)Value, DataTypeCategory, Result);
}


bool DataTypeUtils::LWordToAny(uint64_t Value, int Category,
        string& Result) {

    std::stringstream stream;
    
    switch(Category) {
        case DataTypeCategory::BOOL : Result = (Value > 0 ? "1" : "0");
                                      return true;
        case DataTypeCategory::BYTE :   Value = Value & 0x00000000000000FF;
                                        stream << std::hex << Value;
                                        Result = "16#" + stream.str();
                                        return true;
        case DataTypeCategory::WORD :   Value = Value & 0x000000000000FFFF;
                                        stream << std::hex << Value;
                                        Result = "16#" + stream.str();
                                        return true;
        case DataTypeCategory::DWORD :  Value = Value & 0x00000000FFFFFFFF;
                                        stream << std::hex << Value;
                                        Result = "16#" + stream.str();
                                        return true;
        case DataTypeCategory::LWORD :  Value = Value & 0xFFFFFFFFFFFFFFFF;
                                        stream << std::hex << Value;
                                        Result = "16#" + stream.str();
                                        return true;
        case DataTypeCategory::CHAR :   Result = (char )(Value & 0x00000000000000FF);
                                        return true;
        case DataTypeCategory::INT :    Result = std::to_string((int16_t )(Value));
                                        return true;
        case DataTypeCategory::SINT :   Result = std::to_string((int8_t )(Value));
                                        return true;
        case DataTypeCategory::DINT :   Result = std::to_string((int32_t )(Value));
                                        return true;
        case DataTypeCategory::LINT :   Result = std::to_string((int64_t )(Value));
                                        return true;
        case DataTypeCategory::UINT :   Result = std::to_string((uint16_t )(Value));
                                        return true;
        case DataTypeCategory::USINT :  Result = std::to_string((uint8_t )(Value));
                                        return true;
        case DataTypeCategory::UDINT :  Result = std::to_string((uint32_t )(Value));
                                        return true;
        case DataTypeCategory::ULINT :  Result = std::to_string((uint64_t )(Value));
                                        return true;
        case DataTypeCategory::REAL :   Result = std::to_string((float)(Value));
                                        return true;
        case DataTypeCategory::LREAL :  Result = std::to_string((double)(Value));
                                        return true;
        default: return false;
    }
}


bool DataTypeUtils::CharToAny(char Value, int Category,
        string& Result) {

    std::stringstream stream;

    int16_t ByteValue = (uint8_t )Value;

    switch(Category) {
        case DataTypeCategory::BOOL :   Result = (Value == '0' ? "0" : "1");
                                        return true;
        case DataTypeCategory::BYTE :
        case DataTypeCategory::WORD :
        case DataTypeCategory::DWORD :
        case DataTypeCategory::LWORD :  stream << std::hex << ByteValue;
                                        Result = "16#" + stream.str();
                                        return true;

        case DataTypeCategory::CHAR :   Result = Value;
                                        return true;
        case DataTypeCategory::INT :    
        case DataTypeCategory::SINT :
        case DataTypeCategory::DINT :
        case DataTypeCategory::LINT :
        case DataTypeCategory::UINT :
        case DataTypeCategory::USINT :
        case DataTypeCategory::UDINT :
        case DataTypeCategory::ULINT :  Result = std::to_string(ByteValue);
                                        return true;
        case DataTypeCategory::REAL :
        case DataTypeCategory::LREAL :  Result = std::to_string(ByteValue) + ".0";
                                        return true;
        default: return false;
    }
}


bool DataTypeUtils::IntToAny(int16_t Value, int DataTypeCategory,
        string& Result) {
    return LIntToAny((int64_t)Value, DataTypeCategory, Result);
}


bool DataTypeUtils::SIntToAny(int8_t Value, int DataTypeCategory,
        string& Result) {
    return LIntToAny((int64_t)Value, DataTypeCategory, Result);
}


bool DataTypeUtils::DIntToAny(int32_t Value, int DataTypeCategory,
        string& Result) {
    return LIntToAny((int64_t)Value, DataTypeCategory, Result);
}


bool DataTypeUtils::LIntToAny(int64_t Value, int Category,
        string& Result) {

    std::stringstream stream;
    
    switch(Category) {
        case DataTypeCategory::BOOL : Result = (Value > 0 ? "1" : "0");
                                      return true;
        case DataTypeCategory::BYTE :   Value = Value & 0x00FF;
                                        stream << std::hex << Value;
                                        Result = "16#" + stream.str();
                                        return true;
        case DataTypeCategory::WORD :   Value = Value & 0xFFFF;
                                        stream << std::hex << Value;
                                        Result = "16#" + stream.str();
                                        return true;
        case DataTypeCategory::DWORD :  Value = Value & 0xFFFFFFFF;
                                        stream << std::hex << Value;
                                        Result = "16#" + stream.str();
                                        return true;
        case DataTypeCategory::LWORD :  Value = Value & 0xFFFFFFFFFFFFFFFF;
                                        stream << std::hex << Value;
                                        Result = "16#" + stream.str();
                                        return true;
        case DataTypeCategory::CHAR :   Result = (char )(Value & 0x00FF);
                                        return true;
        case DataTypeCategory::INT :    Result = std::to_string((int16_t )(Value));
                                        return true;
        case DataTypeCategory::SINT :   Result = std::to_string((int8_t )(Value));
                                        return true;
        case DataTypeCategory::DINT :   Result = std::to_string((int32_t )(Value));
                                        return true;
        case DataTypeCategory::LINT :   Result = std::to_string((int64_t)(Value));
                                        return true;
        case DataTypeCategory::UINT :   Result = std::to_string((uint16_t )(Value));
                                        return true;
        case DataTypeCategory::USINT :  Result = std::to_string((uint8_t )(Value));
                                        return true;
        case DataTypeCategory::UDINT :  Result = std::to_string((uint32_t )(Value));
                                        return true;
        case DataTypeCategory::ULINT :  Result = std::to_string((uint64_t )(Value));
                                        return true;
        case DataTypeCategory::REAL :   Result = std::to_string((float)(Value));
                                        return true;
        case DataTypeCategory::LREAL :  Result = std::to_string((double)(Value));
                                        return true;
        default: return false;
    }
}


bool DataTypeUtils::UIntToAny(uint16_t Value, int DataTypeCategory,
        string& Result) {
    return UlintToAny((uint64_t)Value, DataTypeCategory, Result);
}

bool DataTypeUtils::USintToAny(uint8_t Value, int DataTypeCategory,
                    string& Result) {
    return UlintToAny((uint64_t)Value, DataTypeCategory, Result);
}


bool DataTypeUtils::UDintToAny(uint32_t Value, int DataTypeCategory,
        string& Result) {
    return UlintToAny((uint64_t)Value, DataTypeCategory, Result);
}


bool DataTypeUtils::UlintToAny(uint64_t Value, int DataTypeCategory,
        string& Result) {
    return LWordToAny(Value, DataTypeCategory, Result);
}


bool DataTypeUtils::RealToAny(float Value, int DataTypeCategory,
        string& Result) {
    return LRealToAny((double)Value, DataTypeCategory, Result);
}


bool DataTypeUtils::LRealToAny(double Value, int Category,
        string& Result) {

    switch(Category) {
        case DataTypeCategory::BOOL :  Result = (Value > 0.0 ? "1" : "0");
                                        return true;

        case DataTypeCategory::BYTE :   
        case DataTypeCategory::WORD :
        case DataTypeCategory::DWORD :
        case DataTypeCategory::LWORD :
        case DataTypeCategory::CHAR :   return false;
        case DataTypeCategory::INT :    Result = std::to_string((int16_t )(Value));
                                        return true;
        case DataTypeCategory::SINT :   Result = std::to_string((int8_t )(Value));
                                        return true;
        case DataTypeCategory::DINT :   Result = std::to_string((int32_t )(Value));
                                        return true;
        case DataTypeCategory::LINT :   Result = std::to_string((int64_t)(Value));
                                        return true;
        case DataTypeCategory::UINT :   Result = std::to_string((uint16_t )(Value));
                                        return true;
        case DataTypeCategory::USINT :  Result = std::to_string((uint8_t )(Value));
                                        return true;
        case DataTypeCategory::UDINT :  Result = std::to_string((uint32_t )(Value));
                                        return true;
        case DataTypeCategory::ULINT :  Result = std::to_string((uint64_t )(Value));
                                        return true;
        case DataTypeCategory::REAL :   Result = std::to_string((float)(Value));
                                        return true;
        case DataTypeCategory::LREAL :  Result = std::to_string((double)(Value));
                                        return true;
        default: return false;
    }
}






string DataTypeUtils::DTToDTString(DateTODDataType& dt) {
    string Dtstr, Month, Year, Day, Hr, Min, Sec;
    Year = std::to_string(dt.Date.Year);
    Month = std::to_string(dt.Date.Month);
    if (dt.Date.Month < 10)
        Month = "0" + Month;

    Day = std::to_string(dt.Date.Day);
    if (dt.Date.Day < 10)
        Day = "0" + Day;

    Hr = std::to_string(dt.Tod.Hr);
    if (dt.Tod.Hr < 10)
        Hr = "0" + Hr;

    Min = std::to_string(dt.Tod.Min);
    if (dt.Tod.Min < 10)
        Min = "0" + Min;

    Sec = std::to_string(dt.Tod.Sec);
    if (dt.Tod.Sec < 10)
        Sec = "0" + Sec;

    Dtstr = Year + "-" + Month + "-" + Day + "T" + Hr + ":" + Min + ":" + Sec  
        + "Z";
    return Dtstr;
}

string DataTypeUtils::DateToDTString(DateDataType& date1) {
    DateTODDataType dt_temp;
    dt_temp.Tod.Hr = 0;
    dt_temp.Tod.Min = 0;
    dt_temp.Tod.Sec = 0;
    dt_temp.Date = date1;

    return DTToDTString(dt_temp);
}

void DataTypeUtils::AddToDT(DateTODDataType& Dt, TimeType& Time) {
    string Dt1;
    Dt1 = DTToDTString(Dt);

    std::tm t = {};
    std::tm * ptm;
    std::istringstream ss(Dt1);
    ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");
    time_t SecsSinceEpoch = mktime(&t);
    SecsSinceEpoch += Time.SecsElapsed;

    ptm = gmtime(&SecsSinceEpoch);

    Dt.Date.Year = ptm->tm_year + 1900;
    Dt.Date.Month = ptm->tm_mon + 1;
    Dt.Date.Day = ptm->tm_mday;
    Dt.Tod.Hr = ptm->tm_hour;
    Dt.Tod.Min = ptm->tm_min;
    Dt.Tod.Sec = ptm->tm_sec;
}

void DataTypeUtils::AddToTOD(TODDataType& tod, TimeType& Time) {
    unsigned long SecsElapsed = tod.Hr*3600 + tod.Min*60 + tod.Sec;

    SecsElapsed += Time.SecsElapsed;

    tod.Hr = SecsElapsed / 3600;
    tod.Min = (SecsElapsed - tod.Hr*3600)/60;
    tod.Sec = (SecsElapsed - tod.Hr*3600 - tod.Min*60);

    if (tod.Hr >= 24)
        tod.Hr = tod.Hr - 24;
}

void DataTypeUtils::SubFromDT(DateTODDataType& Dt, TimeType& Time) {
    string Dt1;
    Dt1 = DTToDTString(Dt);

    std::tm t = {};
    std::tm * ptm;
    std::istringstream ss(Dt1);
    ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");
    time_t SecsSinceEpoch = mktime(&t);

    SecsSinceEpoch -= Time.SecsElapsed;

    ptm = gmtime(&SecsSinceEpoch);

    Dt.Date.Year = ptm->tm_year + 1900;
    Dt.Date.Month = ptm->tm_mon + 1;
    Dt.Date.Day = ptm->tm_mday;
    Dt.Tod.Hr = ptm->tm_hour;
    Dt.Tod.Min = ptm->tm_min;
    Dt.Tod.Sec = ptm->tm_sec;
}

void DataTypeUtils::SubFromTOD(TODDataType& tod, TimeType& Time) {
    unsigned long SecsElapsed = tod.Hr*3600 + tod.Min*60 + tod.Sec;

    if (Time.SecsElapsed <= SecsElapsed)
        SecsElapsed -= Time.SecsElapsed;
    else
        SecsElapsed = (24*3600 - (Time.SecsElapsed - SecsElapsed));

    tod.Hr = SecsElapsed / 3600;
    tod.Min = (SecsElapsed - tod.Hr*3600)/60;
    tod.Sec = (SecsElapsed - tod.Hr*3600 - tod.Min*60);

    if (tod.Hr >= 24)
        tod.Hr = tod.Hr - 24;
}

TimeType DataTypeUtils::SubDTs(DateTODDataType& Dt1, DateTODDataType& Dt2) {
    string Dt1str, Dt2str;
    TimeType ret;
    Dt1str = DTToDTString(Dt1);
    Dt2str = DTToDTString(Dt2);

    std::tm t1 = {};
    std::tm t2 = {};
    std::tm * ptm;
    std::istringstream ss1(Dt1str);
    ss1 >> std::get_time(&t1, "%Y-%m-%dT%H:%M:%S");
    time_t SecsSinceEpoch1 = mktime(&t1);
    std::istringstream ss2(Dt2str);
    ss2 >> std::get_time(&t2, "%Y-%m-%dT%H:%M:%S");
    time_t SecsSinceEpoch2 = mktime(&t2);

    
    ret.SecsElapsed = SecsSinceEpoch1 - SecsSinceEpoch2;
    
    return ret;

}

TimeType DataTypeUtils::SubTODs(TODDataType& tod1, TODDataType& tod2) {
    unsigned long SecsElapsed1 = tod1.Hr*3600 + tod1.Min*60 + tod1.Sec;
    unsigned long SecsElapsed2 = tod2.Hr*3600 + tod2.Min*60 + tod2.Sec;

    TimeType ret;
    
    ret.SecsElapsed = SecsElapsed1 - SecsElapsed2;
    return ret;
}

TimeType DataTypeUtils::SubDATEs(DateDataType& date1, DateDataType& date2) {
    DateTODDataType dt1_temp, dt2_temp;
    dt1_temp.Tod.Hr = 0;
    dt1_temp.Tod.Min = 0;
    dt1_temp.Tod.Sec = 0;
    dt1_temp.Date = date1;
    dt2_temp.Tod.Hr = 0;
    dt2_temp.Tod.Min = 0;
    dt2_temp.Tod.Sec = 0;
    dt2_temp.Date = date2;
    return SubDTs(dt1_temp, dt2_temp);
}