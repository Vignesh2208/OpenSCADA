
#include <assert>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <regex>

#include "pc_emulator/include/pc_datatype.h"
#include "pc_emulator/include/pc_configuration.h"

using namespace std;
using namespace pc_emulator;

void PCDataType::AddDataTypeField(string FieldName, string FieldTypeName,
            string InitialValue, int FieldInterfaceType, s64 RangeMin,
            s64 RangeMax) {
    PCDataType * DataType = LookupDataType(FieldTypeName);
    if (DataType == nullptr) {
        __configuration->PCLogger->RaiseException(
            "Field Type Name " + FieldTypeName + " is not found !");
    }

    AddDataTypeField(FieldName, FieldTypeName,
                    DataType->__DataTypeCategory, InitialValue,
                    FieldInterfaceType, RangeMin, RangeMax);
}

void PCDataType:::AddDataTypeField(string FieldName, string FieldTypeName,
            DataTypeCategories FieldTypeCategory, string InitialValue,
            int FieldInterfaceType, s64 RangeMin, s64 RangeMax) {


    PCDataType * DataType = LookupDataType(FieldTypeName);
    if (DataType == nullptr) {
        __configuration->PCLogger->RaiseException(
            "Field Type Name " + FieldTypeName + " is not found !");
    }
    if (DataType->__DataTypeCategory == DataTypeCategories::POU 
            || DataType->__DataTypeCategory == DataTypeCategories::DERIVED) {
        if (!InitialValue.empty()) {
            __configuration->PCLogger->RaiseException("Initial Values cannot "
                "be specified for complex data types");
        }
    } 
    
    if(__DataTypeCategory == DataTypeCategories::ARRAY) {
        __configuration->PCLogger->RaiseException("New fields cannot be "
            "added to a data type of ARRAY category");
    }

    if (DataType->__DataTypeCategory == DataTypeCategories::ARRAY) {
        switch(DataType->__DimensionSizes.length()) {
            case 1 : AddArrayDataTypeField(FieldName, DataType->__DataTypeName,
                        DataType->__DimensionSizes[0], InitialValue,
                        FieldInterfaceType, RangeMin, RangeMax);
                    break;
            case 2 : AddArrayDataTypeField(FieldName, DataType->__DataTypeName,
                        DataType->__DimensionSizes[0], 
                        DataType->__DimensionSizes[1], InitialValue,
                        FieldInterfaceType, RangeMin, RangeMax);
                    break;
            default:  __configuration->PCLogger->RaiseException(
                        "Dimensions cannot exceed 2 "); 
        }
        return;
    }

    PCDataTypeField NewField(FieldName, FieldTypeName,
                            FieldTypeCategory, RangeMin, RangeMax,
                            InitialValue, FieldInterfaceType, DataType);

    __FieldsByInterfaceType[FieldInterfaceType].push_back(NewField);    
    if (FieldInterfaceType == FIELD_INTERFACE_TYPES::VAR_IN_OUT ||
        FieldInterfaceType == FIELD_INTERFACE_TYPES::VAR_EXTERNAL ||
        FieldInterfaceType == FIELD_INTERFACE_TYPES::VAR_ACCESS ) {
        // this is a pointer
        __SizeInBits += sizeof (PCDataType *);
    } else {
        __SizeInBits += DataType->__SizeInBits;
    }
    
    __NFields ++;
    
    
}

