#include <iostream>
#include <cstdint>
#include <vector>
#include <cstring>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "src/pc_emulator/include/pc_variable.h"
#include "src/pc_emulator/include/pc_datatype.h"
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_resource.h"
#include "src/pc_emulator/include/utils.h"
using namespace std;
using namespace pc_emulator;
using namespace pc_specification;
using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldIntfType = pc_specification::FieldInterfaceType;


PCVariable::PCVariable(PCConfiguration * configuration,
                PCResource * AssociatedResource,
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

    __AssociatedResource = AssociatedResource;
    __ByteOffset = 0;
    __BitOffset = 0;
    __VariableName = VariableName;
    //__MemoryLocation = PCMemUnit();
    __MemAllocated = false;
    __IsDirectlyRepresented = false;
    __IsVariableContentTypeAPtr = false;
}

void PCVariable::Cleanup() {

    // delete all accessed fields 
}

void PCVariable::CheckValidity() {
    if (!__IsDirectlyRepresented)
        assert(__BitOffset == 0);
}

void PCVariable::AllocateStorage() {

    if (! __MemoryLocation.IsInitialized()) {
        int DataTypeSizeBytes = (__VariableDataType->__SizeInBits)/8;

        DataTypeSizeBytes = DataTypeSizeBytes > 0 ? DataTypeSizeBytes : 1;

        __MemoryLocation.AllocateStaticMemory(DataTypeSizeBytes);
    }
}

void PCVariable::AllocateAndInitialize() {

    if (!__MemAllocated) {
        this->AllocateStorage();
        this->InitializeAllNonPtrFields();
        this->InitializeAllDirectlyRepresentedFields();
    }
    __MemAllocated = true;
    CheckValidity();
}

void PCVariable::OnExecutorStartup() {
    for(auto& DefinedField: 
        __VariableDataType->__FieldsByInterfaceType[
            FieldIntfType::VAR_TEMP]) {
        
        PCVariable* FieldVariable 
            = GetPtrToField(DefinedField.__FieldName);
        InitializeVariable(FieldVariable, DefinedField.__InitialValue);
    }
}

void PCVariable::ParseRemFieldAttributes(std::vector<string>& NestedFields,
                        int StartPos, DataTypeFieldAttributes& FieldAttributes,
                        PCVariable * HolderVariable) {
    
    if (StartPos == (int)NestedFields.size())
        return;

    PCDataType * DataType = HolderVariable->__VariableDataType;
    FieldAttributes.FieldDataTypePtr = HolderVariable->__VariableDataType;
    FieldAttributes.HoldVariablePtr = HolderVariable;
    FieldAttributes.SizeInBits = HolderVariable->__VariableDataType->__SizeInBits;

    

    for (int i = StartPos; i < (int)NestedFields.size(); i++) {
        string AccessedFieldName = NestedFields[i];
        for (int IntfType = FieldIntfType::VAR_INPUT; 
                IntfType <= FieldIntfType::NA; IntfType ++) {
            for(auto& DefinedField: DataType->__FieldsByInterfaceType[IntfType]) {
                PCDataType * FieldDataType = DefinedField.__FieldTypePtr;
                assert(FieldDataType != nullptr);
                if(AccessedFieldName == DefinedField.__FieldName) {
                    FieldAttributes.FieldInterfaceType = IntfType;
                    FieldAttributes.FieldDataTypePtr = FieldDataType;

                    if (i == (int)NestedFields.size() - 1)
                        return; // note that the relative offset is already set

                    

                    int NxtNestedField = i + 1;
                    int RelativeOffsetIncrement = 0;
                    if(DefinedField.__FieldTypeCategory 
                                                == DataTypeCategory::ARRAY) {
                        int NDims = DefinedField.__NDimensions;
                        assert(NDims == 1 || NDims == 2);
                        int idx1, idx2;
                        if (NDims == 1) {
                            assert(i + 1 < NestedFields.size());
                            idx1 = std::stoi(NestedFields[i+1]);
                            idx2 = -1;
                            NxtNestedField = i + 2;
                            RelativeOffsetIncrement 
                                = (idx1 - 1)*FieldAttributes
                                        .FieldDataTypePtr->__SizeInBits;
                        } else {
                            assert(i + 2 < NestedFields.size());
                            idx1 = std::stoi(NestedFields[i+1]);
                            idx2 = std::stoi(NestedFields[i+2]);
                            NxtNestedField = i + 3; 
                            RelativeOffsetIncrement 
                                = ((idx1 - 1)*
                                    DefinedField.__Dimension2 + (idx2 - 1))*
                                    FieldAttributes.FieldDataTypePtr->__SizeInBits;
                        }
                    }

                    if (IntfType == FieldIntfType::VAR_IN_OUT || 
                        IntfType == FieldIntfType::VAR_EXTERNAL ||
                        IntfType == FieldIntfType::VAR_ACCESS ||
                        IntfType == FieldIntfType::VAR_EXPLICIT_STORAGE) {
                        PCVariable * nxtHolderVariable = nullptr;
                        
                        assert(FieldAttributes.RelativeOffset % 8 == 0);
                        std::memcpy(&nxtHolderVariable,
                            HolderVariable->__MemoryLocation.GetPointerToMemory(
                            HolderVariable->__ByteOffset + FieldAttributes.RelativeOffset/8),
                            sizeof(PCVariable *));

                        if (nxtHolderVariable == nullptr) {
                            __configuration->PCLogger->RaiseException(
                                "Null ptr dereference in: " 
                                    + FieldAttributes.NestedFieldName
                                    + " At Offset: "
                                    + std::to_string(FieldAttributes.RelativeOffset/8)
                                    + " Holder Variable: " + HolderVariable->__VariableName);
                        }
                        FieldAttributes.HoldVariablePtr = nxtHolderVariable;
                        // Relative Offset reset
                        FieldAttributes.RelativeOffset = RelativeOffsetIncrement;                         
                        ParseRemFieldAttributes(NestedFields, NxtNestedField,
                                    FieldAttributes, nxtHolderVariable);
                        return;
                    } 
                    FieldAttributes.RelativeOffset += RelativeOffsetIncrement;
                    ParseRemFieldAttributes(NestedFields, NxtNestedField,
                                    FieldAttributes, HolderVariable);
                    return;
                } else {

                    
                    if (IntfType == FieldIntfType::VAR_IN_OUT || 
                        IntfType == FieldIntfType::VAR_EXTERNAL ||
                        IntfType == FieldIntfType::VAR_ACCESS||
                        IntfType == FieldIntfType::VAR_EXPLICIT_STORAGE) {
                        // this is a pointer

                        assert(FieldAttributes.RelativeOffset %8 == 0);

                        FieldAttributes.RelativeOffset 
                            += sizeof (PCDataType *)*8;
                    } else {

                        assert(FieldAttributes.RelativeOffset %8 == 0);
                        // Need to check for array here
                        if (DefinedField.__FieldTypeCategory
                            == DataTypeCategory::ARRAY) {
                            int NArrayElements = DefinedField.__Dimension1;
                            if (DefinedField.__NDimensions == 2)
                                NArrayElements *= DefinedField.__Dimension2;   
                            FieldAttributes.RelativeOffset
                                += (NArrayElements*
                                    DefinedField.__FieldTypePtr->__SizeInBits);
                        } else {
                            
                            FieldAttributes.RelativeOffset 
                                    += FieldDataType->__SizeInBits;
                        }

                        if (FieldAttributes.RelativeOffset %8 != 0) {
                            FieldAttributes.RelativeOffset
                                += (8 - FieldAttributes.RelativeOffset %8);
                        }
                    }
                }
            }
        }
    }
    __configuration->PCLogger->RaiseException("Nested Field not found !");
}


