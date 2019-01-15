#include <iostream>
#include <cstdint>
#include <cstring>

#include "pc_emulator/include/pc_variable.h"
#include "pc_emulator/include/pc_datatype.h"

using namespace std;
using namespace pc_emulator;

PCVariable::PCVariable(PCConfiguration * configuration,
                string VariableName,
                string VariableDataTypeName) {

    //by default treat variables as IN_OUT unless explicitly stated.
    __configuration = configuration;
    assert(__configuration != nullptr);
    __VariableDataType = __configuration->RegisteredDataTypes.GetDataType(
                                            VariableDataTypeName);
    if (!__VariableDataType) {
        __configuration->PCLogger->RaiseException("Data type " 
                + VariableDataTypeName + " not registered !");
    }

    __ByteOffset = 0;
    __BitOffset = 0;
    __VariableName = VariableName;
    __MemoryLocation = PCMemUnit::PCMemUnit();
}

void PCVariable::AllocateStorage() {

    if (! __MemoryLocation.IsInitialized()) {
        int DataTypeSizeBytes = (__VariableDataType->__SizeInBits)/8;

        DataTypeSizeBytes = DataTypeSizeBytes > 0 ? DataTypeSizeBytes : 1;

        __MemoryLocation.AllocateStaticMemory(DataTypeSizeBytes);
    }
}


PCVariable PCVariable::GetPCVariableToField(string NestedFieldName) {
    DataTypeFieldAttributes Attributes;
    
    if (NestedFieldName.empty())
        return *this;
    __VariableDataType->GetFieldAttributes(NestedFieldName, Attributes);

    if (Attributes.FieldInterfaceType != FIELD_INTERFACE_TYPES::VAR_IN_OUT
        && Attributes.FieldInterfaceType != FIELD_INTERFACE_TYPES::VAR_EXTERNAL
        && Attributes.FieldInterfaceType != FIELD_INTERFACE_TYPES::VAR_ACCESS) {
        PCVariable VariablePtrToField(__configuration,
                                    __VariableName + NestedFieldName,
                                    Attributes.FieldDataTypePtr->__DataTypeName);
        VariablePtrToField.__ByteOffset 
                        = __ByteOffset + Attributes.RelativeOffset / 8;
        VariablePtrToField.__BitOffset 
                        = __BitOffset + Attributes.RelativeOffset % 8;
        VariablePtrToField.__MemoryLocation.SetMemUnitLocation(
                            (PCMemUnit *)&this->__MemoryLocation);

        return VariablePtrToField;
    } else {
        PCVariable * VariablePtrToField;
        char * PtrToStorageLoc = (char *) this->__MemoryLocation.GetPointerToMemory(
                                __ByteOffset + (Attributes.RelativeOffset / 8));
        memcpy(&VariablePtrToField, PtrToStorageLoc, sizeof(PCVariable *));
        return *VariablePtrToField;
    }
}

void PCVariable::CopyPCVariableFieldFromPointer(string NestedFieldName,
                PCVariable * From) {
    
    if (NestedFieldName.empty()) {
        //  Copy whole content of From except variable name.
        __ByteOffset = From->__ByteOffset;
        __BitOffset = From->__BitOffset;
        assert(this->__VariableDataType->__DataTypeName
            == From->__VariableDataType->__DataTypeName);
        AllocateStorage();
        this->__MemoryLocation.CopyFromMemUnit(&From->__MemoryLocation, 0, 
                From->__MemoryLocation.GetMemUnitSize(), 0);
        return;

    }

    DataTypeFieldAttributes Attributes;
    __VariableDataType->GetFieldAttributes(NestedFieldName, Attributes);

    assert(Attributes.FieldDataTypePtr->__DataTypeName
            == From->__VariableDataType->__DataTypeName);
    assert(this->__MemoryLocation.IsInitialized());


    if (Attributes.FieldInterfaceType != FIELD_INTERFACE_TYPES::VAR_IN_OUT
        && Attributes.FieldInterfaceType != FIELD_INTERFACE_TYPES::VAR_EXTERNAL
        && Attributes.FieldInterfaceType != FIELD_INTERFACE_TYPES::VAR_ACCESS) {
            // The content pointed to by From is copied at the appropriate
            // offset

            assert(__ByteOffset + (Attributes.RelativeOffset / 8) 
            + From->__MemoryLocation.GetMemUnitSize() - From->__ByteOffset
             <= this->__MemoryLocation.GetMemUnitSize());
            
            this->__MemoryLocation.CopyFromMemUnit(&From->__MemoryLocation, 
                From->__ByteOffset,
                From->__MemoryLocation.GetMemUnitSize() - From->__ByteOffset,
                __ByteOffset + (Attributes.RelativeOffset / 8));


    } else {
        // The pointer From is itself to be copied at the appropriate offset

        assert(__ByteOffset + (Attributes.RelativeOffset / 8) 
        + sizeof(PCVariable *) <= this->__MemoryLocation.GetMemUnitSize());

        std::memcpy(
            &this->__MemoryLocation.GetStorageLocation().get()[
                __ByteOffset + (Attributes.RelativeOffset / 8)],
            &From, sizeof(PCVariable *));
    }

}