void PCDataType:::AddArrayDataTypeField(string FieldName, string FieldTypeName,
            int DimensionSize, string InitialValue,
            int FieldInterfaceType, s64 RangeMin, s64 RangeMax) {

    PCDataType * DataType = LookupDataType(FieldTypeName);
    if (DataType == nullptr) {
        __configuration->PCLogger->RaiseException(
            "Field Type Name " + FieldTypeName + " is not found !");
    }

    if (DataType->__DataTypeCategory == DataTypeCategories::POU 
            || DataType->__DataTypeCategory == DataTypeCategories::DERIVED) {
        if (!InitialValue.empty()) {
            __configuration->PCLogger->RaiseException("Initial Values cannot "
                "be specified for complex data types");
        }
    }

    if(__DataTypeCategory == DataTypeCategories::ARRAY) {
        __configuration->PCLogger->RaiseException("New fields cannot be "
            "added to a data type of ARRAY category");
    }

    std::vector<std::string> InitialValues;
    boost::split(InitalValues, InitialValue,
                boost::is_any_of(",{}"), boost::token_compress_on); 
    if (!InitialValue.empty())
        assert(InitialValues.length() == DimensionSize);

    for(int i = 0; i < DimensionSize; i++) {
        PCDataTypeField NewField(FieldName + "[" + std::to_string(i+1) + "]",
                                FieldTypeName, DataType->__DataTypeCategory, 
                                RangeMin, RangeMax, 
                                (InitialValue.empty() ? "" : InitialValues[i]),
                                FieldInterfaceType, DataType);
        __FieldsByInterfaceType[FieldInterfaceType].push_back(NewField);
        if (FieldInterfaceType == FIELD_INTERFACE_TYPES::VAR_IN_OUT ||
            FieldInterfaceType == FIELD_INTERFACE_TYPES::VAR_EXTERNAL ||
            FieldInterfaceType == FIELD_INTERFACE_TYPES::VAR_ACCESS ) {
            // this is a pointer
            __SizeInBits += sizeof (PCDataType *);
        } else {
            __SizeInBits += DataType->__SizeInBits;
        }
        __NFields ++;
    }

    
    
    
}

void PCDataType::AddArrayDataTypeField(string FieldName, string FieldTypeName,
            int DimensionSize1, int DimesionSize2, string InitialValue,
            int FieldInterfaceType, s64 RangeMin, s64 RangeMax) {

    PCDataType * DataType = LookupDataType(FieldTypeName);
    if (DataType == nullptr) {
        __configuration->PCLogger->RaiseException(
            "Field Type Name " + FieldTypeName + " is not found !");
    }

    if (DataType->__DataTypeCategory == DataTypeCategories::POU 
            || DataType->__DataTypeCategory == DataTypeCategories::DERIVED) {
        if (!InitialValue.empty()) {
            __configuration->PCLogger->RaiseException("Initial Values cannot "
                "be specified for complex data types");
        }
    }

    if(__DataTypeCategory == DataTypeCategories::ARRAY) {
        __configuration->PCLogger->RaiseException("New fields cannot be "
            "added to a data type of ARRAY category");
    }

    std::vector<std::string> InitialValues;
    boost::split(InitalValues, InitialValue,
                boost::is_any_of(",{}"), boost::token_compress_on); 
    if (!InitialValue.empty())
        assert(InitialValues.length() == DimensionSize1*DimensionSize2);

    for(int i = 0; i < DimensionSize1; i++) {
        for(int j = 0; j < DimensionSize2; j++) {
            PCDataTypeField NewField(
                                FieldName + "[" + std::to_string(i+1) + "]"
                                + "[" + std::to_string(j+1) + "]",
                                FieldTypeName, DataType->__DataTypeCategory, 
                                RangeMin, RangeMax
                                (InitialValue.empty() ? "" 
                                    : InitialValues[i*DimensionSize2 + j]),
                                FieldInterfaceType, DataType);
            __FieldsByInterfaceType[FieldInterfaceType].push_back(NewField);
            if (FieldInterfaceType == FIELD_INTERFACE_TYPES::VAR_IN_OUT ||
                FieldInterfaceType == FIELD_INTERFACE_TYPES::VAR_EXTERNAL ||
                FieldInterfaceType == FIELD_INTERFACE_TYPES::VAR_ACCESS ) {
                // this is a pointer
                __SizeInBits += sizeof (PCDataType *);
            } else {
                __SizeInBits += DataType->__SizeInBits;
            }
            __NFields ++;
        }
    }
    
}