/*
 * Given a nested field name, it returns attributes of that field. The field
 * can potentially be a pointer as well
 * The returned FieldAttributes are:
 *   HoldVariablePtr:    Pointer to PCVariable which owns the memory location
 *                       containing the content of the field i.e the value of
 *                       the field is stored in a memory location owned by this
 *                       variable. If the field is a pointer, then the value of
 *                       the field would be the address its pointing to.
 *   NestedFieldName:    Just a reference to the full field name. (Copied from
 *                       argument)
 *   FieldDataTypePtr:   Points to the DataType of the field regardless of 
 *                       whether the field is a pointer or not
 *   SizeInBits:         SizeInBits of the HolderVariable
 *   FieldInterfaceType: InterfaceType of the field. If the field is a pointer
 *                       it will be in {VAR_IN_OUT, VAR_EXTERNAL, VAR_ACCESS
 *                       VAR_EXPLICIT_STORAGE}
 *   RelativeOffset:     Offset in bits which needs to be added to 
 *                       HolderVariablePtr->__ByteOffset to get interested 
 *                       memory location containing the value of the
 *                       NestedFieldName
*/
void PCVariable::GetFieldAttributes(string NestedFieldName,
                                    DataTypeFieldAttributes& FieldAttributes) {
    std::vector<std::string> NestedFields;

    boost::trim_if(NestedFieldName, boost::is_any_of("\t .[]"));
    boost::split(NestedFields, NestedFieldName,
                boost::is_any_of(".[]"), boost::token_compress_on);
    if (NestedFieldName == "") {
        
        FieldAttributes.RelativeOffset = 0;
        FieldAttributes.FieldInterfaceType = FieldIntfType::NA;
        FieldAttributes.SizeInBits = this->__VariableDataType->__SizeInBits;
        FieldAttributes.FieldDataTypePtr = this->__VariableDataType;
        FieldAttributes.NestedFieldName = "";
        FieldAttributes.HoldVariablePtr = this;
        return;
            
    }

    FieldAttributes.FieldDataTypePtr = this->__VariableDataType;
    FieldAttributes.HoldVariablePtr = this;
    FieldAttributes.FieldInterfaceType = FieldIntfType::NA;
    FieldAttributes.NestedFieldName = NestedFieldName;
    FieldAttributes.RelativeOffset = 0;
    FieldAttributes.SizeInBits = this->__VariableDataType->__SizeInBits;
    ParseRemFieldAttributes(NestedFields, 0, FieldAttributes, this);
}

/*
 *  Given a NestedFieldName, It creates a PCVariable pointer whose
 *  Byte and Bit Offsets are set accordingly to point to the memory location
 *  containing the value of NestedFieldName. If the NestedFieldName is a pointer
 *  then this value would be the address pointed to by the pointer   
*/
PCVariable* PCVariable::GetPtrToField(string NestedFieldName) {
    DataTypeFieldAttributes Attributes;
    
    CheckValidity();
    assert(__VariableDataType->IsFieldPresent(NestedFieldName) == true);

    if (NestedFieldName.empty())
        return this;

    auto got = __AccessedFields.find (NestedFieldName);
    if (got != __AccessedFields.end()) {
        return got->second.get();
    }

    GetFieldAttributes(NestedFieldName, Attributes);


    PCVariable * HolderVariable = Attributes.HoldVariablePtr;
    assert(HolderVariable != nullptr);

    __AccessedFields.insert(
                    std::make_pair(NestedFieldName,
                    std::unique_ptr<PCVariable>(
                        new PCVariable(__configuration,
                            __AssociatedResource,
                            __VariableName + NestedFieldName,
                            Attributes.FieldDataTypePtr->__DataTypeName))));


    auto VariablePtrToField = __AccessedFields.find(
        NestedFieldName)->second.get();
    
    VariablePtrToField->__ByteOffset 
                    = HolderVariable->__ByteOffset 
                    + Attributes.RelativeOffset / 8;
    VariablePtrToField->__BitOffset 
                    = HolderVariable->__BitOffset 
                    + (Attributes.RelativeOffset % 8);
    VariablePtrToField->__MemoryLocation.SetMemUnitLocation(
                        (PCMemUnit *)&HolderVariable->__MemoryLocation);
    VariablePtrToField->__MemAllocated = true;

    if (Attributes.FieldInterfaceType != FieldIntfType::VAR_IN_OUT
        && Attributes.FieldInterfaceType != FieldIntfType::VAR_EXTERNAL
        && Attributes.FieldInterfaceType != FieldIntfType::VAR_ACCESS
        && Attributes.FieldInterfaceType != FieldIntfType::VAR_EXPLICIT_STORAGE) {
        VariablePtrToField->__IsVariableContentTypeAPtr = false;
    } else {
        VariablePtrToField->__IsVariableContentTypeAPtr = true;
    }
    return VariablePtrToField;
}