void PCVariable::SetPCVariableField(string NestedFieldName, string Value) {

    DataTypeFieldAttributes Attributes;
    __VariableDataType->GetFieldAttributes(NestedFieldName, Attributes);

    if (Attributes.FieldInterfaceType == FIELD_INTERFACE_TYPES::VAR_IN_OUT
        || Attributes.FieldInterfaceType == FIELD_INTERFACE_TYPES::VAR_EXTERNAL
        || Attributes.FieldInterfaceType == FIELD_INTERFACE_TYPES::VAR_ACCESS) {
            // it is a pointer, we must get the pointed variable and set it there
            auto PointedVariable  =  GetPCVariableToField(
                     NestedFieldName);       
            PointedVariable.SetPCVariableField("", Value);
            return;
    }

    assert(this->__MemoryLocation.IsInitialized());

    switch(Attributes.FieldDataTypePtr->__DataTypeCategory) {
        case DataTypeCategories::BOOL :     
                            bool BoolValue;
                            if (!DataTypeUtils::ValueToBool(Value, BoolValue)){
                                __configuration->PCLogger->RaiseException(
                                    "Bool conversion error !");
                            }
                            std::memcpy(
                            &this->__MemoryLocation.GetStorageLocation().get()[
                                __ByteOffset + (Attributes.RelativeOffset / 8)],
                                &BoolValue, sizeof(bool));
                            break;
        case DataTypeCategories::BYTE :     
                            int8_t ByteValue;
                            if (!DataTypeUtils::ValueToByte(Value, ByteValue)){
                                __configuration->PCLogger->RaiseException(
                                    "Byte conversion error !");
                            }
                            std::memcpy(
                            &this->__MemoryLocation.GetStorageLocation().get()[
                                __ByteOffset + (Attributes.RelativeOffset / 8)],
                                &ByteValue, sizeof(int8_t));
                            break;
        case DataTypeCategories::WORD :     
                            int16_t WordValue;
                            if (!DataTypeUtils::ValueToWord(Value, WordValue)){
                                __configuration->PCLogger->RaiseException(
                                    "Word conversion error !");
                            }
                            std::memcpy(
                            &this->__MemoryLocation.GetStorageLocation().get()[
                                __ByteOffset + (Attributes.RelativeOffset / 8)],
                                &WordValue, sizeof(int16_t));
                            break;
        case DataTypeCategories::DWORD :     
                            int32_t DWordValue;
                            if (!DataTypeUtils::ValueToDWord(Value, DWordValue)){
                                __configuration->PCLogger->RaiseException(
                                    "DWord conversion error !");
                            }
                            std::memcpy(
                            &this->__MemoryLocation.GetStorageLocation().get()[
                                __ByteOffset + (Attributes.RelativeOffset / 8)],
                                &DWordValue, sizeof(int32_t));
                            break;
        case DataTypeCategories::LWORD :    
                            int64_t LWordValue;
                            if (!DataTypeUtils::ValueToLWord(Value, LWordValue)){
                                __configuration->PCLogger->RaiseException(
                                    "LWord conversion error !");
                            }
                            std::memcpy(
                            &this->__MemoryLocation.GetStorageLocation().get()[
                                __ByteOffset + (Attributes.RelativeOffset / 8)],
                                &LWordValue, sizeof(int64_t));
                            break;
        case DataTypeCategories::CHAR :    
                            char CharValue;
                            if (!DataTypeUtils::ValueToChar(Value, CharValue)){
                                __configuration->PCLogger->RaiseException(
                                    "Char conversion error !");
                            }
                            std::memcpy(
                            &this->__MemoryLocation.GetStorageLocation().get()[
                                __ByteOffset + (Attributes.RelativeOffset / 8)],
                                &CharValue, sizeof(char));
                            break;
        case DataTypeCategories::INT :      
                            int16_t IntValue;
                            if (!DataTypeUtils::ValueToInt(Value, IntValue)){
                                __configuration->PCLogger->RaiseException(
                                    "Int conversion error !");
                            }
                            std::memcpy(
                            &this->__MemoryLocation.GetStorageLocation().get()[
                                __ByteOffset + (Attributes.RelativeOffset / 8)],
                                &IntValue, sizeof(int16_t));
                            break;
        case DataTypeCategories::SINT :     
                            int8_t SIntValue;
                            if (!DataTypeUtils::ValueToSint(Value, SIntValue)){
                                __configuration->PCLogger->RaiseException(
                                    "SInt conversion error !");
                            }
                            std::memcpy(
                            &this->__MemoryLocation.GetStorageLocation().get()[
                                __ByteOffset + (Attributes.RelativeOffset / 8)],
                                &SIntValue, sizeof(int8_t));
                            break;
        case DataTypeCategories::DINT :     
                            int32_t DIntValue;
                            if (!DataTypeUtils::ValueToDint(Value, DIntValue)){
                                __configuration->PCLogger->RaiseException(
                                    "DInt conversion error !");
                            }
                            std::memcpy(
                            &this->__MemoryLocation.GetStorageLocation().get()[
                                __ByteOffset + (Attributes.RelativeOffset / 8)],
                                &DIntValue, sizeof(int32_t));
                            break;
        case DataTypeCategories::LINT :     
                            int64_t LIntValue;
                            if (!DataTypeUtils::ValueToLint(Value, LIntValue)){
                                __configuration->PCLogger->RaiseException(
                                    "LInt conversion error !");
                            }
                            std::memcpy(
                            &this->__MemoryLocation.GetStorageLocation().get()[
                                __ByteOffset + (Attributes.RelativeOffset / 8)],
                                &LIntValue, sizeof(int64_t));
                            break;
        case DataTypeCategories::UINT :     
                            uint16_t UIntValue;
                            if (!DataTypeUtils::ValueToUint(Value, UIntValue)){
                                __configuration->PCLogger->RaiseException(
                                    "UInt conversion error !");
                            }
                            std::memcpy(
                            &this->__MemoryLocation.GetStorageLocation().get()[
                                __ByteOffset + (Attributes.RelativeOffset / 8)],
                                &UIntValue, sizeof(uint16_t));
                            break;
        case DataTypeCategories::USINT :     
                            uint8_t USIntValue;
                            if (!DataTypeUtils::ValueToUsint(Value, USIntValue)){
                                __configuration->PCLogger->RaiseException(
                                    "USInt conversion error !");
                            }
                            std::memcpy(
                            &this->__MemoryLocation.GetStorageLocation().get()[
                                __ByteOffset + (Attributes.RelativeOffset / 8)],
                                &USIntValue, sizeof(uint8_t));
                            break;
        case DataTypeCategories::UDINT :     
                            uint32_t UDIntValue;
                            if (!DataTypeUtils::ValueToUdint(Value, UDIntValue)){
                                __configuration->PCLogger->RaiseException(
                                    "UDInt conversion error !");
                            }
                            std::memcpy(
                            &this->__MemoryLocation.GetStorageLocation().get()[
                                __ByteOffset + (Attributes.RelativeOffset / 8)],
                                &UDIntValue, sizeof(uint32_t));
                            break;
        case DataTypeCategories::ULINT :     
                            uint64_t ULIntValue;
                            if (!DataTypeUtils::ValueToUlint(Value, ULIntValue)){
                                __configuration->PCLogger->RaiseException(
                                    "ULInt conversion error !");
                            }
                            std::memcpy(
                            &this->__MemoryLocation.GetStorageLocation().get()[
                                __ByteOffset + (Attributes.RelativeOffset / 8)],
                                &ULIntValue, sizeof(uint64_t));
                            break;
        case DataTypeCategories::REAL :     
                            float RealValue;
                            if (!DataTypeUtils::ValueToReal(Value, RealValue)){
                                __configuration->PCLogger->RaiseException(
                                    "Real conversion error !");
                            }
                            std::memcpy(
                            &this->__MemoryLocation.GetStorageLocation().get()[
                                __ByteOffset + (Attributes.RelativeOffset / 8)],
                                &RealValue, sizeof(float));
                            break;
        case DataTypeCategories::LREAL :     
                            double LRealValue;
                            if (!DataTypeUtils::ValueToLReal(Value, LRealValue)){
                                __configuration->PCLogger->RaiseException(
                                    "LReal conversion error !");
                            }
                            std::memcpy(
                            &this->__MemoryLocation.GetStorageLocation().get()[
                                __ByteOffset + (Attributes.RelativeOffset / 8)],
                                &LRealValue, sizeof(double));
                            break;
        case DataTypeCategories::TIME :     
                            TimeType Time;
                            if (!DataTypeUtils::ValueToTime(Value, Time)){
                                __configuration->PCLogger->RaiseException(
                                    "Time conversion error !");
                            }
                            std::memcpy(
                            &this->__MemoryLocation.GetStorageLocation().get()[
                                __ByteOffset + (Attributes.RelativeOffset / 8)],
                                &Time, sizeof(TimeType));
                            break;
        case DataTypeCategories::DATE :     
                            DateType Date;
                            if (!DataTypeUtils::ValueToDate(Value, Date)){
                                __configuration->PCLogger->RaiseException(
                                    "Date conversion error !");
                            }
                            std::memcpy(
                            &this->__MemoryLocation.GetStorageLocation().get()[
                                __ByteOffset + (Attributes.RelativeOffset / 8)],
                                &Date, sizeof(DateType));
                            break;
        case DataTypeCategories::TIME_OF_DAY :     
                            TODType TOD;
                            if (!DataTypeUtils::ValueToTOD(Value, TOD)){
                                __configuration->PCLogger->RaiseException(
                                    "TOD conversion error !");
                            }
                            std::memcpy(
                            &this->__MemoryLocation.GetStorageLocation().get()[
                                __ByteOffset + (Attributes.RelativeOffset / 8)],
                                &TOD, sizeof(TOD));
                            break;
        case DataTypeCategories::DATE_AND_TIME :     
                            DateTODDataType Dt;
                            if (!DataTypeUtils::ValueToDT(Value, Dt)){
                                __configuration->PCLogger->RaiseException(
                                    "Dt conversion error !");
                            }
                            std::memcpy(
                            &this->__MemoryLocation.GetStorageLocation().get()[
                                __ByteOffset + (Attributes.RelativeOffset / 8)],
                                &Dt, sizeof(DateTODDataType));
                            break;
        default :           __configuration->PCLogger->RaiseException(
                                "Only fields pointing to elementary data types"
                                " can be set with passed string values !");
    }
}

