
#include <assert.h>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <regex>
#include <vector>

#include "src/pc_emulator/include/pc_datatype.h"
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/proto/configuration.pb.h"

using namespace std;
using namespace pc_emulator;
using namespace pc_specification;


using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldInterfaceType = pc_specification::FieldInterfaceType;


void PCDataTypeField::SetExplicitStorageConstraints(int memType,
                            int ByteOffset, int BitOffset) {
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

void PCDataType::AddDataTypeField(string FieldName, string FieldTypeName,
            string InitialValue, int FieldIntfType, s64 RangeMin,
            s64 RangeMax) {
    PCDataType * DataType = LookupDataType(FieldTypeName);
    if (DataType == nullptr) {
        __configuration->PCLogger->RaiseException(
            "Field Type Name " + FieldTypeName + " is not found !");
    }

    AddDataTypeField(FieldName, FieldTypeName,
                    DataType->__DataTypeCategory, InitialValue,
                    FieldIntfType, RangeMin, RangeMax);
}


void PCDataType::AddDataTypeFieldAT(string FieldName, string FieldTypeName,
            string InitialValue, s64 RangeMin,
            s64 RangeMax, int MemType, int ByteOffset, int BitOffset) {
    PCDataType * DataType = LookupDataType(FieldTypeName);
    if (DataType == nullptr) {
        __configuration->PCLogger->RaiseException(
            "Field Type Name " + FieldTypeName + " is not found !");
    }

    // At fields can be added only to a PoU data type
    assert(__DataTypeCategory == DataTypeCategory::POU);

    AddDataTypeFieldAT(FieldName, FieldTypeName,
                    DataType->__DataTypeCategory, InitialValue,
                    RangeMin, RangeMax ,MemType, ByteOffset, BitOffset);
}

void PCDataType::AddDataTypeField(string FieldName, string FieldTypeName,
            DataTypeCategory FieldTypeCategory, string InitialValue,
            int FieldIntfType, s64 RangeMin, s64 RangeMax) {


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
                        FieldIntfType, RangeMin, RangeMax);
                    break;
            case 2 : AddArrayDataTypeField(FieldName, DataType->__DataTypeName,
                        DataType->__DimensionSizes[0], 
                        DataType->__DimensionSizes[1], InitialValue,
                        FieldIntfType, RangeMin, RangeMax);
                    break;
            default:  __configuration->PCLogger->RaiseException(
                        "Dimensions cannot exceed 2 "); 
        }
        return;
    }

    PCDataTypeField NewField(FieldName, FieldTypeName,
                            FieldTypeCategory, RangeMin, RangeMax,
                            InitialValue, FieldIntfType, DataType);

    __FieldsByInterfaceType[FieldIntfType].push_back(NewField);    
    if (FieldIntfType == FieldInterfaceType::VAR_IN_OUT ||
        FieldIntfType == FieldInterfaceType::VAR_EXTERNAL ||
        FieldIntfType == FieldInterfaceType::VAR_ACCESS ) {
        // this is a pointer
        __SizeInBits += sizeof (PCDataType *);
    } else {
        __SizeInBits += DataType->__SizeInBits;
    }
    
    __NFields ++;
    
    
}