void PCVariable::InitializeVariable(PCVariable * V, string InitialValue) {

    bool resbool;
    uint8_t resbyte;
    uint16_t resword;
    uint32_t resdword;
    uint64_t reslword;
    char reschar;
    int16_t resint;
    int8_t ressint;
    int32_t resdint;
    int64_t reslint;
    uint16_t resuint;
    uint8_t resusint;
    uint32_t resudint;
    uint64_t resulint;
    float resreal;
    double reslreal;
    TimeType restime;
    DateType resdate;
    DateTODType resdt;
    TODType restod;
    std::vector<std::string> InitialValues;

    assert(V->__VariableDataType != nullptr);

    switch(V->__VariableDataType->__DataTypeCategory) {
        case DataTypeCategory::BOOL : 
            assert(V->__VariableDataType->__NFields == 0);
            assert(DataTypeUtils::ValueToBool(InitialValue, resbool));
            V->SetField("", &resbool, sizeof(resbool));
            return;

        case DataTypeCategory::BYTE : 
            assert(V->__VariableDataType->__NFields == 0);
            assert(DataTypeUtils::ValueToByte(InitialValue, resbyte));
            V->SetField("", &resbyte, sizeof(resbyte));
            return; 

        case DataTypeCategory::WORD : 
            assert(V->__VariableDataType->__NFields == 0);          
            assert(DataTypeUtils::ValueToWord(InitialValue, resword));
            V->SetField("", &resword, sizeof(resword));
            return;     

        case DataTypeCategory::DWORD : 
            assert(V->__VariableDataType->__NFields == 0);    
            assert(DataTypeUtils::ValueToDWord(InitialValue, resdword));
            V->SetField("", &resdword, sizeof(resdword));
            return;     

        case DataTypeCategory::LWORD : 
            assert(V->__VariableDataType->__NFields == 0);
            
            assert(DataTypeUtils::ValueToLWord(InitialValue, reslword));
            V->SetField("", &reslword, sizeof(reslword));
            return;     

        case DataTypeCategory::CHAR : 
            assert(V->__VariableDataType->__NFields == 0);
            assert(DataTypeUtils::ValueToChar(InitialValue, reschar));
            V->SetField("", &reschar, sizeof(reschar));
            return; 
                
        case DataTypeCategory::INT : 
            assert(V->__VariableDataType->__NFields == 0);
            
            assert(DataTypeUtils::ValueToInt(InitialValue, resint));
            V->SetField("", &resint, sizeof(resint));
            return;

        case DataTypeCategory::SINT : 
            assert(V->__VariableDataType->__NFields == 0);
            
            assert(DataTypeUtils::ValueToSint(InitialValue, ressint));
            V->SetField("", &ressint, sizeof(ressint));
            return;     

        case DataTypeCategory::DINT : 
            assert(V->__VariableDataType->__NFields == 0);
            
            assert(DataTypeUtils::ValueToDint(InitialValue, resdint));
            V->SetField("", &resdint, sizeof(resdint));
            return;     

        case DataTypeCategory::LINT : 
            assert(V->__VariableDataType->__NFields == 0);
            
            assert(DataTypeUtils::ValueToLint(InitialValue, reslint));
            V->SetField("", &reslint, sizeof(reslint));
            return;     

        case DataTypeCategory::UINT : 
            assert(V->__VariableDataType->__NFields == 0);
            
            assert(DataTypeUtils::ValueToUint(InitialValue, resuint));
            V->SetField("", &resuint, sizeof(resuint));
            return;     

        case DataTypeCategory::USINT : 
            assert(V->__VariableDataType->__NFields == 0);
            
            assert(DataTypeUtils::ValueToUsint(InitialValue, resusint));
            V->SetField("", &resusint, sizeof(resusint));
            return;     

        case DataTypeCategory::UDINT : 
            assert(V->__VariableDataType->__NFields == 0);
            
            assert(DataTypeUtils::ValueToUdint(InitialValue, resudint));
            V->SetField("", &resudint, sizeof(resudint));
            return;

        case DataTypeCategory::ULINT : 
            assert(V->__VariableDataType->__NFields == 0);
            
            assert(DataTypeUtils::ValueToUlint(InitialValue, resulint));
            V->SetField("", &resulint, sizeof(resulint));
            return; 

        case DataTypeCategory::REAL : 
            assert(V->__VariableDataType->__NFields == 0);
            
            assert(DataTypeUtils::ValueToReal(InitialValue, resreal));
            V->SetField("", &resreal, sizeof(resreal));
            return;     

        case DataTypeCategory::LREAL : 
            assert(V->__VariableDataType->__NFields == 0);
            
            assert(DataTypeUtils::ValueToLReal(InitialValue, reslreal));
            V->SetField("", &reslreal, sizeof(reslreal));
            return;     

        case DataTypeCategory::TIME : 
            assert(V->__VariableDataType->__NFields == 0);
            
            assert(DataTypeUtils::ValueToTime(InitialValue, restime));
            V->SetField("", &restime, sizeof(restime));
            return;     

        case DataTypeCategory::DATE : 
            assert(V->__VariableDataType->__NFields == 0);
            
            assert(DataTypeUtils::ValueToDate(InitialValue, resdate));
            V->SetField("", &resdate, sizeof(resdate));
            return;     

        case DataTypeCategory::DATE_AND_TIME : 
            assert(V->__VariableDataType->__NFields == 0);
            
            assert(DataTypeUtils::ValueToDT(InitialValue, resdt));
            V->SetField("", &resdt, sizeof(resdt));
            return;     

        case DataTypeCategory::TIME_OF_DAY : 
            assert(V->__VariableDataType->__NFields == 0);
            
            assert(DataTypeUtils::ValueToTOD(InitialValue, restod));
            V->SetField("", &restod, sizeof(restod));
            return;  

        case DataTypeCategory::ARRAY:
            __configuration->PCLogger->RaiseException("Cannot directly "
                "initializae an ARRAY variable!");
            

        default :
            for (int IntfType = FieldIntfType::VAR_INPUT; 
                IntfType <= FieldIntfType::NA; IntfType ++) {
                
                if (IntfType == FieldIntfType::VAR_IN_OUT ||
                    IntfType == FieldIntfType::VAR_EXTERNAL ||
                    IntfType == FieldIntfType::VAR_ACCESS ||
                    IntfType == FieldIntfType::VAR_EXPLICIT_STORAGE)
                    continue;

                // all non ptr fields
                for(auto& DefinedField: 
                    V->__VariableDataType->__FieldsByInterfaceType[IntfType]) {


                    if (DefinedField.__FieldTypeCategory
                            == DataTypeCategory::ARRAY) {
                        

                        string Init, FieldName;
                        int tot_offset;
                        for(int i = 0; i < DefinedField.__Dimension1;
                                i++) {
                            if (DefinedField.__NDimensions == 2) {
                                for(int j = 0; 
                                    j < DefinedField.__Dimension2; j++) {
                                    FieldName = "[" + std::to_string(i)
                                        +"][" + std::to_string(j) + "]";
                                    tot_offset = i*DefinedField.__Dimension2
                                                    + j;
                                    PCVariable* FieldVariable 
                                    = V->GetPtrToField(
                                        DefinedField.__FieldName + FieldName);
                                    InitializeVariable(FieldVariable,
                                        Utils::GetInitialValueForArrayIdx(
                                            tot_offset,
                                            DefinedField.__InitialValue,
                                            DefinedField.__FieldTypePtr,
                                            __configuration));
                                }
                            } else {
                                FieldName = "[" + std::to_string(i) + "]";
                                tot_offset = i;
                                PCVariable* FieldVariable 
                                    = V->GetPtrToField(
                                        DefinedField.__FieldName + FieldName);
                                    InitializeVariable(FieldVariable,
                                        Utils::GetInitialValueForArrayIdx(
                                            tot_offset,
                                            DefinedField.__InitialValue,
                                            DefinedField.__FieldTypePtr,
                                            __configuration));
                            }
                        }   
                    } else {
                        PCVariable* FieldVariable 
                            = V->GetPtrToField(DefinedField.__FieldName);
                        
                        InitializeVariable(FieldVariable, 
                                            DefinedField.__InitialValue);
                    }
                    
                }
            }   
            return; 
    }
    return;

}
void PCVariable::InitializeAllNonPtrFields() {
    for (int IntfType = FieldIntfType::VAR_INPUT; 
            IntfType <= FieldIntfType::NA; IntfType ++) {
        
        if (IntfType != FieldIntfType::VAR_IN_OUT &&
            IntfType != FieldIntfType::VAR_EXTERNAL &&
            IntfType != FieldIntfType::VAR_ACCESS &&
            IntfType != FieldIntfType::VAR_EXPLICIT_STORAGE) {
            for(auto& DefinedField: 
                __VariableDataType->__FieldsByInterfaceType[IntfType]) {
                
                PCVariable* FieldVariable 
                    = GetPtrToField(DefinedField.__FieldName);

                // Note that no subfield of this can be of explicit storage type
                // because this field is not explicit storage type.
                InitializeVariable(FieldVariable, DefinedField.__InitialValue);
            }
        } 
    }
}

void PCVariable::InitializeAllDirectlyRepresentedFields() {
    if (__VariableDataType->__DataTypeCategory == DataTypeCategory::POU) {
        for(auto& DefinedField: 
                __VariableDataType->__FieldsByInterfaceType[
                    FieldIntfType::VAR_EXPLICIT_STORAGE]) {
            
            PCVariable* FieldVariable;  
            if (DefinedField.__StorageMemType == MemType::RAM_MEM) {        
                FieldVariable 
                    = __configuration->GetVariablePointerToMem(
                        DefinedField.__StorageMemType,
                        DefinedField.__StorageByteOffset,
                        DefinedField.__StorageBitOffset,
                        DefinedField.__FieldTypeName);
            }
            else {
                assert(__AssociatedResource != nullptr);
                FieldVariable 
                    = __AssociatedResource->GetVariablePointerToMem(
                        DefinedField.__StorageMemType,
                        DefinedField.__StorageByteOffset,
                        DefinedField.__StorageBitOffset,
                        DefinedField.__FieldTypeName);
            }

            //InitializeVariable(FieldVariable);
            SetField(DefinedField.__FieldName, FieldVariable);

            if (DefinedField.__FieldTypeCategory 
                != DataTypeCategory::DERIVED)
                FieldVariable->SetField("", 
                                DefinedField.__InitialValue);
            
        }
    }
}

void PCVariable::ResolveAllExternalFields() {
    if (__VariableDataType->__DataTypeCategory == DataTypeCategory::POU) {
        for(auto& DefinedField: 
                __VariableDataType->__FieldsByInterfaceType[
                    FieldIntfType::VAR_EXTERNAL]) {        
            PCVariable* FieldVariable;      

            assert(__VariableDataType->__PoUType 
                                != pc_specification::PoUType::FC);
            
            FieldVariable = __configuration->GetVariable(
                            DefinedField.__FieldName);
                
            if (__VariableDataType->__PoUType 
                                == pc_specification::PoUType::PROGRAM) {
                // A program's extern can only be global variables defined in configuration/resource
                // definition
                if (FieldVariable == nullptr) {
                    __configuration->PCLogger->RaiseException(
                        "Undefined extern reference to: " + DefinedField.__FieldName
                        + " in definition of " + __VariableName);
                }
            } else {
            
                if (FieldVariable == nullptr 
                    && __AssociatedResource != nullptr) {
                    FieldVariable 
                        = __AssociatedResource->GetPOUGlobalVariable(
                                        DefinedField.__FieldName);

                    if (FieldVariable == nullptr) {
                        __configuration->PCLogger->RaiseException(
                        "Undefined extern reference to: " + DefinedField.__FieldName
                        + " in definition of " + __VariableName);
                    }
                }
            }
            SetField(DefinedField.__FieldName, FieldVariable);
        }
    }
}