void PCVariable::SetPCVariableField(string NestedFieldName, void * Value,
                                    int CopySizeBytes) {

    
    if (!NestedFieldName.empty()) {
        DataTypeFieldAttributes Attributes;
        __VariableDataType->GetFieldAttributes(NestedFieldName, Attributes);

        if (Attributes.FieldInterfaceType == FIELD_INTERFACE_TYPES::VAR_IN_OUT
            || Attributes.FieldInterfaceType == FIELD_INTERFACE_TYPES::VAR_EXTERNAL
            || Attributes.FieldInterfaceType == FIELD_INTERFACE_TYPES::VAR_ACCESS) {
                // it is a pointer, we must get the pointed variable and set it there
                auto PointedVariable  =  GetPCVariableToField(
                        NestedFieldName);       
                PointedVariable.SetPCVariableField("", Value,
                                sizeof(PCVariable *));
                return;
        }

        assert(this->__MemoryLocation.IsInitialized());

        std::memcpy(
            &this->__MemoryLocation.GetStorageLocation().get()[
                __ByteOffset + (Attributes.RelativeOffset / 8)],
                Value, CopySizeBytes);
        return;

    }

    std::memcpy(&this->__MemoryLocation.GetStorageLocation().get()[__ByteOffset],
                Value, CopySizeBytes);
    return;
}

