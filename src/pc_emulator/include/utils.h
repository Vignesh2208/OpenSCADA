#ifndef __PC_EMULATOR_INCLUDE_PC_UTILS_H__
#define __PC_EMULATOR_INCLUDE_PC_UTILS_H__


#include <iostream>
#include <vector>
#include <climits>
#include <cstdlib>
#include <assert.h>
#include <unordered_map>
#include "src/pc_emulator/proto/configuration.pb.h"
#include "src/pc_emulator/proto/system_specification.pb.h"
#include <fcntl.h>
#include <sys/mman.h>


using namespace std;
using namespace pc_specification;

#define US_IN_MS 1000
#define US_IN_SEC 1000000
#define NS_IN_US 1000
#define NS_IN_MS 1000000
#define NS_IN_SEC 1000000000
#define MS_IN_SEC 1000

namespace pc_emulator {

    class PCDataType;
    class PCVariable;
    class PCConfiguration;
    struct DataTypeFieldAttributesStruct;

    //! Class containing Util functions. 
    /*!
      Util functions are used by other classes to perform initialization and
      book keeping operations
    */
    class Utils {

        public:
            //! Member function to initalize a datatype. 
            /*!
                Initializes all the fields of a data-type
                \param __configuration  A configuration object
                \param __new_data_type  The data-type object to be initialized
                \param DataTypeSpec detailed pecification of the data-type 
                                    fields
            */
            static void InitializeDataType(PCConfiguration * __configuration,
                                PCDataType * __new_data_type,
                                const pc_specification::DataType& DataTypeSpec);

            //! Member function to initalize all ACCESS POU fields. 
            /*!
                Initializes all the ACCESS fields specified in the configuration
                \param __configuration  A configuration object
                \param __new_data_type  The access datatype object to be 
                                        initialized
                \param DataTypeSpec detailed pecification of the data-type 
                                    fields
            */
            static void InitializeAccessDataType(
                PCConfiguration * __configuration, PCDataType * __new_data_type,
                const pc_specification::DataType& DataTypeSpec);

            //! Extract MemType, Byte and Bit Offsets from a string 
            /*!
                Extracts details from a storage spec string like %MW3.0
                \param StorageSpec  Storage specification string
                \param MemType  extracted memory type, (RAM/INPUT or OUTPUT)
                \param ByteOffset extracted byte offset specified within the
                                  memory
                \param BitOffset   extracted bit offset specified within the byte
                \return true if storage spec is in correct format, else false
            */
            static bool ExtractFromStorageSpec(string StorageSpec, 
                                            int * MemType, int * ByteOffset,
                                            int * BitOffset);

            //! Returns the datatype name given its category
            /*!
                \param DataTypeCategory  Category of the data-type from
                                         pc_specification::DataTypeCategory
                \return The data-type name is returned
            */
            static string GetElementaryDataTypeName(int DataTypeCategory);

            //! Returns the type of operation given an operator
            /*!
                The type of operation includes ARITHMETIC, BITWISE or RELATIONAL
                \param varop  Category of operation defined in pc_emulator::VariableOps
                \return Category of the variable operator defined in pc_emulator::VarOpType
            */
            static int GetVarOpType(int varop);

        
            //! Returns storage specification details for an access path
            /*!
                \param __configuration PCConfiguration object
                \param StorageSpec The storage specification string for the access path
                \param MemType Returned memory type (RAM or INPUT or OUTPUT)
                \param ByteOffset Returned byte offset in the specified memory
                \param BitOffset Returned bit offset within the specified byte
                \param CandidateResourceName If the specified storage spec is 
                                            of the form <resource_name>.<Memory_spec>
                                            then this contains the extracted resource name.
                                            Else it is "NONE".
                \return true if the storage spec is in the correct format, else false
            */
            static bool ExtractFromAccessStorageSpec(
            PCConfiguration * __configuration, string StorageSpec, 
                                            int * MemType, int * ByteOffset,
                                            int * BitOffset,
                                            string& CandidateResourceName);

            //! Returns the installation directory containing the OpenSCADA files
            static string GetInstallationDirectory();

            //! Returns the actual data-type name given one of its aliases
            /*!
                \param AliasName    The alias data-type name which needs to resolved
                \param __configuration  A PCConfiguration object
                \return Returns the original data-type name 
            */
            static string ResolveAliasName(string AliasName,
                                PCConfiguration * __configuration);

            //! Returns the initial value for an element of an array datatype
            /*!
                \param Idx The index of the desired array element
                \param InitialValue Initial value specified for the array
                \param ElementDataType A PCDataType pointer for the element's datatype
                \param configuration: A PCConfiguration object
                \return Returns the initial value of the specified index
            */
            static string GetInitialValueForArrayIdx(int Idx, string InitialValue,
                                            PCDataType * ElementDataType,
                                            PCConfiguration * configuration);

            //! Tests if two variables are equal
            /*!
                \param Var1 Variable-1
                \param Var2 Variable-2
                \return True iff both variable's memory locations and byte and bit
                        offsets are the same. Otherwise it returns false
            */
            static bool TestEQPtrs(PCVariable * Var1, PCVariable *  Var2);

            //! Given a field interface type, checks if it is a pointer field
            /*!
                \param FieldInterfaceType  One of the values defined in 
                                            pc_specification::FieldIntfTypes
                \return True iff FieldInterfaceType is VAR_IN_OUT, VAR_EXTERNAL
                                VAR_ACCESS or VAR_EXPLICIT_STORAGE.
            */
            static bool IsFieldTypePtr(int FieldInterfaceType);