/*
 *  Given a field referred to by NestedFieldName, it returns a PCVariable 
 *  pointer stored at the memory location reserved for the field.
 *  This function should return the value set by the SetField function
 *  NestedFieldName:    Pointer stored at the memory location reserved for this
 *                      field is returned
 */
PCVariable * PCVariable::GetPtrStoredAtField(string NestedFieldName) {
    DataTypeFieldAttributes Attributes;

    if (NestedFieldName.empty())
        return nullptr;

    GetFieldAttributes(NestedFieldName, Attributes);

    // Checks if the field referred to by NestedFieldName is actually holding
    // a pointer
    if(Attributes.FieldInterfaceType == FieldIntfType::VAR_IN_OUT ||
        Attributes.FieldInterfaceType == FieldIntfType::VAR_EXPLICIT_STORAGE ||
        Attributes.FieldInterfaceType == FieldIntfType::VAR_ACCESS ||
        Attributes.FieldInterfaceType == FieldIntfType::VAR_EXTERNAL
        && Attributes.HoldVariablePtr == this) {
        PCVariable *StoredPointer;
        auto FieldVariable = GetPtrToField(NestedFieldName);
        std::memcpy(&StoredPointer, 
            FieldVariable->__MemoryLocation.GetPointerToMemory(
                FieldVariable->__ByteOffset),
            sizeof(PCVariable *));
        return StoredPointer;
    }

    return nullptr; // this field location does not store a pointer or the
                    // pointer is stored indirectly in a memory location pointed
                    // to by another pointer.

    

}

/*
 *  Given DataTypeFieldAttributes, this function may copy some content of the
 *  argument "From" or it may copy the argument "From" itself to the appropriate
 *  location derived from the FieldAttributes. It copies some content from
 *  "From" iff Attributes of a non pointer field are given. It copies "From" itself
 *  iff Attributes of a pointer field are given.
 */
void PCVariable::CopyToPCVariableFieldFromPointer(
        DataTypeFieldAttributes& Attributes, PCVariable * From) {

    // Regardless of whether the Attributes of a pointer/non-pointer field
    // are given, first check if both data types are the same
    assert(Attributes.FieldDataTypePtr->__DataTypeName
            == From->__VariableDataType->__DataTypeName);

    PCVariable * FieldVariable = 
                    GetPtrToField(Attributes.NestedFieldName);

    // Attributes of a non pointer field are given. We must copy some content
    // from the "From" variable. The amount of content copied must be equal
    // to the size of the data type.
    if (Attributes.FieldInterfaceType != FieldIntfType::VAR_IN_OUT
        && Attributes.FieldInterfaceType != FieldIntfType::VAR_EXTERNAL
        && Attributes.FieldInterfaceType != FieldIntfType::VAR_ACCESS
        && Attributes.FieldInterfaceType != FieldIntfType::VAR_EXPLICIT_STORAGE) {
            
            // Slightly trick if the datatype is BOOL. We must also look at the
            // BitOffset in the interested memory location
            if (From->__VariableDataType->__DataTypeCategory 
                                            == DataTypeCategory::BOOL) {

                int8_t temp = From->__MemoryLocation.GetStorageLocation()
                                                    .get()[From->__ByteOffset];
                if (temp & ((1UL) <<  From->__BitOffset)) {
                    FieldVariable->__MemoryLocation.GetStorageLocation().get()[
                        FieldVariable->__ByteOffset] 
                                |= ((1UL) <<  FieldVariable->__BitOffset);
                }
                else {
                    FieldVariable->__MemoryLocation.GetStorageLocation().get()[
                        FieldVariable->__ByteOffset] 
                                &= ~((1UL) <<  FieldVariable->__BitOffset);
                }
                return;
            }
           // The content pointed to by From is copied from the appropriate
           // offset to the appropriate offset
           FieldVariable->__MemoryLocation.CopyFromMemUnit(
                &From->__MemoryLocation, 
                From->__ByteOffset,
                FieldVariable->__VariableDataType->__SizeInBits/8,
                FieldVariable->__ByteOffset);


    } else {
        // The pointer From is itself to be copied at the appropriate offset
        assert((int)(FieldVariable->__ByteOffset  + sizeof(PCVariable *))
            <= (int)FieldVariable->__MemoryLocation.GetMemUnitSize());
        std::memcpy(
            FieldVariable->__MemoryLocation.GetPointerToMemory(
                FieldVariable->__ByteOffset), &From, sizeof(PCVariable *));
        
    }
}

/*
 *  Wrapper for the previous function. Here the Attributes are not directly
 *  specified but the NestedFieldName is provided as a string. It first obtains
 *  the Attributes of the Field before calling the previous function
 */
void PCVariable::SetField(string NestedFieldName, PCVariable * From) {
    
    
    if (NestedFieldName.empty()) {
        //  Copy whole content of From except variable name.
        assert(this->__VariableDataType->__DataTypeName
            == From->__VariableDataType->__DataTypeName);


        if (From->__VariableDataType->__DataTypeCategory 
                                        == DataTypeCategory::BOOL) {

            int8_t temp = From->__MemoryLocation.GetStorageLocation().get()
                                                        [From->__ByteOffset];
            if (temp & ((1UL) <<  From->__BitOffset)) {
                this->__MemoryLocation.GetStorageLocation().get()[
                    __ByteOffset] |= ((1UL) <<  __BitOffset);
            }
            else {
                this->__MemoryLocation.GetStorageLocation().get()[
                    __ByteOffset] &= ~((1UL) <<  __BitOffset);
            }
            return;
        }
        this->__MemoryLocation.CopyFromMemUnit(
                &From->__MemoryLocation, 
                From->__ByteOffset,
                __VariableDataType->__SizeInBits/8,
                __ByteOffset);
        return;

    }

    // Get the field attributes
    DataTypeFieldAttributes Attributes;
    GetFieldAttributes(NestedFieldName, Attributes);

    // This will behave exactly like SetField if the FieldAttributes indicate
    // that it is a pointer
    CopyToPCVariableFieldFromPointer(Attributes, From);
}