void PCVariable::GetAndStoreValue(string NestedFieldName,
    void * Value, int CopySize, int DataTypeCategory) {
    assert (Value != nullptr);
    if (!NestedFieldName.empty()) {
        auto PointedVariable =  GetPCVariableToField(NestedFieldName);
        assert(PointedVariable.__VariableDataType->__DataTypeCategory
            == DataTypeCategory);
    
        std::memcpy(&Value, 
                    &PointedVariable.__MemoryLocation
                        .GetStorageLocation()
                        .get()[PointedVariable.__ByteOffset],                               
                    CopySize);    
    } else {
        assert(this->__VariableDataType->__DataTypeCategory
            == DataTypeCategory);
    
        std::memcpy(&Value, 
                    &this->__MemoryLocation
                        .GetStorageLocation()
                        .get()[this->__ByteOffset],                               
                    CopySize); 
    }
}

template <typename T> T PCVariable::GetFieldValue(string NestedFieldName,
                                            int DataTypeCategory) {
    T Value;
    int CopySize = sizeof(T);
    GetAndStoreValue(NestedFieldName, &Value, CopySize, DataTypeCategory);
    return Value;
}


template <typename T> bool PCVariable::OperateOnVariables(T var1, T var2,
                                        int DataTypeCategory, int VarOp) {

    if (DataTypeCategory == DataTypeCategories::NOT_ASSIGNED ||
        DataTypeCategory == DataTypeCategories::DERIVED ||
        DataTypeCategory == DataTypeCategories::POU ||
        DataTypeCategory == DataTypeCategories::ARRAY)
            __configuration->PCLogger->RaiseException("Cannot perform operation"
                                    "on complex data type variables")
    switch(VarOp) {
        case VariableOps::ADD :     
            if (DataTypeCategory == DataTypeCategories::DATE ||
                DataTypeCategory == DataTypeCategories::DATE_AND_TIME)
                __configuration->PCLogger->RaiseException(
                    "Cannot perform + operation on date and date_time vars")
            break;
        
        case VariableOps::SUB :     
            if (DataTypeCategory == DataTypeCategories::DATE ||
                DataTypeCategory == DataTypeCategories::DATE_AND_TIME)
                __configuration->PCLogger->RaiseException(
                    "Cannot perform - operation on date and date_time vars")
            break;
        
        case VariableOps::MUL : 
        case VariableOps::DIV : 
        case VariableOps::MOD :
        case VariableOps::AND :
        case VariableOps::OR  :
        case VariableOps::XOR :
        case VariableOps::LS  :
        case VariableOps::RS  : 

            if (DataTypeCategory == DataTypeCategories::DATE ||
                DataTypeCategory == DataTypeCategories::DATE_AND_TIME ||
                DataTypeCategory == DataTypeCategories::TIME_OF_DAY ||
                DataTypeCategory == DataTypeCategories::TIME)
                __configuration->PCLogger->RaiseException(
                    "Cannot perform any of *,/,%%,&,|,^,<<,>> operation on "
                    "date, date_time, tod and time vars")
            break;           
            
    }


    switch(VarOp) {
        case VariableOps::ADD :     
            auto resa = var1 + var2;
            this->SetPCVariableField("", &resa, sizeof(resa));
            return true;
        
        case VariableOps::SUB :     
            auto ress = var1 - var2;
            this->SetPCVariableField("", &ress, sizeof(ress));
            return true;
        
        case VariableOps::MUL : 
            auto resm = var1 * var2;
            this->SetPCVariableField("", &resm, sizeof(resm));
            return true;
        case VariableOps::DIV : 
            auto resd = var1/var2;
            this->SetPCVariableField("", &resd, sizeof(resd));
            return true;
        case VariableOps::MOD :
            auto resmod = var1 % var2;
            this->SetPCVariableField("", &resmod, sizeof(resmod));
            return true;
        case VariableOps::AND :
            auto resand = var1 & var2;
            this->SetPCVariableField("", &resand, sizeof(resand));
            return true;

        case VariableOps::OR  :
            auto resor = var1 | var2;
            this->SetPCVariableField("", &resor, sizeof(resor));
            return true;
        case VariableOps::XOR :
            auto resxor = var1 ^ var2;
            this->SetPCVariableField("", &resxor, sizeof(resxor));
            return true;
        case VariableOps::LS  :
            auto resls = var1 << var2;
            this->SetPCVariableField("", &resls, sizeof(resls));
            return true;
        case VariableOps::RS  :
            auto resrs = var1 >> var2;
            this->SetPCVariableField("", &resrs, sizeof(resrs));
            return true;

        case VariableOps::EQ  :
            auto reseq = (var1 == var2);
            this->SetPCVariableField("", &reseq, sizeof(reseq));
            return reseq;
        case VariableOps::GT  :
            auto resgt = (var1 > var2);
            this->SetPCVariableField("", &resgt, sizeof(resgt));
            return resgt;      
        case VariableOps::GE  :
            auto resge = (var1 >= var2);
            this->SetPCVariableField("", &resge, sizeof(resge));
            return resge;
        case VariableOps::LT  :
            auto reslt = (var1 < var2);
            this->SetPCVariableField("", &reslt, sizeof(reslt));
            return reslt;
        case VariableOps::LE  :
            auto resle = (var1 <= var2);
            this->SetPCVariableField("", &resle, sizeof(resle));
            return resle;
        default :   __configuration->PCLogger->RaiseException("Unsupported "
                                    "variable operation type !");

    }


    return false;
}