void PCDataType::AddDataTypeFieldAT(string FieldName, string FieldTypeName,
            DataTypeCategory FieldTypeCategory, string InitialValue,
            s64 RangeMin, s64 RangeMax, int MemType, int ByteOffset,
            int BitOffset) {

    assert(__DataTypeCategory == DataTypeCategory::POU);
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
            case 1 : AddArrayDataTypeFieldAT(FieldName, DataType->__DataTypeName,
                        DataType->__DimensionSizes[0], InitialValue, RangeMin,
                        RangeMax, MemType, ByteOffset, BitOffset);
                    break;
            case 2 : AddArrayDataTypeFieldAT(FieldName, DataType->__DataTypeName,
                        DataType->__DimensionSizes[0], 
                        DataType->__DimensionSizes[1], InitialValue, RangeMin,
                        RangeMax, MemType, ByteOffset, BitOffset);
                    break;
            default:  __configuration->PCLogger->RaiseException(
                        "Dimensions cannot exceed 2 "); 
        }
        return;
    }

    PCDataTypeField NewField(FieldName, FieldTypeName,
                            FieldTypeCategory, RangeMin, RangeMax,
                            InitialValue,
                            FieldInterfaceType::VAR_EXPLICIT_STORAGE,
                            DataType);

    if (DataType->__DataTypeCategory == DataTypeCategory::BOOL)
        NewField.SetExplicitStorageConstraints(MemType, ByteOffset, BitOffset);
    else
        NewField.SetExplicitStorageConstraints(MemType, ByteOffset, 0);

    __FieldsByInterfaceType[FieldInterfaceType::VAR_EXPLICIT_STORAGE]
            .push_back(NewField);    
    
    // this is a pointer
    __SizeInBits += sizeof (PCDataType *);
    __NFields ++;

}

void PCDataType::AddArrayDataTypeField(string FieldName, string FieldTypeName,
            int DimensionSize, string InitialValue,
            int FieldIntfType, s64 RangeMin, s64 RangeMax) {

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

    std::vector<std::string> InitialValues;
    boost::split(InitialValues, InitialValue,
                boost::is_any_of(",{}"), boost::token_compress_on); 
    if (!InitialValue.empty())
        assert((int)InitialValues.size() == DimensionSize);

    for(int i = 0; i < DimensionSize; i++) {
        PCDataTypeField NewField(FieldName + "[" + std::to_string(i+1) + "]",
                                FieldTypeName, DataType->__DataTypeCategory, 
                                RangeMin, RangeMax, 
                                (InitialValue.empty() ? "" : InitialValues[i]),
                                FieldIntfType, DataType);
        __FieldsByInterfaceType[FieldIntfType].push_back(NewField);
        if (FieldIntfType == FieldInterfaceType::VAR_IN_OUT ||
            FieldIntfType == FieldInterfaceType::VAR_EXTERNAL ||
            FieldIntfType == FieldInterfaceType::VAR_ACCESS ) {
            // this is a pointer
            __SizeInBits += sizeof (PCDataType *);
        } else {
            __SizeInBits += DataType->__SizeInBits;
        }
        __NFields ++;
    }

    
    
    
}


void PCDataType::AddArrayDataTypeFieldAT(string FieldName, string FieldTypeName,
            int DimensionSize, string InitialValue, s64 RangeMin,
            s64 RangeMax, int MemType, int ByteOffset, int BitOffset) {

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

    assert(__DataTypeCategory == DataTypeCategory::POU);

    std::vector<std::string> InitialValues;
    boost::split(InitialValues, InitialValue,
                boost::is_any_of(",{}"), boost::token_compress_on); 
    if (!InitialValue.empty())
        assert((int)InitialValues.size() == DimensionSize);
    int CurrBitOffset = BitOffset;    

    for(int i = 0; i < DimensionSize; i++) {
        PCDataTypeField NewField(FieldName + "[" + std::to_string(i+1) + "]",
                                FieldTypeName, DataType->__DataTypeCategory, 
                                RangeMin, RangeMax, 
                                (InitialValue.empty() ? "" : InitialValues[i]),
                                FieldInterfaceType::VAR_EXPLICIT_STORAGE,
                                DataType);

        __FieldsByInterfaceType[FieldInterfaceType::VAR_EXPLICIT_STORAGE]
                    .push_back(NewField);

        
        if (DataType->__DataTypeCategory == DataTypeCategory::BOOL) {
            CurrBitOffset ++;
            NewField.SetExplicitStorageConstraints(MemType,
                    ByteOffset + CurrBitOffset / 8, CurrBitOffset % 8);
        } else {
            NewField.SetExplicitStorageConstraints(MemType,
                    ByteOffset + i*(DataType->__SizeInBits / 8), 0);
        }
        
    
        // this is a pointer
        __SizeInBits += sizeof (PCDataType *);
        __NFields ++;
    }

    
    
    
}