void PCVariable::SetField(string NestedFieldName, string Value) {

    DataTypeFieldAttributes Attributes;
    bool BoolValue;
    int bit_off;
    uint8_t ByteValue;
    uint16_t WordValue;
    uint32_t DWordValue;
    uint64_t LWordValue;
    char CharValue;
    int16_t IntValue;
    int8_t SIntValue;
    int32_t DIntValue;
    int64_t LIntValue;
    uint16_t UIntValue;
    uint8_t USIntValue;
    uint32_t UDIntValue;
    uint64_t ULIntValue;
    float RealValue;
    double LRealValue;
    TimeType Time;
    DateType Date;
    TODType TOD;
    DateTODDataType Dt;

    assert(__VariableDataType->IsFieldPresent(NestedFieldName) == true);
    GetFieldAttributes(NestedFieldName, Attributes);
    CheckValidity();


    if (Attributes.FieldInterfaceType == FieldIntfType::VAR_IN_OUT
        || Attributes.FieldInterfaceType == FieldIntfType::VAR_EXTERNAL
        || Attributes.FieldInterfaceType == FieldIntfType::VAR_ACCESS
        || Attributes.FieldInterfaceType == FieldIntfType::VAR_EXPLICIT_STORAGE) {

       // a string value cannot be set at a field location which is a pointer,
       // use SetField instead. Raise Excecption.
       assert(false);
       return; 
    }

    assert(this->__MemoryLocation.IsInitialized());
    PCVariable *FieldVariable = 
                GetPtrToField(NestedFieldName);

    switch(Attributes.FieldDataTypePtr->__DataTypeCategory) {
        case DataTypeCategory::BOOL :     
            bit_off = FieldVariable->__BitOffset;
            if (!DataTypeUtils::ValueToBool(Value, BoolValue)){
                __configuration->PCLogger->RaiseException(
                    "Bool conversion error !");
            }
            
            if (BoolValue) // set bit at bit_offset
                FieldVariable->__MemoryLocation.GetStorageLocation().get()[
                    FieldVariable->__ByteOffset] |= 
                                ((1UL) <<  bit_off);
            else // clear bit at bit offset
                FieldVariable->__MemoryLocation.GetStorageLocation().get()[
                    FieldVariable->__ByteOffset] &= 
                                ~((1UL) <<  bit_off);
            break;

        case DataTypeCategory::BYTE :     
            
            if (!DataTypeUtils::ValueToByte(Value, ByteValue)){
                __configuration->PCLogger->RaiseException(
                    "Byte conversion error !");
            }
            std::memcpy(
                FieldVariable->__MemoryLocation
                    .GetPointerToMemory(FieldVariable->__ByteOffset),
                &ByteValue, sizeof(uint8_t));
            break;
        case DataTypeCategory::WORD :     
            
            if (!DataTypeUtils::ValueToWord(Value, WordValue)){
                __configuration->PCLogger->RaiseException(
                    "Word conversion error !");
            }
            std::memcpy(
                FieldVariable->__MemoryLocation
                    .GetPointerToMemory(FieldVariable->__ByteOffset),
                &WordValue, sizeof(uint16_t));
            break;
        case DataTypeCategory::DWORD :     
            
            if (!DataTypeUtils::ValueToDWord(Value, DWordValue)){
                __configuration->PCLogger->RaiseException(
                    "DWord conversion error !");
            }
            std::memcpy(
                FieldVariable->__MemoryLocation
                    .GetPointerToMemory(FieldVariable->__ByteOffset),
                &DWordValue, sizeof(uint32_t));
            break;
        case DataTypeCategory::LWORD :    
            
            if (!DataTypeUtils::ValueToLWord(Value, LWordValue)){
                __configuration->PCLogger->RaiseException(
                    "LWord conversion error !");
            }
            std::memcpy(
                FieldVariable->__MemoryLocation
                    .GetPointerToMemory(FieldVariable->__ByteOffset),
                &LWordValue, sizeof(uint64_t));
            break;
        case DataTypeCategory::CHAR :    
            
            if (!DataTypeUtils::ValueToChar(Value, CharValue)){
                __configuration->PCLogger->RaiseException(
                    "Char conversion error !");
            }
            std::memcpy(
                FieldVariable->__MemoryLocation
                    .GetPointerToMemory(FieldVariable->__ByteOffset),
                &CharValue, sizeof(char));
            break;
        case DataTypeCategory::INT :      
            
            if (!DataTypeUtils::ValueToInt(Value, IntValue)){
                __configuration->PCLogger->RaiseException(
                    "Int conversion error !");
            }
            std::memcpy(
                FieldVariable->__MemoryLocation
                    .GetPointerToMemory(FieldVariable->__ByteOffset),
                &IntValue, sizeof(int16_t));
            break;
        case DataTypeCategory::SINT :     
            
            if (!DataTypeUtils::ValueToSint(Value, SIntValue)){
                __configuration->PCLogger->RaiseException(
                    "SInt conversion error !");
            }
            std::memcpy(
                FieldVariable->__MemoryLocation
                    .GetPointerToMemory(FieldVariable->__ByteOffset),
                &SIntValue, sizeof(int8_t));
            break;
        case DataTypeCategory::DINT :     
            
            if (!DataTypeUtils::ValueToDint(Value, DIntValue)){
                __configuration->PCLogger->RaiseException(
                    "DInt conversion error !");
            }
            std::memcpy(
                FieldVariable->__MemoryLocation
                    .GetPointerToMemory(FieldVariable->__ByteOffset),
                &DIntValue, sizeof(int32_t));
            break;
        case DataTypeCategory::LINT :     
            
            if (!DataTypeUtils::ValueToLint(Value, LIntValue)){
                __configuration->PCLogger->RaiseException(
                    "LInt conversion error !");
            }
            std::memcpy(
                FieldVariable->__MemoryLocation
                    .GetPointerToMemory(FieldVariable->__ByteOffset),
                &LIntValue, sizeof(int64_t));
            break;
        case DataTypeCategory::UINT :     
            
            if (!DataTypeUtils::ValueToUint(Value, UIntValue)){
                __configuration->PCLogger->RaiseException(
                    "UInt conversion error !");
            }
            std::memcpy(
                FieldVariable->__MemoryLocation
                    .GetPointerToMemory(FieldVariable->__ByteOffset),
                &UIntValue, sizeof(uint16_t));
            break;
        case DataTypeCategory::USINT :     
            
            if (!DataTypeUtils::ValueToUsint(Value, USIntValue)){
                __configuration->PCLogger->RaiseException(
                    "USInt conversion error !");
            }
            std::memcpy(
                FieldVariable->__MemoryLocation
                    .GetPointerToMemory(FieldVariable->__ByteOffset),
                &USIntValue, sizeof(uint8_t));
            break;
        case DataTypeCategory::UDINT :     
            
            if (!DataTypeUtils::ValueToUdint(Value, UDIntValue)){
                __configuration->PCLogger->RaiseException(
                    "UDInt conversion error !");
            }
            std::memcpy(
                FieldVariable->__MemoryLocation
                    .GetPointerToMemory(FieldVariable->__ByteOffset),
                &UDIntValue, sizeof(uint32_t));
            break;
        case DataTypeCategory::ULINT :     
            
            if (!DataTypeUtils::ValueToUlint(Value, ULIntValue)){
                __configuration->PCLogger->RaiseException(
                    "ULInt conversion error !");
            }
            std::memcpy(
                FieldVariable->__MemoryLocation
                    .GetPointerToMemory(FieldVariable->__ByteOffset),
                &ULIntValue, sizeof(uint64_t));
            break;
        case DataTypeCategory::REAL :     
            
            if (!DataTypeUtils::ValueToReal(Value, RealValue)){
                __configuration->PCLogger->RaiseException(
                    "Real conversion error !");
            }
            std::memcpy(
                FieldVariable->__MemoryLocation
                    .GetPointerToMemory(FieldVariable->__ByteOffset),
                &RealValue, sizeof(float));
            break;
        case DataTypeCategory::LREAL :     
            
            if (!DataTypeUtils::ValueToLReal(Value, LRealValue)){
                __configuration->PCLogger->RaiseException(
                    "LReal conversion error !");
            }
            std::memcpy(
                FieldVariable->__MemoryLocation
                    .GetPointerToMemory(FieldVariable->__ByteOffset),
                &LRealValue, sizeof(double));
            break;
        case DataTypeCategory::TIME :     
            
            if (!DataTypeUtils::ValueToTime(Value, Time)){
                __configuration->PCLogger->RaiseException(
                    "Time conversion error !");
            }
            std::memcpy(
                FieldVariable->__MemoryLocation
                    .GetPointerToMemory(FieldVariable->__ByteOffset),
                &Time, sizeof(TimeType));
            break;
        case DataTypeCategory::DATE :     
            
            if (!DataTypeUtils::ValueToDate(Value, Date)){
                __configuration->PCLogger->RaiseException(
                    "Date conversion error !");
            }
            std::memcpy(
                FieldVariable->__MemoryLocation
                    .GetPointerToMemory(FieldVariable->__ByteOffset),
                &Date, sizeof(DateType));
            break;
        case DataTypeCategory::TIME_OF_DAY :     
            
            if (!DataTypeUtils::ValueToTOD(Value, TOD)){
                __configuration->PCLogger->RaiseException(
                    "TOD conversion error !");
            }
            std::memcpy(
                FieldVariable->__MemoryLocation
                    .GetPointerToMemory(FieldVariable->__ByteOffset),
                &TOD, sizeof(TOD));
            break;
        case DataTypeCategory::DATE_AND_TIME :     
            
            if (!DataTypeUtils::ValueToDT(Value, Dt)){
                __configuration->PCLogger->RaiseException(
                    "Dt conversion error !");
            }
            std::memcpy(
                FieldVariable->__MemoryLocation
                    .GetPointerToMemory(FieldVariable->__ByteOffset),
                &Dt, sizeof(DateTODDataType));
            break;
        default :           __configuration->PCLogger->RaiseException(
                                "Only fields pointing to elementary data types"
                                " can be set with passed string values !");
    }
}