void PCDataType::SetElementaryDataTypeAttributes(string InitialValue,
                                                s64 RangeMin, s64 RangeMax) {

    bool is_empty = InitialValue.empty();
    string InitValue, DataTypeName;
    switch(__DataTypeCategory) {
        case DataTypeCategories::BOOL :     
                            __SizeInBits = 8, DataTypeName = "BOOL";
                            InitValue = is_empty ? "0" : InitialValue;
                            break;
        case DataTypeCategories::BYTE :     
                            __SizeInBits = 8, DataTypeName = "BYTE";
                            InitValue = is_empty ? "16#0" : InitialValue;
                            break;
        case DataTypeCategories::WORD :     
                            __SizeInBits = 16, DataTypeName = "WORD";
                            InitValue = is_empty ? "16#0" : InitialValue;
                            break;
        case DataTypeCategories::DWORD :     
                            __SizeInBits = 32, DataTypeName = "DWORD";
                            InitValue = is_empty ? "16#0" : InitialValue;
                            break;
        case DataTypeCategories::LWORD :    
                            __SizeInBits = 64, DataTypeName = "LWORD";
                            InitValue = is_empty ? "16#0" : InitialValue;
                            break;
        case DataTypeCategories::CHAR :    
                            __SizeInBits = 8, DataTypeName = "CHAR";
                            InitValue = is_empty ? "" : InitialValue;
                            break;
        case DataTypeCategories::INT :      
                            __SizeInBits = 16, DataTypeName = "INT";
                            InitValue = is_empty ? "0" : InitialValue;
                            break;
        case DataTypeCategories::SINT :     
                            __SizeInBits = 8, DataTypeName = "SINT";
                            InitValue = is_empty ? "0" : InitialValue;
                            break;
        case DataTypeCategories::DINT :     
                            __SizeInBits = 32, DataTypeName = "DINT";
                            InitValue = is_empty ? "0" : InitialValue;
                            break;
        case DataTypeCategories::LINT :     
                            __SizeInBits = 64, DataTypeName = "LINT";
                            InitValue = is_empty ? "0" : InitialValue;
                            break;
        case DataTypeCategories::UINT :     
                            __SizeInBits = 16, DataTypeName = "UINT";
                            InitValue = is_empty ? "0" : InitialValue;
                            break;
        case DataTypeCategories::USINT :     
                            __SizeInBits = 8, DataTypeName = "USINT";
                            InitValue = is_empty ? "0" : InitialValue;
                            break;
        case DataTypeCategories::UDINT :     
                            __SizeInBits = 32, DataTypeName = "UDINT";
                            InitValue = is_empty ? "0" : InitialValue;
                            break;
        case DataTypeCategories::ULINT :     
                            __SizeInBits = 64, DataTypeName = "ULINT";
                            InitValue = is_empty ? "0" : InitialValue;
                            break;
        case DataTypeCategories::REAL :     
                            __SizeInBits = 32, DataTypeName = "UREAL";
                            InitValue = is_empty ? "0.0" : InitialValue;
                            break;
        case DataTypeCategories::LREAL :     
                            __SizeInBits = 64, DataTypeName = "LREAL";
                            InitValue = is_empty ? "0.0" : InitialValue;
                            break;
        case DataTypeCategories::TIME :     
                            __SizeInBits 
                                = sizeof(pc_emulator::Time)*8, DataTypeName = "TIME";
                            InitValue = is_empty ? "t#0s" : InitialValue;
                            break;
        case DataTypeCategories::DATE :     
                            __SizeInBits 
                                = sizeof(pc_emulator::Date)*8, DataTypeName = "DATE";
                            InitValue = is_empty ? "d#0001-01-01" : InitialValue;
                            break;
        case DataTypeCategories::TIME_OF_DAY :     
                            __SizeInBits 
                                = sizeof(pc_emulator::TOD)*8, DataTypeName = "TOD";
                            InitValue = is_empty ? "tod#00:00:00" : InitialValue;
                            break;
        case DataTypeCategories::DATE_AND_TIME :     
                            __SizeInBits 
                                = sizeof(pc_emulator::DateTOD)*8, DataTypeName = "DT";
                            InitValue = is_empty ? "dt#0001-01-01-00:00:00" 
                                            : InitialValue;
                            break;
        default :           return;
    }

    __NFields = 0; // This will be 0 only for elementary data types
    //AddDataTypeField("@", DataTypeName, DataTypeCategory, InitValue, 
    //                RangeMin, RangeMax);
    __RangeMax = RangeMax
    __RangeMin = RangeMin;
    __InitialValue = InitValue;

}