void PCDataType::AddArrayDataTypeField(string FieldName, string FieldTypeName,
            int DimensionSize1, int DimensionSize2, string InitialValue,
            int FieldIntfType, s64 RangeMin, s64 RangeMax) {

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

    std::vector<std::string> InitialValues;
    boost::split(InitialValues, InitialValue,
                boost::is_any_of(",{}"), boost::token_compress_on); 
    if (!InitialValue.empty())
        assert((int)InitialValues.size() == DimensionSize1*DimensionSize2);

    for(int i = 0; i < DimensionSize1; i++) {
        for(int j = 0; j < DimensionSize2; j++) {
            PCDataTypeField NewField(
                                FieldName + "[" + std::to_string(i+1) + "]"
                                + "[" + std::to_string(j+1) + "]",
                                FieldTypeName, DataType->__DataTypeCategory, 
                                RangeMin, RangeMax,
                                (InitialValue.empty() ? "" 
                                    : InitialValues[i*DimensionSize2 + j]),
                                FieldIntfType, DataType);
            __FieldsByInterfaceType[FieldIntfType].push_back(NewField);
            if (FieldIntfType == FieldInterfaceType::VAR_IN_OUT ||
                FieldIntfType == FieldInterfaceType::VAR_EXTERNAL ||
                FieldIntfType == FieldInterfaceType::VAR_ACCESS ) {
                // this is a pointer
                __SizeInBits += sizeof (PCDataType *);
            } else {
                __SizeInBits += DataType->__SizeInBits;
            }
            __NFields ++;
        }
    }
    
}

void PCDataType::AddArrayDataTypeFieldAT(string FieldName, string FieldTypeName,
            int DimensionSize1, int DimensionSize2, string InitialValue,
            s64 RangeMin, s64 RangeMax, int MemType, int ByteOffset,
            int BitOffset) {

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

    assert(__DataTypeCategory == DataTypeCategory::POU);
    std::vector<std::string> InitialValues;
    boost::split(InitialValues, InitialValue,
                boost::is_any_of(",{}"), boost::token_compress_on); 
    if (!InitialValue.empty())
        assert((int)InitialValues.size() == DimensionSize1*DimensionSize2);

    int CurrBitOffset = BitOffset;
    for(int i = 0; i < DimensionSize1; i++) {
        for(int j = 0; j < DimensionSize2; j++) {
            PCDataTypeField NewField(
                                FieldName + "[" + std::to_string(i+1) + "]"
                                + "[" + std::to_string(j+1) + "]",
                                FieldTypeName, DataType->__DataTypeCategory, 
                                RangeMin, RangeMax,
                                (InitialValue.empty() ? "" 
                                    : InitialValues[i*DimensionSize2 + j]),
                                FieldInterfaceType::VAR_EXPLICIT_STORAGE,
                                DataType);
            __FieldsByInterfaceType[FieldInterfaceType::VAR_EXPLICIT_STORAGE]
                        .push_back(NewField);
            
            if (DataType->__DataTypeCategory == DataTypeCategory::BOOL) {
                CurrBitOffset ++;
                NewField.SetExplicitStorageConstraints(MemType,
                        ByteOffset + CurrBitOffset / 8, CurrBitOffset % 8);
            } else {
                NewField.SetExplicitStorageConstraints(MemType,
                        ByteOffset + (i*DimensionSize2 + j)*(
                            DataType->__SizeInBits / 8), 0);
            }
            // this is a pointer
            __SizeInBits += sizeof (PCDataType *);
            __NFields ++;
        }
    }
    
}