/*
 *  Given a NestedFieldName it will either copy "Value" itself to the appropriate
 *  offset or it may copy the content of Value at the appropriate offset.
 *  It will do something analogous to SetField iff NestedFieldName is a pointer
 *  field. Else, it will content with size = CopySizeBytes from the memory
 *  location pointer to by Value.
 */
void PCVariable::SetField(string NestedFieldName, void * Value,
                                    int CopySizeBytes) {

    CheckValidity();

    assert(__VariableDataType->IsFieldPresent(NestedFieldName) == true);
    if (!NestedFieldName.empty()) {
        DataTypeFieldAttributes Attributes;
        GetFieldAttributes(NestedFieldName, Attributes);

        if (Attributes.FieldInterfaceType == FieldIntfType::VAR_IN_OUT
            || Attributes.FieldInterfaceType 
                        == FieldIntfType::VAR_EXTERNAL
            || Attributes.FieldInterfaceType 
                        == FieldIntfType::VAR_ACCESS
            || Attributes.FieldInterfaceType 
                        == FieldIntfType::VAR_EXPLICIT_STORAGE) {
                // it is a pointer, we must get the pointed variable and set it there
                // This should basically do what SetField did.
                // Value is set at appropriate offset, not *Value.

                
                auto PointedVariable  =  GetPtrToField(
                        NestedFieldName);   
                
                /* 
                PointedVariable->SetField("", &Value,
                                sizeof(PCVariable *));*/
                std::memcpy(
                    PointedVariable->__MemoryLocation
                        .GetPointerToMemory(PointedVariable->__ByteOffset),
                    &Value, sizeof (PCVariable *));
                return;
        } 

        assert(this->__MemoryLocation.IsInitialized());
        PCVariable *FieldVariable = 
                GetPtrToField(NestedFieldName);

        if (FieldVariable->__VariableDataType->__DataTypeCategory 
                                            == DataTypeCategory::BOOL) {
            bool BoolValue = *(bool *)Value;

            if (BoolValue) // set bit at appropriate bit and byte offset
                FieldVariable->__MemoryLocation.GetStorageLocation().get()[
                    FieldVariable->__ByteOffset] 
                        |= ((1UL) << FieldVariable->__BitOffset);
            else
                FieldVariable->__MemoryLocation.GetStorageLocation().get()[
                    FieldVariable->__ByteOffset] 
                        &= ~((1UL) << FieldVariable->__BitOffset);
            return;
        }

        std::memcpy(
            this->__MemoryLocation
                    .GetPointerToMemory(FieldVariable->__ByteOffset),
            Value, CopySizeBytes);
        return;

    }

    if (__VariableDataType->__DataTypeCategory 
                                            == DataTypeCategory::BOOL) {
            bool BoolValue = *(bool *)Value;

            if (BoolValue) // set bit at appropriate bit and byte offset
                __MemoryLocation.GetStorageLocation().get()[__ByteOffset] 
                        |= ((1UL) << __BitOffset);
            else
                __MemoryLocation.GetStorageLocation().get()[__ByteOffset] 
                        &= ~((1UL) << __BitOffset);
            return;
    } else {
        std::memcpy(this->__MemoryLocation.GetPointerToMemory(__ByteOffset),
                Value, CopySizeBytes);
    }
    return;
    
}

void PCVariable::GetAndStoreValue(string NestedFieldName,
    void * Value, int CopySize, int CategoryOfDataType) {
    assert (Value != nullptr);
    assert(__VariableDataType->IsFieldPresent(NestedFieldName) == true);
    
    if (!NestedFieldName.empty()) {
        PCVariable* PointerAtFieldLocation = GetPtrStoredAtField(NestedFieldName);
        if (PointerAtFieldLocation != nullptr) {
            
            return PointerAtFieldLocation->GetAndStoreValue("", Value, CopySize,
                                                            CategoryOfDataType);

        } 
    }
    PCVariable * PointedVariable;

    if (!NestedFieldName.empty())
        PointedVariable =  GetPtrToField(NestedFieldName);
    else
        PointedVariable = this;
    
    assert(PointedVariable->__VariableDataType->__DataTypeCategory
        == CategoryOfDataType);

    if (CategoryOfDataType == DataTypeCategory::BOOL) {
        if (PointedVariable->__MemoryLocation.GetStorageLocation().get()[
            PointedVariable->__ByteOffset] 
            & ((1UL) << PointedVariable->__BitOffset)) {
            // bit pointed to by pointed variable is set
            *(bool *) Value = true;
        } else {
            *(bool *) Value = false;
        }
        return;
    }
    std::memcpy(Value, 
        PointedVariable->__MemoryLocation.GetPointerToMemory(
            PointedVariable->__ByteOffset),  CopySize);    
}

template <typename T> T PCVariable::GetValueStoredAtField(string NestedFieldName,
                                            int CategoryOfDataType) {
    T Value;
    int CopySize = sizeof(T);
    CheckValidity();
    GetAndStoreValue(NestedFieldName, &Value, CopySize, CategoryOfDataType);
    return Value;
}

void PCVariable::CheckOperationValidity(int CategoryOfDataType, int VarOp) {

    if (CategoryOfDataType == DataTypeCategory::NOT_ASSIGNED ||
        CategoryOfDataType == DataTypeCategory::DERIVED ||
        CategoryOfDataType == DataTypeCategory::POU ||
        CategoryOfDataType == DataTypeCategory::ARRAY)
            __configuration->PCLogger->RaiseException("Cannot perform operation"
                                    "on complex data type variables");
    switch(VarOp) {
        case VariableOps::ADD :     
            if (CategoryOfDataType == DataTypeCategory::DATE ||
                CategoryOfDataType == DataTypeCategory::DATE_AND_TIME)
                __configuration->PCLogger->RaiseException(
                    "Cannot perform + operation on date and date_time vars");
            break;
        
        case VariableOps::SUB :     
            if (CategoryOfDataType == DataTypeCategory::DATE ||
                CategoryOfDataType == DataTypeCategory::DATE_AND_TIME)
                __configuration->PCLogger->RaiseException(
                    "Cannot perform - operation on date and date_time vars");
            break;
        
        case VariableOps::MUL : 
        case VariableOps::DIV : 
        case VariableOps::MOD :
        case VariableOps::AND :
        case VariableOps::OR  :
        case VariableOps::XOR :
        case VariableOps::LS  :
        case VariableOps::RS  : 

            if (CategoryOfDataType == DataTypeCategory::DATE ||
                CategoryOfDataType == DataTypeCategory::DATE_AND_TIME ||
                CategoryOfDataType == DataTypeCategory::TIME_OF_DAY ||
                CategoryOfDataType == DataTypeCategory::TIME)
                __configuration->PCLogger->RaiseException(
                    "Cannot perform any of *,/,%%,&,|,^,<<,>> operation on "
                    "date, date_time, tod and time vars");
            break;           
            
    }

}

