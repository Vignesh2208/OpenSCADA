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

    class PCVariable {

        private:
            void AllocateStorage();
            void CopyPCVariableFieldFromPointer(string NestedFieldName,
                                                PCVariable * From);

            void CopyPCVariableFieldFromPointer(DataTypeFieldAttributes&
                                                Attributes, PCVariable * From);
            void GetAndStoreValue(string NestedFieldName, void * Value,
                                int CopySize, int DataTypeCategory);

            template <typename T> bool OperateOnVariables(T var1, T var2,
                                        int DataTypeCategory, int VarOp);

            bool InitiateOperationOnVariables(PCVariable& V, int VarOp);

            void InitializeVariable(PCVariable * V);
            void InitializeAllNonPtrFields();
            void InitializeAllDirectlyRepresentedFields();
            void CheckValidity();

        public:
            int __ByteOffset;
            int __BitOffset;
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
            ~PCVariable();

            
            void AllocateAndInitialize();
            void OnExecutorStartup();
            PCVariable* GetPCVariableToField(string NestedFieldName);

            void SetVarExternalPtr(string FieldName, PCVariable * VarExtPtr);
            void SetVarAccessPtr(string FieldName, PCVariable * VarAccessPtr);
            void SetVarExplicitStoragePtr(string FieldName,
                            PCVariable * VarExplicitStoragePtr);
            void SetVarInOutPtr(string FieldName, PCVariable * VarInOutPtr);

            
            void SetPCVariableField(string NestedFieldName, string value);

            void SetPCVariableField(string NestedFieldName, void * value,
                                    int CopySizeBytes);

            

            template <typename T> T GetFieldValue(string NestedFieldName,
                                            int DataTypeCategory);
          

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

            bool operator == (PCVariable& V );
            bool operator > (PCVariable& V );
            bool operator < (PCVariable& V );
            bool operator <= (PCVariable& V );
            bool operator >= (PCVariable& V );

    };
}

#endif