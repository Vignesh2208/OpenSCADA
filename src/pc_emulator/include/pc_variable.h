#ifndef __PC_EMULATOR_INCLUDE_PC_VARIABLE_H__
#define __PC_EMULATOR_INCLUDE_PC_VARIABLE_H__
#include <iostream>
#include <memory>
#include <unordered_map>
#include "pc_datatype.h"
#include "pc_mem_unit.h"

using namespace std;

namespace pc_emulator {

    class PCConfiguration;
    class PCResource;
    class PCVariable;

    enum VariableOps {
        ADD,
        SUB,
        MUL,
        DIV,
        MOD,
        AND,
        OR,
        XOR,
        LS,
        RS,
        EQ,
        GT,
        LT,
        GE,
        LE
    };  

    enum VarOpType {
        RELATIONAL,
        ARITHMETIC,
        BITWISE
    };

    typedef struct DataTypeFieldAttributesStruct {
        unsigned long RelativeOffset;
        unsigned long SizeInBits;
        int FieldInterfaceType;
        PCDataType* FieldDataTypePtr;
        PCVariable* HoldVariablePtr;
        string NestedFieldName;
    } DataTypeFieldAttributes;

    class PCVariable {

        private:
            void AllocateStorage();

            void CopyToPCVariableFieldFromPointer(DataTypeFieldAttributes&
                                                Attributes, PCVariable * From);
            void GetAndStoreValue(string NestedFieldName, void * Value,
                                int CopySize, int CategoryOfDataType);
            void CheckOperationValidity(int CategoryOfDataType, int VarOp);

            template <typename T> bool ArithmeticOpOnVariables(T var1, T var2,
                                        int CategoryOfDataType, int VarOp);
            template <typename T> bool RelationalOpOnVariables(T var1, T var2,
                                        int CategoryOfDataType, int VarOp);
            template <typename T> bool BitwiseOpOnVariables(T var1, T var2,
                                        int CategoryOfDataType, int VarOp);
            template <typename T> bool AllOpsOnVariables(T var1, T var2,
                                        int CategoryOfDataType, int VarOp);

            

            void InitializeVariable(PCVariable * V, string InitialValue);
            void InitializeAllNonPtrFields();
            void InitializeAllDirectlyRepresentedFields();
            
            void CheckValidity();
            void ParseRemFieldAttributes(std::vector<string>& Fields,
                        int StartPos, DataTypeFieldAttributes& FieldAttributes,
                        PCVariable * HolderVariable);

        public:
            int __ByteOffset;
            int __BitOffset;

            bool __IsVariableContentTypeAPtr; // 1 - pointer, 0 - non_pointer
            string __VariableName;
            PCDataType* __VariableDataType;
            PCMemUnit __MemoryLocation;
            PCConfiguration * __configuration;
            PCResource * __AssociatedResource; // if null, associated with all
                                               // resources or entire configuration
            std::unordered_map<std::string,  PCVariable*> __AccessedFields;
            bool __MemAllocated, __IsDirectlyRepresented;


            PCVariable(PCConfiguration * configuration,
                    PCResource * AssociatedResource,
                    string VariableName,
                    string VariableDataTypeName);

            void Cleanup();    
            void AllocateAndInitialize();
            void ResolveAllExternalFields();
            void OnExecutorStartup();
            PCVariable* GetPCVariableToField(string NestedFieldName);


            void SetPtr(string NestedFieldName, PCVariable * ptr);
            PCVariable * GetPtrStoredAtField(string NestedFieldName);
            
            void SetPCVariableField(string NestedFieldName, string value);
            void SetPCVariableField(string NestedFieldName, void * value,
                                    int CopySizeBytes);

            void CopyToPCVariableFieldFromPointer(string NestedFieldName,
                                                PCVariable * From);
            template <typename T> T GetFieldValue(string NestedFieldName,
                                            int CategoryOfDataType);
            void GetFieldAttributes(string NestedFieldName, 
                            DataTypeFieldAttributes& FieldAttributes);


            bool InitiateOperationOnVariables(PCVariable& V, int VarOp);
            void operator=(PCVariable& V);
            PCVariable& operator+(PCVariable& V );
            PCVariable& operator-(PCVariable& V );
            PCVariable& operator/(PCVariable& V );
            PCVariable& operator*(PCVariable& V );
            PCVariable& operator%(PCVariable& V );
            PCVariable& operator&(PCVariable& V );
            PCVariable& operator|(PCVariable& V );
            PCVariable& operator^(PCVariable& V );
            PCVariable& operator<<(PCVariable& V );
            PCVariable& operator>>(PCVariable& V );

            
    };

    bool operator == (PCVariable& V1, PCVariable& V2);
    bool operator > (PCVariable& V1, PCVariable& V2);
    bool operator < (PCVariable& V1, PCVariable& V2);
    bool operator <= (PCVariable& V1, PCVariable& V2);
    bool operator >= (PCVariable& V1, PCVariable& V2);
    
}

#endif