template <typename T> bool PCVariable::ArithmeticOpOnVariables(T var1, T var2,
                                        int CategoryOfDataType, int VarOp) {

     T result;
     switch(VarOp) {
        case VariableOps::ADD :     
            result = var1 + var2;
            this->SetField("", &result, sizeof(result));
            return true;
        
        case VariableOps::SUB :     
            result = var1 - var2;
            this->SetField("", &result, sizeof(result));
            return true;
        
        case VariableOps::MUL : 
            result = var1 * var2;
            this->SetField("", &result, sizeof(result));
            return true;
        case VariableOps::DIV : 
            result = var1/var2;
            this->SetField("", &result, sizeof(result));
            return true;
        case VariableOps::MOD :
            result = var1 % var2;
            this->SetField("", &result, sizeof(result));
            return true;
        default: __configuration->PCLogger->RaiseException("Unsupported "
                                    "variable arithmetic operation type !");
    }
    return false;
}

template <typename T> bool PCVariable::RelationalOpOnVariables(T var1, T var2,
                                int CategoryOfDataType, int VarOp) {
    bool test;
    switch(VarOp) {
        case VariableOps::EQ  :
            test = (var1 == var2);
            this->SetField("", &test, sizeof(test));
            return test;
        case VariableOps::GT  :
            test = (var1 > var2);
            this->SetField("", &test, sizeof(test));
            return test;      
        case VariableOps::GE  :
            test = (var1 >= var2);
            this->SetField("", &test, sizeof(test));
            return test;
        case VariableOps::LT  :
            test = (var1 < var2);
            this->SetField("", &test, sizeof(test));
            return test;
        case VariableOps::LE  :
            test = (var1 <= var2);
            this->SetField("", &test, sizeof(test));
            return test;
        default :   __configuration->PCLogger->RaiseException("Unsupported "
                                    "variable relational operation type !");
    }
    return false;
}

template <typename T> bool PCVariable::BitwiseOpOnVariables(T var1, T var2,
                            int CategoryOfDataType, int VarOp) {
    T result;
    switch(VarOp) {
        case VariableOps::OR  :
            result = var1 | var2;
            this->SetField("", &result, sizeof(result));
            return true;
        case VariableOps::XOR :
            result = var1 ^ var2;
            this->SetField("", &result, sizeof(result));
            return true;
        case VariableOps::LS  :
            result = var1 << var2;
            this->SetField("", &result, sizeof(result));
            return true;
        case VariableOps::RS  :
            result = var1 >> var2;
            this->SetField("", &result, sizeof(result));
            return true;

         default: __configuration->PCLogger->RaiseException("Unsupported "
                                    "variable bitwise operation type !");
    }
    return false;
}

template <typename T> bool PCVariable::AllOpsOnVariables(T var1, T var2,
                                        int CategoryOfDataType, int VarOp) {

    T result;
    bool test;
    switch(VarOp) {
        case VariableOps::ADD :     
            result = var1 + var2;
            this->SetField("", &result, sizeof(result));
            return true;
        
        case VariableOps::SUB :     
            result = var1 - var2;
            this->SetField("", &result, sizeof(result));
            return true;
        
        case VariableOps::MUL : 
            result = var1 * var2;
            this->SetField("", &result, sizeof(result));
            return true;
        case VariableOps::DIV : 
            result = var1/var2;
            this->SetField("", &result, sizeof(result));
            return true;
        case VariableOps::MOD :
            result = var1 % var2;
            this->SetField("", &result, sizeof(result));
            return true;
        case VariableOps::AND :
            result = var1 & var2;
            this->SetField("", &result, sizeof(result));
            return true;

        case VariableOps::OR  :
            result = var1 | var2;
            this->SetField("", &result, sizeof(result));
            return true;
        case VariableOps::XOR :
            result = var1 ^ var2;
            this->SetField("", &result, sizeof(result));
            return true;
        case VariableOps::LS  :
            result = var1 << var2;
            this->SetField("", &result, sizeof(result));
            return true;
        case VariableOps::RS  :
            result = var1 >> var2;
            this->SetField("", &result, sizeof(result));
            return true;

        case VariableOps::EQ  :
            test = (var1 == var2);
            this->SetField("", &test, sizeof(test));
            return test;

        case VariableOps::GT  :
            test = (var1 > var2);
            this->SetField("", &test, sizeof(test));
            return test; 

        case VariableOps::GE  :
            test = (var1 >= var2);
            this->SetField("", &test, sizeof(test));
            return test;

        case VariableOps::LT  :
            test = (var1 < var2);
            this->SetField("", &test, sizeof(test));
            return test;

        case VariableOps::LE  :
            test = (var1 <= var2);
            this->SetField("", &test, sizeof(test));
            return test;
        default :   __configuration->PCLogger->RaiseException("Unsupported "
                                    "variable operation type !");

    }
    return false;
}

void PCVariable::operator=(PCVariable& V) {
    this->__ByteOffset = V.__ByteOffset;
    this->__BitOffset = V.__BitOffset;
    this->__VariableName = V.__VariableName;
    this->__VariableDataType = V.__VariableDataType;
    this->__MemoryLocation = V.__MemoryLocation;
    this->__configuration = V.__configuration;
    this->__MemAllocated = V.__MemAllocated;
    this->__IsDirectlyRepresented = V.__IsDirectlyRepresented;
    this->__AssociatedResource = V.__AssociatedResource;
}