PCDataType * PCDataType::LookupDataType(string DataTypeName) {
    return __configuration->RegisteredDataTypes.GetDataType(
                                            DataTypeName);
}



PCDataType::PCDataType(PCConfiguration* configuration, 
                    string AliasName, string DataTypeName,
                    DataTypeCategories Category = DataTypeCategories::NOT_ASSIGNED,
                    string InitialValue="", s64 RangeMin = LLONG_MIN,
                    s64 RangeMax = LLONG_MAX) {

    
    __configuration = __configuration;
    __DataTypeName = DataTypeName;
    __NFields = 0, __SizeInBits = 0;
    __AliasName = AliasName;
    __InitialValue = InitialValue;
    __RangeMax = RangeMax;
    __RangeMin = RangeMin;

    for (int IntfType = FIELD_INTERFACE_TYPES::VAR_INPUT; 
            IntfType != FIELD_INTERFACE_TYPES::NA; IntfType ++) {
        __FieldsByInterfaceType.insert(
            std::pair<int,vector<PCDataTypeField> >(
                IntfType, vector<PCDataTypeField>()));
    }
    

    // Since size of dimensions is not specified, it cannot be an array or
    // string
    assert (Category != DataTypeCategories::ARRAY);

    if (LookupDataType(AliasName) != nullptr) {
            __configuration->PCLogger->RaiseException("Alias Name " + AliasName
                    " has already been used !");
    }        
    

    if (Category == DataTypeCategories::NOT_ASSIGNED) {
        // Since DataTypeCategory is not provided, this DataTypeName must have
        // already been registered
        PCDataType * DataType = LookupDataType(DataTypeName);
        assert (DataType != nullptr);
        __DataTypeCategory = DataType->__DataTypeCategory;
        
        if (__DataTypeCategory == DataTypeCategories::DERIVED
            || __DataTypeCategory == DataTypeCategories::POU) {
                if (!InitialValue.empty()) {
                    __configuration->PCLogger->RaiseException(
                            "Cannot specify initial values for complex "
                            "data types");
                }

                for (int IntfType = FIELD_INTERFACE_TYPES::VAR_INPUT; 
                    IntfType != FIELD_INTERFACE_TYPES::NA; IntfType ++) {

                    // This is like a Typedef of a derived data type/function block
                    // Copy all the fields of the derived data type/function block
                    for (auto& field: 
                            DataType->__FieldsByInterfaceType[IntfType]) {
                        PCDataType * FieldDataType = LookupDataType(
                                                        field.FieldTypeName);
                        if (!FieldDataType) {
                            __configuration->PCLogger->RaiseException(
                                "Field DataType Not found for " 
                                + field.FieldTypeName);
                        }
                        __FieldsByInterfaceType[FieldInterfaceType].push_back(
                            field);
                        __NFields ++;

                        if (FieldInterfaceType 
                                == FIELD_INTERFACE_TYPES::VAR_IN_OUT || 
                            FieldInterfaceType 
                                == FIELD_INTERFACE_TYPES::VAR_EXTERNAL ||
                            FieldInterfaceType 
                                == FIELD_INTERFACE_TYPES::VAR_ACCESS ) {
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
        if (__DataTypeCategory != DataTypeCategories::DERIVED &&
            __DataTypeCategory != DataTypeCategories::POU) {
                SetElementaryDataTypeAttributes(InitialValue,
                        RangeMin, RangeMax);
        } else {
            __configuration->PCLogger->RaiseException("Data type category must"
                " be not assigned for complex data types!\n");
        }
    }
    __DimensionSizes.append(1);
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
            string InitialValue="") {

    __configuration = configuration;
    __DataTypeName = DataTypeName;
    __NFields = 0, __SizeInBits = 0;
    __AliasName = AliasName;
    __InitialValue = InitialValue;
    __RangeMax = RangeMax;
    __RangeMin = RangeMin;

    for (int IntfType = FIELD_INTERFACE_TYPES::VAR_INPUT; 
            IntfType != FIELD_INTERFACE_TYPES::NA; IntfType ++) {
        __FieldsByInterfaceType.insert(
            std::pair<int,vector<PCDataTypeField> >(
                IntfType, vector<PCDataTypeField>()));
    }

    if (LookupDataType(AliasName) != nullptr) {
            __configuration->PCLogger->RaiseException("Alias Name " + AliasName
                    " has already been used !");
    }

    PCDataType * DataType = LookupDataType(DataTypeName);
    __DataTypeCategory = DataTypeCategories::ARRAY;
    assert(DataType != nullptr);
    if ((DataType->__DataTypeCategory == DataTypeCategories::DERIVED ||
            DataType->__DataTypeCategory == DataTypeCategories::POU)
        && !InitialValue.empty()) {
            __configuration->PCLogger->RaiseException("Initial Values cannot"
                " be specified for arrays of complex types!\n");
    }

    __RangeMax = (DataType->RangeMax < RangeMax) ? DataType->RangeMax :
                                                        RangeMax;

    __RangeMin = (DataType->RangeMin > RangeMin) ? DataType->RangeMin :
                                                        RangeMin;

    

    std::vector<std::string> InitialValues;
    boost::split(InitalValues, InitialValue,
                boost::is_any_of(",{}"), boost::token_compress_on); 
    assert (InitialValues.empty() ||
            InitialValues.length() == DimSize);                                       

    for(int i = 0; i < DimSize; i++) {
        string Init = InitialValues.empty() ? DataType->InitialValue :
                            InitialValues[i];
        PCDataTypeField NewField(
                            "[" + std::to_string(i+1) + "]", 
                            DataTypeName,
                            DataType->__DataTypeCategory, 
                            __RangeMin, __RangeMax,
                            Init, FIELD_INTERFACE_TYPES::NA, DataType);
        __FieldsByInterfaceType[FIELD_INTERFACE_TYPES::NA].push_back(
            NewField);
        __NFields ++;
        __SizeInBits += DataType->__SizeInBits;
        
    }
    __DimensionSizes.append(DimSize);
}


PCDataType::PCDataType(PCConfiguration* configuration, 
            string AliasName, string DataTypeName, int Dim1Size, int Dim2Size, 
            string InitialValue="", 
            s64 RangeMin = LLONG_MIN, s64 RangeMax = LLONG_MAX) {

    __configuration = configuration;
    __DataTypeName = DataTypeName;
    __NFields = 0, __SizeInBits = 0;
    __AliasName = AliasName;
    __InitialValue = InitialValue;
    __RangeMax = RangeMax;
    __RangeMin = RangeMin;

    for (int IntfType = FIELD_INTERFACE_TYPES::VAR_INPUT; 
            IntfType != FIELD_INTERFACE_TYPES::NA; IntfType ++) {
        __FieldsByInterfaceType.insert(
            std::pair<int,vector<PCDataTypeField> >(
                IntfType, vector<PCDataTypeField>()));
    }

    if (LookupDataType(AliasName) != nullptr) {
            __configuration->PCLogger->RaiseException("Alias Name " + AliasName
                    " has already been used !");
    }

    PCDataType * DataType = LookupDataType(DataTypeName);
    assert(DataType != nullptr);
    if ((DataType->__DataTypeCategory == DataTypeCategories::DERIVED ||
            DataType->__DataTypeCategory == DataTypeCategories::POU)
        && !InitialValue.empty()) {
            __configuration->PCLogger->RaiseException("Initial Values cannot"
                " be specified for arrays of complex types!\n");
    }

    __DataTypeCategory = DataTypeCategories::ARRAY;

    __RangeMax = (DataType->RangeMax < RangeMax) ? DataType->RangeMax :
                                                        RangeMax;
    __RangeMin = (DataType->RangeMin > RangeMin) ? DataType->RangeMin :
                                                        RangeMin;
                                                        
    std::vector<std::string> InitialValues;
    boost::split(InitalValues, InitialValue,
                boost::is_any_of(",{}"), boost::token_compress_on); 
    assert (InitialValues.empty() ||
            InitialValues.length() == Dim1Size*Dim2Size);                                       

    for(int i = 0; i < Dim1Size; i++) {
        for(int j = 0; j < Dim2Size; j++) {
            string Init = InitialValues.empty() ? DataType->InitialValue :
                                InitialValues[i*Dim2Size + j];
            PCDataTypeField NewField(
                    "[" + std::to_string(i+1) + "][" + std::to_string(j+1) + "]", 
                    DataTypeName,
                    DataType->__DataTypeCategory, 
                    __RangeMin, __RangeMax,
                    Init, FIELD_INTERFACE_TYPES::NA, DataType);
            __FieldsByInterfaceType[FIELD_INTERFACE_TYPES::NA].push_back(
                NewField);
            __NFields ++;
            __SizeInBits += DataType->__SizeInBits;
        }
    }
    __DimensionSizes.append(Dim1Size);
    __DimensionSizes.append(Dim2Size);
}

void PCDataType::RegisterDataType() {
    __configuration->RegisteredDataTypes.RegisterDataType(__AliasName,
            this);
}


void PCDataType::GetFieldAttributes(string NestedFieldName,
                                    DataTypeFieldAttributes& FieldAttributes) {
    std::vector<std::string> NestedFields;
    boost::split(NestedFields, NestedFieldName,
                boost::is_any_of("."), boost::token_compress_on);
    if (NestedFields.empty()) {
        
        FieldAttributes.RelativeOffset = 0;
        FieldAttributes.FieldInterfaceType = FIELD_INTERFACE_TYPES::NA;
        FieldAttributes.SizeInBits = __SizeInBits;
        FieldAttributes.FieldDataTypePtr = this;
        return;
            
    }

    PCDataType * DataType = this;
    FieldAttributes.RelativeOffset = 0;
    for (auto& AccessedFieldName : NestedFields) {
        for (int IntfType = FIELD_INTERFACE_TYPES::VAR_INPUT; 
            IntfType != FIELD_INTERFACE_TYPES::NA; IntfType ++) {
            for(auto& DefinedField: DataType->__FieldsByInterfaceType[IntfType]) {
                PCDataType * FieldDataType = DefinedField.__FieldTypePtr;
                assert(FieldDataType != nullptr);
                if(AccessedFieldName == DefinedField.__FieldName) {
                    DataType = FieldDataType;

                    if (DefinedField.__FieldInterfaceType 
                            == FIELD_INTERFACE_TYPES::VAR_IN_OUT || 
                        DefinedField.__FieldInterfaceType 
                            == FIELD_INTERFACE_TYPES::VAR_EXTERNAL ||
                        DefinedField.__FieldInterfaceType 
                            == FIELD_INTERFACE_TYPES::VAR_ACCESS ) {
                        FieldAttributes.SizeInBits = sizeof (PCDataType *);
                    } else {
                        FieldAttributes.SizeInBits = DataType->__SizeInBits;
                    }
                    FieldAttributes.FieldInterfaceType 
                            = DefinedField.__FieldInterfaceType;
                    FieldAttributes.FieldDataTypePtr = DataType;
                    return;
                } else {
                    if (DefinedField.__FieldInterfaceType 
                            == FIELD_INTERFACE_TYPES::VAR_IN_OUT || 
                        DefinedField.__FieldInterfaceType 
                            == FIELD_INTERFACE_TYPES::VAR_EXTERNAL ||
                        DefinedField.__FieldInterfaceType 
                            == FIELD_INTERFACE_TYPES::VAR_ACCESS ) {
                        // this is a pointer
                        FieldAttributes.RelativeOffset += sizeof (PCDataType *);
                    } else {
                        FieldAttributes.RelativeOffset 
                                += FieldDataType->__SizeInBits;
                    }
                }
            }
        }
    }
    
    __configuration->PCLogger->RaiseException("Accessed field not found!");
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
        CharValue = '';
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