void PCDataType::SetElementaryDataTypeAttributes(string InitialValue,
                                                s64 RangeMin, s64 RangeMax) {

    bool is_empty = InitialValue.empty();
    string InitValue, DataTypeName;
    switch(__DataTypeCategory) {
        case DataTypeCategory::BOOL :     
                            __SizeInBits = 8, DataTypeName = "BOOL";
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
                            __SizeInBits = 32, DataTypeName = "UREAL";
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

}

PCDataType * PCDataType::LookupDataType(string DataTypeName) {
    return __configuration->RegisteredDataTypes.GetDataType(
                                            DataTypeName);
}



PCDataType::PCDataType(PCConfiguration* configuration, 
                    string AliasName, string DataTypeName,
                    DataTypeCategory Category,
                    string InitialValue, s64 RangeMin,
                    s64 RangeMax) {

    
    __configuration = __configuration;
    __DataTypeName = DataTypeName;
    __NFields = 0, __SizeInBits = 0;
    __AliasName = AliasName;
    __InitialValue = InitialValue;
    __RangeMax = RangeMax;
    __RangeMin = RangeMin;

    for (int IntfType = FieldInterfaceType::VAR_INPUT; 
            IntfType != FieldInterfaceType::NA; IntfType ++) {
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
                    IntfType != FieldInterfaceType::NA; IntfType ++) {

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
                            __SizeInBits += sizeof (PCDataType *);
                        } else {
                            __SizeInBits += FieldDataType->__SizeInBits;
                        } 

                    }
                }           

        } else {
            SetElementaryDataTypeAttributes(InitialValue, RangeMin, RangeMax);
        }
    } else {
        __DataTypeCategory = Category;
        if (__DataTypeCategory != DataTypeCategory::DERIVED &&
            __DataTypeCategory != DataTypeCategory::POU) {
                SetElementaryDataTypeAttributes(InitialValue,
                        RangeMin, RangeMax);
        } /*else {
            __configuration->PCLogger->RaiseException("Data type category must"
                " be not assigned for complex data types!\n");
        }*/
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
 

    for (int IntfType = FieldInterfaceType::VAR_INPUT; 
            IntfType != FieldInterfaceType::NA; IntfType ++) {
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
    boost::split(InitialValues, InitialValue,
                boost::is_any_of(",{}"), boost::token_compress_on); 
    assert (InitialValues.empty() ||
            (int)InitialValues.size() == DimSize);                                       

    for(int i = 0; i < DimSize; i++) {
        string Init = InitialValues.empty() ? DataType->__InitialValue :
                            InitialValues[i];
        PCDataTypeField NewField(
                            "[" + std::to_string(i+1) + "]", 
                            DataTypeName,
                            DataType->__DataTypeCategory, 
                            __RangeMin, __RangeMax,
                            Init, FieldInterfaceType::NA, DataType);
        __FieldsByInterfaceType[FieldInterfaceType::NA].push_back(
            NewField);
        __NFields ++;
        __SizeInBits += DataType->__SizeInBits;
        
    }
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
    

    for (int IntfType = FieldInterfaceType::VAR_INPUT; 
            IntfType != FieldInterfaceType::NA; IntfType ++) {
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
    boost::split(InitialValues, InitialValue,
                boost::is_any_of(",{}"), boost::token_compress_on); 
    assert (InitialValues.empty() ||
            (int)InitialValues.size() == Dim1Size*Dim2Size);                                       

    for(int i = 0; i < Dim1Size; i++) {
        for(int j = 0; j < Dim2Size; j++) {
            string Init = InitialValues.empty() ? DataType->__InitialValue :
                                InitialValues[i*Dim2Size + j];
            PCDataTypeField NewField(
                    "[" + std::to_string(i+1) + "][" + std::to_string(j+1) + "]", 
                    DataTypeName,
                    DataType->__DataTypeCategory, 
                    __RangeMin, __RangeMax,
                    Init, FieldInterfaceType::NA, DataType);
            __FieldsByInterfaceType[FieldInterfaceType::NA].push_back(
                NewField);
            __NFields ++;
            __SizeInBits += DataType->__SizeInBits;
        }
    }
    __DimensionSizes.push_back(Dim1Size);
    __DimensionSizes.push_back(Dim2Size);
}

void PCDataType::RegisterDataType() {
    __configuration->RegisteredDataTypes.RegisterDataType(__AliasName,
            this);
}

bool PCDataType::CheckRemFields(std::vector<string>& NestedFields, int StartPos,
                            PCDataType * Current) {
    if (StartPos >= (int)NestedFields.size())
        return true;

    for (int i = StartPos ; i < (int)NestedFields.size(); i++) {
        string AccessedFieldName = NestedFields[i];
        for (int IntfType = FieldInterfaceType::VAR_INPUT; 
            IntfType != FieldInterfaceType::NA + 1; IntfType ++) {
            for(auto& DefinedField: Current->__FieldsByInterfaceType[IntfType]) {
                PCDataType * FieldDataType = DefinedField.__FieldTypePtr;
                assert(FieldDataType != nullptr);
                if(AccessedFieldName == DefinedField.__FieldName) {
                    return CheckRemFields(NestedFields, i+1, FieldDataType);
                }
            }
        }
    }
    return false;
}


bool PCDataType::IsFieldPresent(string NestedFieldName) {
    std::vector<std::string> NestedFields;
    boost::split(NestedFields, NestedFieldName,
                boost::is_any_of("."), boost::token_compress_on);
    if (NestedFields.empty()) {
        return true;           
    }
    
    return CheckRemFields(NestedFields, 0, this);

}


bool DataTypeUtils::ValueToBool(string Value, bool& BoolValue) {
    if(boost::iequals(Value, "True") 
        || boost::iequals(Value, "1")) {
        BoolValue = true;
        return true;
    }

    if(boost::iequals(Value, "False") 
        || boost::iequals(Value, "0")) {
        BoolValue = false;
        return true;
    }

    return false;
}
bool DataTypeUtils::ValueToByte(string Value, int8_t & ByteValue) {
    // Value can be between 16#0 to 16#FF
    if (boost::starts_with(Value, "16#")
            && Value.length() <= 5) {
        string BitString = "0x" + Value.substr(3,Value.length() - 3);
        auto Cast = boost::lexical_cast<int>(BitString);
        ByteValue = Cast & 0x00FF;
        return true;
    }
    return false;
}

bool DataTypeUtils::ValueToWord(string Value, int16_t & WordValue) {

    if (boost::starts_with(Value, "16#")
            && Value.length() <= 7) {
        string BitString = "0x" + Value.substr(3,Value.length() - 3);
        auto Cast = boost::lexical_cast<int>(BitString);
        WordValue = Cast & 0x00FFFF;
        return true;
    }
    return false;

}
bool DataTypeUtils::ValueToDWord(string Value, int32_t& DWordValue) {
    if (boost::starts_with(Value, "16#")
            && Value.length() <= 11) {
        string BitString = "0x" + Value.substr(3,Value.length() - 3);
        auto Cast = boost::lexical_cast<int32_t>(BitString);
        DWordValue = Cast & 0x00FFFFFFFF;
        return true;
    }
    return false;

}
bool DataTypeUtils::ValueToLWord(string Value, int64_t & LWordValue) {
    if (boost::starts_with(Value, "16#")
            && Value.length() <= 19) {
        string BitString = "0x" + Value.substr(3,Value.length() - 3);
        auto Cast = boost::lexical_cast<int64_t>(BitString);
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
    RealValue = boost::lexical_cast<double>(Value);
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
            std::regex("^((?:[01]\d|2[0-3]):[0-5]\d:[0-5]\d$)"))){
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
            std::regex("([12]\d{3}-(0[1-9]|1[0-2])-(0[1-9]|[12]\d|3[01]))"))
        && std::regex_match(Value.substr(14,8),
            std::regex("^((?:[01]\d|2[0-3]):[0-5]\d:[0-5]\d$)"))){
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
            std::regex("([12]\d{3}-(0[1-9]|1[0-2])-(0[1-9]|[12]\d|3[01]))"))){
        Date.Year = boost::lexical_cast<int>(Value.substr(2,4));
        Date.Month = boost::lexical_cast<int>(Value.substr(7,2));
        Date.Day = boost::lexical_cast<int>(Value.substr(10,2));
        return true;
    }
    return false;
}