bool PCVariable::InitiateOperationOnVariables(PCVariable& V, int VarOp) {

    assert(this->__VariableDataType->__DataTypeCategory
        == V.__VariableDataType->__DataTypeCategory);
    int CategoryOfDataType = V.__VariableDataType->__DataTypeCategory;
    CheckValidity();

    CheckOperationValidity(CategoryOfDataType, VarOp);
    auto varoptype = Utils::GetVarOpType(VarOp);

    float float_var1, float_var2, float_ress;
    double double_var1, double_var2, double_ress;

    switch(CategoryOfDataType) {
        case DataTypeCategory::BOOL :
            return AllOpsOnVariables<bool>(
                    this->GetValueStoredAtField<bool>("", CategoryOfDataType),
                    V.GetValueStoredAtField<bool>("", CategoryOfDataType),
                    CategoryOfDataType, VarOp);

        case DataTypeCategory::BYTE :
        case DataTypeCategory::SINT :     
            return AllOpsOnVariables<int8_t>(
                    this->GetValueStoredAtField<int8_t>("", CategoryOfDataType),
                    V.GetValueStoredAtField<int8_t>("", CategoryOfDataType),
                    CategoryOfDataType, VarOp);
        
        case DataTypeCategory::WORD :   
        case DataTypeCategory::INT :     
            return AllOpsOnVariables<int16_t>(
                    this->GetValueStoredAtField<int16_t>("", CategoryOfDataType),
                    V.GetValueStoredAtField<int16_t>("", CategoryOfDataType),
                    CategoryOfDataType, VarOp);

        case DataTypeCategory::DWORD :   
        case DataTypeCategory::DINT :     
            return AllOpsOnVariables<int32_t>(
                    this->GetValueStoredAtField<int32_t>("", CategoryOfDataType),
                    V.GetValueStoredAtField<int32_t>("", CategoryOfDataType),
                    CategoryOfDataType, VarOp);

        case DataTypeCategory::LWORD :   
        case DataTypeCategory::LINT :     
            return AllOpsOnVariables<int64_t>(
                    this->GetValueStoredAtField<int64_t>("", CategoryOfDataType),
                    V.GetValueStoredAtField<int64_t>("", CategoryOfDataType),
                    CategoryOfDataType, VarOp);

          
        case DataTypeCategory::USINT :     
            return AllOpsOnVariables<uint8_t>(
                    this->GetValueStoredAtField<uint8_t>("", CategoryOfDataType),
                    V.GetValueStoredAtField<uint8_t>("", CategoryOfDataType),
                    CategoryOfDataType, VarOp);

        case DataTypeCategory::UINT :     
            return AllOpsOnVariables<uint16_t>(
                    this->GetValueStoredAtField<uint16_t>("", CategoryOfDataType),
                    V.GetValueStoredAtField<uint16_t>("", CategoryOfDataType),
                    CategoryOfDataType, VarOp);
        
        case DataTypeCategory::UDINT :     
            return AllOpsOnVariables<uint32_t>(
                    this->GetValueStoredAtField<uint32_t>("", CategoryOfDataType),
                    V.GetValueStoredAtField<uint32_t>("", CategoryOfDataType),
                    CategoryOfDataType, VarOp);
        
        case DataTypeCategory::ULINT :     
            return AllOpsOnVariables<uint64_t>(
                    this->GetValueStoredAtField<uint64_t>("", CategoryOfDataType),
                    V.GetValueStoredAtField<uint64_t>("", CategoryOfDataType),
                    CategoryOfDataType, VarOp);

        case DataTypeCategory::CHAR :  
            if (varoptype == VarOpType::RELATIONAL) {   
                return RelationalOpOnVariables<char>(
                    this->GetValueStoredAtField<char>("", CategoryOfDataType),
                    V.GetValueStoredAtField<char>("", CategoryOfDataType),
                    CategoryOfDataType, VarOp);
            } else {
                __configuration->PCLogger->RaiseException("Bitwise and "
                "arithmetic ops not supported for char variables");
            }

        case DataTypeCategory::REAL : 
            if (varoptype == VarOpType::RELATIONAL) {  
                return RelationalOpOnVariables<float>(
                    this->GetValueStoredAtField<float>("", CategoryOfDataType),
                    V.GetValueStoredAtField<float>("", CategoryOfDataType),
                    CategoryOfDataType, VarOp);
            } else if(varoptype == VarOpType::ARITHMETIC) {
                float_var1 = this->GetValueStoredAtField<float>("",
                                    CategoryOfDataType);
                float_var2 = V.GetValueStoredAtField<float>("",
                                    CategoryOfDataType);
                if (VarOp == VariableOps::ADD) {
                    float_ress = float_var1 + float_var2;
                } else if (VarOp == VariableOps::SUB) {
                    float_ress = float_var1 - float_var2;
                } else if (VarOp == VariableOps::MUL) {
                    float_ress = float_var1*float_var2;
                } else if (VarOp == VariableOps::DIV) {
                    float_ress = float_var1/float_var2;
                } else {
                    __configuration->PCLogger->RaiseException("Mod operation "
                    "not supported for REAL variables!");
                }

                this->SetField("", &float_ress, sizeof(float_ress));
            } else {
                __configuration->PCLogger->RaiseException("Bitwise "
                " ops not supported for real variables");
            }

        case DataTypeCategory::LREAL :     
            if (varoptype == VarOpType::RELATIONAL) {  
                return RelationalOpOnVariables<double>(
                    this->GetValueStoredAtField<double>("", CategoryOfDataType),
                    V.GetValueStoredAtField<double>("", CategoryOfDataType),
                    CategoryOfDataType, VarOp);
            } else if(varoptype == VarOpType::ARITHMETIC) {
                double_var1 = this->GetValueStoredAtField<double>("",
                                    CategoryOfDataType);
                double_var2 = V.GetValueStoredAtField<double>("",
                                    CategoryOfDataType);
                if (VarOp == VariableOps::ADD) {
                    double_ress = double_var1 + double_var2;
                } else if (VarOp == VariableOps::SUB) {
                    double_ress = double_var1 - double_var2;
                } else if (VarOp == VariableOps::MUL) {
                    double_ress = double_var1*double_var2;
                } else if (VarOp == VariableOps::DIV) {
                    double_ress = double_var1/double_var2;
                } else {
                    __configuration->PCLogger->RaiseException("Mod operation "
                    "not supported for REAL variables!");
                }

                this->SetField("", &double_ress, sizeof(double_ress));
            } else {
                __configuration->PCLogger->RaiseException("Bitwise "
                " ops not supported for LReal variables");
            }
        
        case DataTypeCategory::TIME :
            if (varoptype == VarOpType::RELATIONAL) {    
                return RelationalOpOnVariables<TimeType>(
                    this->GetValueStoredAtField<TimeType>("", CategoryOfDataType),
                    V.GetValueStoredAtField<TimeType>("", CategoryOfDataType),
                    CategoryOfDataType, VarOp);
            } else if (varoptype == VarOpType::ARITHMETIC) { 
                return ArithmeticOpOnVariables<TimeType>(
                    this->GetValueStoredAtField<TimeType>("", CategoryOfDataType),
                    V.GetValueStoredAtField<TimeType>("", CategoryOfDataType),
                    CategoryOfDataType, VarOp); 
            } else {
                __configuration->PCLogger->RaiseException("Bitwise "
                " ops not supported for Time variables");
            }
        
        case DataTypeCategory::TIME_OF_DAY: 
            if (varoptype == VarOpType::RELATIONAL) {  
                return RelationalOpOnVariables<TODDataType>(
                    this->GetValueStoredAtField<TODDataType>("", CategoryOfDataType),
                    V.GetValueStoredAtField<TODDataType>("", CategoryOfDataType),
                    CategoryOfDataType, VarOp);
            } else if (varoptype == VarOpType::ARITHMETIC) {  
                return ArithmeticOpOnVariables<TODDataType>(
                    this->GetValueStoredAtField<TODDataType>("", CategoryOfDataType),
                    V.GetValueStoredAtField<TODDataType>("", CategoryOfDataType),
                    CategoryOfDataType, VarOp);
            } else {
                __configuration->PCLogger->RaiseException("Bitwise "
                " ops not supported for TOD variables");
            }
        
        case DataTypeCategory::DATE: 
            if (varoptype == VarOpType::RELATIONAL) {  
                return RelationalOpOnVariables<DateType>(
                    this->GetValueStoredAtField<DateType>("", CategoryOfDataType),
                    V.GetValueStoredAtField<DateType>("", CategoryOfDataType),
                    CategoryOfDataType, VarOp);
            } else if (varoptype == VarOpType::ARITHMETIC) { 
                return ArithmeticOpOnVariables<DateType>(
                    this->GetValueStoredAtField<DateType>("", CategoryOfDataType),
                    V.GetValueStoredAtField<DateType>("", CategoryOfDataType),
                    CategoryOfDataType, VarOp); 
            } else {
                __configuration->PCLogger->RaiseException("Bitwise "
                " ops not supported for Date variables");
            }

        case DataTypeCategory::DATE_AND_TIME: 
            if (varoptype == VarOpType::RELATIONAL) {  
                return RelationalOpOnVariables<DateTODType>(
                    this->GetValueStoredAtField<DateTODType>("", CategoryOfDataType),
                    V.GetValueStoredAtField<DateTODType>("", CategoryOfDataType),
                    CategoryOfDataType, VarOp);
            } else if (varoptype == VarOpType::ARITHMETIC) { 
                return ArithmeticOpOnVariables<DateType>(
                    this->GetValueStoredAtField<DateType>("", CategoryOfDataType),
                    V.GetValueStoredAtField<DateType>("", CategoryOfDataType),
                    CategoryOfDataType, VarOp); 
            } else {
                __configuration->PCLogger->RaiseException("Bitwise "
                " ops not supported for DateTOD variables");
            }

        default :   __configuration->PCLogger->RaiseException(" Operation "
                        " is not supported for variables of DataType "
                        + std::to_string(CategoryOfDataType));
        
    }
    return false;
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

bool operator==(PCVariable& V1, PCVariable& V2) {
    return V1.InitiateOperationOnVariables(V2, VariableOps::EQ);
}

bool operator>(PCVariable& V1, PCVariable& V2) {
    return V1.InitiateOperationOnVariables(V2, VariableOps::GT);
}

bool operator>=(PCVariable& V1, PCVariable& V2) {
    return V1.InitiateOperationOnVariables(V2, VariableOps::GE);
}

bool operator<(PCVariable& V1, PCVariable& V2) {
    return V1.InitiateOperationOnVariables(V2, VariableOps::LT);
}

bool operator<=(PCVariable& V1, PCVariable& V2) {
    return V1.InitiateOperationOnVariables(V2, VariableOps::LE);
}