void PCVariable::operator=(const PCVariable& V) {
    this->__ByteOffset = V.__ByteOffset;
    this->__BitOffset = V.__BitOffset;
    this->__VariableName = V.__VariableName;
    this->__VariableDataType = V.__VariableDataType;
    this->__MemoryLocation = V.__MemoryLocation;
    this->__configuration = V.__configuration;
}



bool PCVariable::InitiateOperationOnVariables(PCVariable& V, int VarOp) {

    assert(this->__VariableDataType->__DataTypeCategory
        == V.__VariableDataType->__DataTypeCategory);
    int DataTypeCategory = V.__VariableDataType->__DataTypeCategory;


    switch(DataTypeCategory) {
        case DataTypeCategories::BOOL :
            return OperateOnVariables<bool>(
                    this->GetFieldValue<bool>("", DataTypeCategory),
                    V.GetFieldValue<bool>("", DataTypeCategory),
                    DataTypeCategory, VarOp);

        case DataTypeCategories::BYTE :
        case DataTypeCategories::SINT :     
            return OperateOnVariables<int8_t>(
                    this->GetFieldValue<int8_t>("", DataTypeCategory),
                    V.GetFieldValue<int8_t>("", DataTypeCategory),
                    DataTypeCategory, VarOp);
        
        case DataTypeCategories::WORD :   
        case DataTypeCategories::INT :     
            return OperateOnVariables<int16_t>(
                    this->GetFieldValue<int16_t>("", DataTypeCategory),
                    V.GetFieldValue<int16_t>("", DataTypeCategory),
                    DataTypeCategory, VarOp);

        case DataTypeCategories::DWORD :   
        case DataTypeCategories::DINT :     
            return OperateOnVariables<int32_t>(
                    this->GetFieldValue<int32_t>("", DataTypeCategory),
                    V.GetFieldValue<int32_t>("", DataTypeCategory),
                    DataTypeCategory, VarOp);

        case DataTypeCategories::LWORD :   
        case DataTypeCategories::LINT :     
            return OperateOnVariables<int64_t>(
                    this->GetFieldValue<int64_t>("", DataTypeCategory),
                    V.GetFieldValue<int64_t>("", DataTypeCategory),
                    DataTypeCategory, VarOp);

          
        case DataTypeCategories::USINT :     
            return OperateOnVariables<uint8_t>(
                    this->GetFieldValue<uint8_t>("", DataTypeCategory),
                    V.GetFieldValue<uint8_t>("", DataTypeCategory),
                    DataTypeCategory, VarOp);

        case DataTypeCategories::UINT :     
            return OperateOnVariables<uint16_t>(
                    this->GetFieldValue<uint16_t>("", DataTypeCategory),
                    V.GetFieldValue<uint16_t>("", DataTypeCategory),
                    DataTypeCategory, VarOp);
        
        case DataTypeCategories::UDINT :     
            return OperateOnVariables<uint32_t>(
                    this->GetFieldValue<uint32_t>("", DataTypeCategory),
                    V.GetFieldValue<uint32_t>("", DataTypeCategory),
                    DataTypeCategory, VarOp);
        
        case DataTypeCategories::ULINT :     
            return OperateOnVariables<uint64_t>(
                    this->GetFieldValue<uint64_t>("", DataTypeCategory),
                    V.GetFieldValue<uint64_t>("", DataTypeCategory),
                    DataTypeCategory, VarOp);

        case DataTypeCategories::CHAR :     
            return OperateOnVariables<char>(
                    this->GetFieldValue<char>("", DataTypeCategory),
                    V.GetFieldValue<char>("", DataTypeCategory),
                    DataTypeCategory, VarOp);

        case DataTypeCategories::REAL :     
            return OperateOnVariables<float>(
                    this->GetFieldValue<float>("", DataTypeCategory),
                    V.GetFieldValue<float>("", DataTypeCategory),
                    DataTypeCategory, VarOp);

        case DataTypeCategories::LREAL :     
            return OperateOnVariables<double>(
                    this->GetFieldValue<double>("", DataTypeCategory),
                    V.GetFieldValue<double>("", DataTypeCategory),
                    DataTypeCategory, VarOp);
        
        case DataTypeCategories::TIME :     
            return OperateOnVariables<TimeType>(
                    this->GetFieldValue<TimeType>("", DataTypeCategory),
                    V.GetFieldValue<TimeType>("", DataTypeCategory),
                    DataTypeCategory, VarOp);
        
        case DataTypeCategories::TIME_OF_DAY: 
            return OperateOnVariables<TODDataType>(
                    this->GetFieldValue<TODDataType>("", DataTypeCategory),
                    V.GetFieldValue<TODDataType>("", DataTypeCategory),
                    DataTypeCategory, VarOp);
        
        case DataTypeCategories::DATE: 
            return OperateOnVariables<DateType>(
                    this->GetFieldValue<DateType>("", DataTypeCategory),
                    V.GetFieldValue<DateType>("", DataTypeCategory),
                    DataTypeCategory, VarOp);

        case DataTypeCategories::DATE_AND_TIME: 
            return OperateOnVariables<DateTODType>(
                    this->GetFieldValue<DateTODType>("", DataTypeCategory),
                    V.GetFieldValue<DateTODType>("", DataTypeCategory),
                    DataTypeCategory, VarOp);

        default :   __configuration->PCLogger->RaiseException(" Operation "
                        " is not supported for variables of DataType "
                        + std::to_string(DataTypeCategory));
        
    }
}

