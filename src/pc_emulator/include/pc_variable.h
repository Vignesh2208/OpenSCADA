#ifndef __PC_EMULATOR_INCLUDE_PC_VARIABLE_H__
#define __PC_EMULATOR_INCLUDE_PC_VARIABLE_H__
#include <iostream>
#include <memory>
#include "pc_configuration.h"
#include "pc_datatype.h"
#include "pc_mem_unit.h"

using namespace std;

namespace pc_emulator {

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
        public:
            int __ByteOffset;
            int __BitOffset;
            string __VariableName;
            PCDataType* __VariableDataType;
            PCMemUnit __MemoryLocation;
            PCConfiguration * __configuration;


            PCVariable(PCConfiguration * configuration,
                    string VariableName,
                    string VariableDataTypeName);
            void AllocateStorage();
            void AllocateAndInitialize();
            void OnExecutorStartup();
            PCVariable GetPCVariableToField(string NestedFieldName);
            void CopyPCVariableFieldFromPointer(string NestedFieldName,
                PCVariable * From);
            void SetPCVariableField(string NestedFieldName, string value);

            void SetPCVariableField(string NestedFieldName, void * value,
                                    int CopySizeBytes);

            void GetAndStoreValue(string NestedFieldName, void * Value,
                                int CopySize, int DataTypeCategory);

            template <typename T> T GetFieldValue(string NestedFieldName,
                                            int DataTypeCategory);

            template <typename T> bool OperateOnVariables(T var1, T var2,
                                        int DataTypeCategory, int VarOp);

            bool InitiateOperationOnVariables(PCVariable& V, int VarOp);

            void InitializeVariable(PCVariable * V);
            void InitializeAllFields(); 

            void operator=(const PCVariable& V);

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