            //! Performs validation check for a POU
            /*!
                \param  POUVar  The POU variable to check
                \param configuration    A PCConfiguration object
                \return Raises an exception if any of the validation checks fail
            */
            static void ValidatePOUDefinition(PCVariable * POUVar, 
                                            PCConfiguration * configuration);

            //! Checks if a file exists on disk
            static bool does_file_exist(const char * filename);

            //! Creates a MMAP'ed file
            /*!
                \param nElements: Sets the size of the MMAP'ed file to this value
                \param  FileName: MMAP'ed file path
                \return A char * pointer to the MMAP'ed memory
            */
            static char * make_mmap_shared(int nElements, string FileName);

            //! Returns a variable to the nested field name
            /*!
                Searches: 
                (1) global variables defined in the configuration, 
                (2) global variables defined in each resource
                (3) variables defined inside POU's of each resource
                \param NestedFieldName A nested field name
                \param configuration A PCConfiguration object
                \return A PCVariable pointer iff found or nullptr
            */
            static PCVariable * GetVariable(string NestedFieldName,
                PCConfiguration * configuration);

            //! Gets Field Attributes for a given access path
            /*!
                Given access path name, it returns its attributes including
                information on where the data is stored
                \param AccessPath   The access path name
                \param configuration    A PCConfiguration object
                \param FieldAttributes Attributes which are returned
                \return True iff AccessPath exists, else False
            */
            static bool GetFieldAttributesForAccessPath(string AccessPath,
                PCConfiguration * configuration,
                struct DataTypeFieldAttributesStruct& FieldAttributes);

            //! Checks if the calling POU can read the NestedField
            /*!
                \param configuration A PCConfiguration object
                \param CallingPoUType   Name of the CallingPoU
                \param NestedFieldName  The nested field name whose read permissions
                                        are to be checked
            */
            static bool ReadAccessCheck(PCConfiguration * configuration,
                string CallingPoUType, string NestedFieldName);
        
            //! Checks if the calling POU can access write to the NestedField
            /*!
                \param configuration A PCConfiguration object
                \param CallingPoUType   Name of the CallingPoU
                \param NestedFieldName  The nested field name whose write permissions
                                        are to be checked
            */
            static bool WriteAccessCheck(PCConfiguration * configuration,
                string CallingPoUType, string NestedFieldName);


            static PCVariable* ReallocateTmpVariable(PCConfiguration * configuration,
                PCVariable * Var, PCDataType * new_data_type,
                string InitialValue);

            static PCVariable* BOOL_TO_ANY(PCConfiguration * configuration, 
                PCVariable * Var, PCDataType * new_datatype);

            static PCVariable* BYTE_TO_ANY(PCConfiguration * configuration, 
                PCVariable * Var, PCDataType * new_datatype);

            static PCVariable* WORD_TO_ANY(PCConfiguration * configuration, 
                PCVariable * Var, PCDataType * new_datatype);

            static PCVariable* DWORD_TO_ANY(PCConfiguration * configuration, 
                PCVariable * Var, PCDataType * new_datatype);

            static PCVariable* LWORD_TO_ANY(PCConfiguration * configuration, 
                PCVariable * Var, PCDataType * new_datatype);

            static PCVariable* CHAR_TO_ANY(PCConfiguration * configuration, 
                PCVariable * Var, PCDataType * new_datatype);

            static PCVariable* INT_TO_ANY(PCConfiguration * configuration, 
                PCVariable * Var, PCDataType * new_datatype);

            static PCVariable* SINT_TO_ANY(PCConfiguration * configuration, 
                PCVariable * Var, PCDataType * new_datatype);

            static PCVariable* DINT_TO_ANY(PCConfiguration * configuration, 
                PCVariable * Var, PCDataType * new_datatype);

            static PCVariable* LINT_TO_ANY(PCConfiguration * configuration, 
                PCVariable * Var, PCDataType * new_datatype);

            static PCVariable* UINT_TO_ANY(PCConfiguration * configuration, 
                PCVariable * Var, PCDataType * new_datatype);

            static PCVariable* USINT_TO_ANY(PCConfiguration * configuration, 
                PCVariable * Var, PCDataType * new_datatype);

            static PCVariable* UDINT_TO_ANY(PCConfiguration * configuration, 
                PCVariable * Var, PCDataType * new_datatype);

            static PCVariable* ULINT_TO_ANY(PCConfiguration * configuration, 
                PCVariable * Var, PCDataType * new_datatype);

            static PCVariable* REAL_TO_ANY(PCConfiguration * configuration, 
                PCVariable * Var, PCDataType * new_datatype);

            static PCVariable* LREAL_TO_ANY(PCConfiguration * configuration, 
                PCVariable * Var, PCDataType * new_datatype);

            static PCVariable* DT_TO_TOD(PCConfiguration * configuration, 
                PCVariable * Var, PCDataType * new_datatype);

            static PCVariable* DT_TO_DATE(PCConfiguration * configuration, 
                PCVariable * Var, PCDataType * new_datatype);

            static bool IsNumType(PCDataType * data_type);

            static bool IsBitType(PCDataType * data_type);

            static bool IsRealType(PCDataType * data_type);

            static bool SameClassOfDataTypes(PCDataType * DT1,
                PCDataType * DT2);

            static PCDataType* GetMostAppropriateTypeCast(PCVariable * CR,
                    std::vector<PCVariable*>& Operands);

            static bool GenerateFullSpecification(
                string SystemSpecificationPath,
                Specification& full_specification);
    };
}   

#endif