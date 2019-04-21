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

    //! An enum describing different types of supported variable operations
    enum VariableOps {
        ADD,    /*!< Addition */
        SUB,    /*!< Subtraction */
        MUL,    /*!< Multiplication */
        DIV,    /*!< Division */
        MOD,    /*!< Mod operator */
        AND,    /*!< Bitwise and */
        OR,     /*!< Bitwise OR */
        XOR,    /*!< Bitwise XOR */
        LS, /*!< Left shift */
        RS, /*!< Right shift */
        EQ, /*!< Equality */
        GT, /*!< Greater than */
        LT, /*!< Less than */
        GE, /*!< Greater than or equal */
        LE  /*!< Lessert than or equal */
    };  

    //! An enum describing different types of variable operators
    enum VarOpType {
        RELATIONAL, 
        ARITHMETIC,
        BITWISE
    };

    //! Details of a data-type field
    typedef struct DataTypeFieldAttributesStruct {
        string NestedFieldName; /*!< Nested Field Name relative to the data type
                                    of ParentVariablePtr */

        unsigned long RelativeOffset; /*!< The data corresponding to the 
        nested field is stored at the specified relative offset from the start
        of the HoldVariablPtr's memory location*/

        PCDataTypeField FieldDetails;   /*!< More details on the field */

        PCVariable* HoldVariablePtr;    /*!< This field is set to a value
        different from the ParentVariablePtr iff the one or more of the 
        intermediate nested fields are pointers. In that case it is set
        to the outermost pointer */

        PCVariable* ParentVariablePtr; /*!< This field is set to point to the
        variable which want's to get attributes of the nested field */
    } DataTypeFieldAttributes;


    //! A class which wraps a variable and provides API to get/set its value
    class PCVariable {

        private:
            
            //! Copies data to the memory location pointed to by a nested field
            /*!
                \param Attributes: Specify details on the memory location of
                                    a nested field
                \param From: Either the pointer From itself is copied, or content
                                from From's memory location is copied. The amount
                                of data to be copied is specified by the Field
                                details present in Attributes. The pointer "From"
                                itself is copied iff the field is a pointer. 
            */
            void CopyToPCVariableFieldFromPointer(DataTypeFieldAttributes&
                                                Attributes, PCVariable * From);

            //! Copies data to the memory location pointed to by a nested field
            /*!
                \param NestedFieldName A nested field relative to this variable's
                                        datatype
                \param Value The source location from which data is to be copied
                \param CopySize The amount of data to be copied
                \param CategoryOfDataType: One of pc_specification::DataTypeCategory
            */
            void GetAndStoreValue(string NestedFieldName, void * Value,
                                int CopySize, int CategoryOfDataType);

            //! Checks if an operation can be performed on a datatype
            /*!
                \param CategoryOfDataType: One of pc_specification::DataTypeCategory
                \param VarOp: One of pc_emulator::VariableOps
                \return Raises an exception iff validation checks fail!
            */
            void CheckOperationValidity(int CategoryOfDataType, int VarOp);


            //! Perform an arithmetic operation on two values
            /*!
                \param value1 First Value
                \param value2 Second Value
                \param CategoryOfDataType   Data type of the values
                                    (One of pc_specification::DataTypeCategory)
                \param VarOp    One of pc_emulator::VariablOps 
            */
            template <typename T> bool ArithmeticOpOnVariables(T value1, T value2,
                                        int CategoryOfDataType, int VarOp);
                        
            //! Perform an relational operation on two values
            /*!
                \param value1 First Value
                \param value2 Second Value
                \param CategoryOfDataType   Data type of the values
                                    (One of pc_specification::DataTypeCategory)
                \param VarOp    One of pc_emulator::VariablOps 
            */
            template <typename T> bool RelationalOpOnVariables(T value1, T value2,
                                        int CategoryOfDataType, int VarOp);

            //! Perform an bitwise operation on two values
            /*!
                \param value1 First Value
                \param value2 Second Value
                \param CategoryOfDataType   Data type of the values
                                    (One of pc_specification::DataTypeCategory)
                \param VarOp    One of pc_emulator::VariablOps 
            */
            template <typename T> bool BitwiseOpOnVariables(T value1, T value2,
                                        int CategoryOfDataType, int VarOp);

            //! Perform an operation on two values
            /*!
                \param value1 First Value
                \param value2 Second Value
                \param CategoryOfDataType   Data type of the values
                                    (One of pc_specification::DataTypeCategory)
                \param VarOp    One of pc_emulator::VariablOps 
            */
            template <typename T> bool AllOpsOnVariables(T value1, T value2,
                                        int CategoryOfDataType, int VarOp);

            
            //! Initialize all the fields of a variable recursively
            /*!
                \param V    Variable to initialize
                \param InitialValue Initial value to be set for this variable
            */
            void InitializeVariable(PCVariable * V, string InitialValue);

            //! Set initial values for all non-pointer fields of the variable
            void InitializeAllNonPtrFields();

            //! Initializes all girectly represented fields of the variable 
            /*!
                Get pointers to memory locations and set these pointers as initial
                values of all the directly represented fields
            */
            void InitializeAllDirectlyRepresentedFields();
            
            void CheckValidity();

            //! Finds the field attributes for one of the intermediate fields
            /*!
                \param Fields: vector of intermediate field names extracted from
                            one single nested field name
                \param StartPos: The index of the intermediate field to consider now
                \param FieldAttributes: Returned field attributes for this
                                        intermediate field
                \param HolderVariable: Variable which holds this intermediate field
                    i.e the intermediate field is stored in this variable's memory
                    location
                \param Current: The intermediate field is a field of this "Current"
                            data-type

            */
            void ParseRemFieldAttributes(std::vector<string>& Fields,
                        int StartPos, DataTypeFieldAttributes& FieldAttributes,
                        PCVariable * HolderVariable, PCDataType * Current);

            //! Read a MMAP'ed or ordinary memory location
            /*!
                \param dst: Destination where the data read is stored
                \param From: Memory location to be read from
                \param CopySizeBytes: Amount of data to be read
                \param Offset:  Offset within the specified memory location
                    to start reading from 
            */
            void SafeMemRead(void * dst, PCMemUnit * From, int CopySizeBytes,
                                int Offset);
                
            //! Write to a MMAP'ed or ordinary memory location
            /*!
                \param To: Memory location to write to
                \param CopySizeBytes: Amount of data to be written
                \param Offset:  Offset within the specified memory location
                    to start writing from 
                \param src: Source from which data is to be read
            */
            void SafeMemWrite(PCMemUnit * To, int CopySizeBytes, int Offset,
                            void * src);

            //! Read a bit from a MMAP'ed or ordinary memory location
            /*!
                \param From: Memory location to be read from
                \param ByteOffset:  Offset within the specified memory location
                    to start reading from
                \param BitOffset Offset of the interested bit within the specified
                    byte
                \return True if the bit read is 1, else False
            */
            bool SafeBitRead(PCMemUnit * From, int ByteOffset, int BitOffset);


            //! Write a bit to a MMAP'ed or ordinary memory location
            /*!
                \param To: Memory location to be written to
                \param ByteOffset:  Offset within the specified memory location
                    to start writing from
                \param BitOffset Offset of the interested bit within the specified
                    byte
                \param value The value to be assigned to the specific bit
            */
            void SafeBitWrite(PCMemUnit * To, int ByteOffset, int BitOffset,
                            bool value);
            

        public:
            int __ByteOffset; /*!<  Starting point of the variable's data
                    relative to its memory location */

            int __BitOffset; /*!<  Starting point of the variable's data
                    relative to its memory location */

            int __TotalSizeInBits; /*!< Total size of the variable's data */

            bool __IsVariableContentTypeAPtr; /*!< Set to true if the variable's
                content is a pointer to another variable */

            bool __PrevValue; /*!< Value of the variable set during a previous
                read. It is set only if the variable is of BOOL type */

            string __VariableName; /*!< Name of the variable */

            PCDataType* __VariableDataType; /*!< Data Type of the variable */

            PCMemUnit __MemoryLocation; /*!< Memory location where its content
                is stored */

            PCConfiguration * __configuration; /*!< Parent configuration object */

            PCResource * __AssociatedResource; /*!< If created by a resource, it
                is set to point to the object of the resource which created it.
                Otherwise it is set to nullptr */
                                               
            std::unordered_map<std::string, std::unique_ptr<PCVariable>> 
                __AccessedFields; /*!< Hashmap to maintain ready to use variables
                    which describe any of the subfields of this variable */

            bool __MemAllocated, __IsDirectlyRepresented, __IsTemporary, 
                __FirstRead, __FirstReturn;

            DataTypeFieldAttributes __VariableAttributes; /*!< Stores details
                about this variable and its datatype */


            //! Constructor
            /*!
                \param configuration    Associated configuration
                \param AssociatedResource   Resource which creates this variable
                    or nullptr if the variable is created by the configuration
                    itself
                \param VariableName Name of the variable
                \param VariableDataTypeName Name of the datatype of the variable
            */
            PCVariable(PCConfiguration * configuration,
                    PCResource * AssociatedResource,
                    string VariableName,
                    string VariableDataTypeName);

            //! Cleaning up operations
            /*!
                Cleans up the allocated memory location and clears all newly
                accessed subfield variables
            */
            void Cleanup(); 

            //! Allocates oridary storage for this variable
            /*!
                Space equivalent to __TotalSizeInBits is reserved
            */
            void AllocateStorage();

            //! Allocates MMAP'ed storage for this variable
            /*!
                Space equivalent to __TotalSizeInBits is reserved
                \param SharedMemFilePath    Full path to shared mem file
            */
            void AllocateStorage(string SharedMemFilePath);  

            //! Allocates ordinary storage and initializes the variable
            /*!
                Initializes all non pointer and directly represented fields
            */
            void AllocateAndInitialize();

            //! Allocates MMAP'ed storage and initializes the variable
            /*!
                Initializes all non pointer and directly represented fields
            */
            void AllocateAndInitialize(string SharedMemFileName);

            //! Resolves/Initializes all external fields
            void ResolveAllExternalFields();

            //! Called by an executor on start up iff this variable is a POU category variable
            /*!
                Can be used to reinitialize all TEMPfields
            */
            void OnExecutorStartup();

            //! Returns a Copy of this variable
            /*!
                Allocates storage, copies all content and initializes
                the new variable. May only be called if a variable's content type
                is not a pointer
            */
            std::unique_ptr<PCVariable> GetCopy();
            

            //! Sets  value to the specified nested field
            /*!
                Accepts value as a string and sets it to the specified field.
                Can only be called when the datatype of the nested field is an
                elementary data type. 

                \param value A string which is in a specific format for each
                            elementary data type
                \param NestedFieldName: A nested field name whose value needs
                            to be set. Raises an exception of the data type of
                            the nested field is not elementary
            */
            void SetField(string NestedFieldName, string value);

            //! Sets the value to the specified nested field
            /*!
                Copies bytes from value into the memory location corresponding
                to the nested field. If the nested field is a pointer, then
                the "value" itself is copied to the memory location

                \param NestedFieldName: A nested field name which needs to be set
                \param value: Buffer containing data to be copied over
                \param CopySizeBytes: Size of data to be copied
            */
            void SetField(string NestedFieldName, void * value,
                            int CopySizeBytes);

            //! Sets the value to the specified nested field
            /*!
                Copies bytes into the memory location corresponding
                to the nested field. If the nested field is a pointer, then
                the "From" itself is copied to the memory location

                \param NestedFieldName: A nested field name which needs to be set
                \param From: Either the content stored in From's memory location
                    is copied or "From" itself is copied
            */
            void SetField(string NestedFieldName, PCVariable * From);


            //! Creates a new variable which points to a sub field
            /*!
                The memory location of the newly created variable points to
                the location where the content of the nested field was stored
                in the original variable.

                \sa __AccessedFields A new entry is added to the hashmap. If the
                    same subfield is requested again, a new variable is not created.
                    Instead the previously created variable is returned again.
                \param NestedFieldName: The Byte and Bit Offsets and MemLocation
                    of the new variable are set to match the physical location
                    where this nested field is stored

            */
            PCVariable* GetPtrToField(string NestedFieldName);

            //! Returns the current value stored at a nested field
            /*!
                Can only be used when the data type of the nested field is one
                of the elementary ones.
                \param NestedFieldName: The field whose content is to be returned
                \param CategoryOfDataType: One of pc_specification::DataTypeCategory
            */
            template <typename T> T GetValueStoredAtField(string NestedFieldName,
                                                        int CategoryOfDataType);

            //! Returns a pointer stored at a field
            /*!
                Only works if the nested field is of type VAR_IN_OUT,
                VAR_ACCESS, VAR_EXTERNAL or VAR_EXPLICIT_STORAGE
            */
            PCVariable * GetPtrStoredAtField(string NestedFieldName);

            //! Returns the attributes of a nested field
            /*!
                \param NestedFieldName The nested field name
                \param FieldAttributes  Returned field attributes
            */
            void GetFieldAttributes(string NestedFieldName, 
                            DataTypeFieldAttributes& FieldAttributes);

            //! Common function called to initiate operations on a variable
            /*!
                \param V    The operation is to be performed on V (if it is a unary operation)
                            or both on V and this variable (iff it is a binary operation)
                \param VarOp    Of type pc_emulator::VariableOps
                \return True on success
            */
            bool InitiateOperationOnVariables(PCVariable& V, int VarOp);

            //! Assignment operator
            /*!
                Copies content from V to this variable
            */
            PCVariable& operator=(PCVariable& V);

            //! ADD operator
            /*!
                Adds V to this variable and stores the result in this variable
            */
            PCVariable& operator+(PCVariable& V );


            //! Sub operator
            /*!
                Subtracts V from this variable and stores the result in this variable
            */
            PCVariable& operator-(PCVariable& V );


            //! Divide operator
            /*!
                Divides this variable by V and stores the result in this variable
            */
            PCVariable& operator/(PCVariable& V );


            //! Multiply operator
            /*!
                Multiply V by this variable and stores the result in this variable
            */
            PCVariable& operator*(PCVariable& V );


            //! Mod operator
            /*!
                Divides this variable by V and stores the remainder in this variable
            */
            PCVariable& operator%(PCVariable& V );


            //! AND operator
            /*!
                Bitwise ANDs V to this variable and stores the result in this variable
            */
            PCVariable& operator&(PCVariable& V );


            //! OR operator
            /*!
                ORs V to this variable and stores the result in this variable
            */
            PCVariable& operator|(PCVariable& V );


            //! XOR operator
            /*!
                XORs V to this variable and stores the result in this variable
            */
            PCVariable& operator^(PCVariable& V );


            //! Left Shift operator
            /*!
                Left shifts this variable by V and stores the result in this variable
            */
            PCVariable& operator<<(PCVariable& V );


            //! Right Shift operator
            /*!
                Right shifts this variable by V and stores the result in this variable
            */
            PCVariable& operator>>(PCVariable& V );


            //! NOT operator
            /*!
                Negates this variable
            */
            PCVariable& operator!();

            //! Equality operator
            /*!
                Checks if two variables are equal. Returns true only if their
                content is equal.
            */
            friend bool operator==(PCVariable& V1, PCVariable& V2) {
                int CopySize = V1.__TotalSizeInBits / 8;

                if (V1.__VariableDataType->__DataTypeCategory 
                        == DataTypeCategory::ARRAY
                    || V1.__VariableDataType->__DataTypeCategory 
                        == DataTypeCategory::DERIVED
                    || V1.__VariableDataType->__DataTypeCategory 
                        == DataTypeCategory::POU) {
                    if (V1.__TotalSizeInBits != V2.__TotalSizeInBits)
                        return false;
                    for (int i = 0 ; i < CopySize; i++) {
                        char * val1 
                            = V1.__MemoryLocation.GetPointerToMemory(
                                V1.__ByteOffset + i);
                        char * val2 = V2.__MemoryLocation.GetPointerToMemory(
                                V2.__ByteOffset + i);
                        if (*val1 != *val2)
                            return false;
                    }
                    return true;
                }
                
                return V1.InitiateOperationOnVariables(V2, VariableOps::EQ);
            }


            //! Greater than operator
            /*!
                Checks if V1 > V2
            */
            friend bool operator>(PCVariable& V1, PCVariable& V2) {
                return V1.InitiateOperationOnVariables(V2, VariableOps::GT);
            }

            //! Greater than or equal operator
            /*!
                Checks if V1 >= V2
            */
            friend bool operator>=(PCVariable& V1, PCVariable& V2) {
                return V1.InitiateOperationOnVariables(V2, VariableOps::GE);
            }


            //! Less than operator
            /*!
                Checks if V1 < V2
            */
            friend bool operator<(PCVariable& V1, PCVariable& V2) {
                return V1.InitiateOperationOnVariables(V2, VariableOps::LT);
            }


            //! Less than or equal operator
            /*!
                Checks if V1 <= V2
            */
            friend bool operator<=(PCVariable& V1, PCVariable& V2) {
                return V1.InitiateOperationOnVariables(V2, VariableOps::LE);
            }

            
    };
    
}

#endif