PCVariable& PCVariable::operator+(PCVariable& V ) {
    InitiateOperationOnVariables(V, VariableOps::ADD);
    return *this;
}

PCVariable& PCVariable::operator-(PCVariable& V ) {
    InitiateOperationOnVariables(V, VariableOps::SUB);
    return *this;
}

PCVariable& PCVariable::operator*(PCVariable& V ) {
    InitiateOperationOnVariables(V, VariableOps::MUL);
    return *this;
}

PCVariable& PCVariable::operator/(PCVariable& V ) {
    InitiateOperationOnVariables(V, VariableOps::DIV);
    return *this;
}

PCVariable& PCVariable::operator%(PCVariable& V ) {
    InitiateOperationOnVariables(V, VariableOps::MOD);
    return *this;
}

PCVariable& PCVariable::operator&(PCVariable& V ) {
    InitiateOperationOnVariables(V, VariableOps::AND);
    return *this;
}

PCVariable& PCVariable::operator|(PCVariable& V ) {
    InitiateOperationOnVariables(V, VariableOps::OR);
    return *this;
}

PCVariable& PCVariable::operator^(PCVariable& V ) {
    InitiateOperationOnVariables(V, VariableOps::XOR);
    return *this;
}

PCVariable& PCVariable::operator<<(PCVariable& V ) {
    InitiateOperationOnVariables(V, VariableOps::LS);
    return *this;
}

PCVariable& PCVariable::operator>>(PCVariable& V ) {
    InitiateOperationOnVariables(V, VariableOps::RS);
    return *this;
}

bool PCVariable::operator==(PCVariable& V ) {
    return InitiateOperationOnVariables(V, VariableOps::EQ);
}

bool PCVariable::operator>(PCVariable& V ) {
    return InitiateOperationOnVariables(V, VariableOps::GT);
}

bool PCVariable::operator>=(PCVariable& V ) {
    return InitiateOperationOnVariables(V, VariableOps::GE);
}

bool PCVariable::operator<(PCVariable& V ) {
    return InitiateOperationOnVariables(V, VariableOps::LT);
}

bool PCVariable::operator<=(PCVariable& V ) {
    return InitiateOperationOnVariables(V, VariableOps::LE);
}