#include <iostream>
#include <cstdint>
#include <cstring>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <vector>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <cerrno>
#include <climits>


#include "src/pc_emulator/include/pc_variable.h"
#include "src/pc_emulator/include/pc_datatype.h"
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_resource.h"
#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/proto/configuration.pb.h"

using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldIntfType = pc_specification::FieldInterfaceType;
using PoUType = pc_specification::PoUType;
using FieldQualifiers = pc_specification::FieldQualifiers;

using SystemConfiguration = pc_system_specification::SystemConfiguration;
using ResourceSpecification = pc_specification::ResourceSpecification;
using MachineSpec = pc_specification::MachineSpecification;
#define STRING(s) #s

bool Utils::IsOperandImmediate(string Operand) {
    if (Operand.empty())
        return false;
    if (Operand[0] >= '0' && Operand[0] <= '9') 
        return true;

    if (Operand.find("#") != string::npos)
        return true;

    if(Operand[0] == '"')
        return true;

    if (Operand == "TRUE" || Operand == "true" || Operand == "True" ||
        Operand == "FALSE" || Operand == "false" || Operand == "False")
        return true;
    return false;

}

void Utils::ExtractCallInterfaceMapping(
    std::unordered_map<string, string>& VarsToSet,
    std::unordered_map<string, string>& VarsToGet,
    string FullInterfaceString) {
    std::vector<string> Values;
    string Init;

    boost::trim_if(FullInterfaceString, boost::is_any_of("\t ,()"));
    boost::split(Values, FullInterfaceString,
    boost::is_any_of(",\n\t"), boost::token_compress_on); 

    for (int i = 0; i < Values.size(); i++) {
        boost::trim_if(Values[i], boost::is_any_of("\t "));
        auto pos = Values[i].find(":=");
        if (pos != string::npos) {
            string VarToSet, ValueToSet;

            VarToSet = Values[i].substr(0, pos);
            ValueToSet = Values[i].substr(pos + 2);
            boost::trim_if(VarToSet, boost::is_any_of("\t "));
            boost::trim_if(ValueToSet, boost::is_any_of("\t "));

            VarsToSet.insert(std::make_pair(VarToSet, ValueToSet));
        } else {

            pos = Values[i].find("=>");
            if (pos != string::npos) {
                string VarToGet, VarToSet;

                VarToGet = Values[i].substr(0, pos);
                VarToSet = Values[i].substr(pos + 2);
                boost::trim_if(VarToGet, boost::is_any_of("\t "));
                boost::trim_if(VarToSet, boost::is_any_of("\t "));

                VarsToGet.insert(std::make_pair(VarToGet, VarToSet));
            }
        }
    }    
}

bool Utils::GenerateFullSpecification(string SystemSpecificationPath,
    Specification& full_specification) {

    SystemConfiguration configuration;
    MachineSpec * machine_spec = new MachineSpec;
    int num_resources;
    int fileDescriptor = open(SystemSpecificationPath.c_str(),
                            O_RDONLY);

    if( fileDescriptor < 0 ) {
        std::cerr << " Error opening the specification file " 
                    << std::endl;
        return false;
    }

    google::protobuf::io::FileInputStream 
                                    fileInput(fileDescriptor);
    fileInput.SetCloseOnDelete( true );

    if (!google::protobuf::TextFormat::Parse(&fileInput,
                                    &configuration)) {
        std::cerr << std::endl << "Failed to parse system spec file!" 
        << std::endl;
        return false;
    }

    if (configuration.has_configuration_name())
        full_specification.set_config_name(configuration.configuration_name());
    
    if (configuration.has_log_level())
        full_specification.set_log_level(configuration.log_level());

    if (configuration.has_log_file_path())
        full_specification.set_log_file_path(configuration.log_file_path());

    if (configuration.has_run_time_secs())    
    	full_specification.set_run_time_secs(configuration.run_time_secs());
    else
	full_specification.set_run_time_secs(INT_MAX -1);
    num_resources = configuration.hardware_spec().num_resources();
    if (num_resources < 0 )
        return false;

    machine_spec->set_num_cpus(num_resources);
    machine_spec->set_ram_mem_size_bytes(
        configuration.hardware_spec().ram_mem_size_bytes());

    if (configuration.hardware_spec().has_random_number_seed()) 
        machine_spec->set_random_number_seed(
            configuration.hardware_spec().random_number_seed());

    for (auto& ins_spec: configuration.hardware_spec().ins_spec()) {
        auto new_ins = machine_spec->add_ins_spec();
        new_ins->CopyFrom(ins_spec);
    }


    for (auto& sfc_spec: configuration.hardware_spec().sfc_spec()) {
        auto new_sfc = machine_spec->add_sfc_spec();
        new_sfc->CopyFrom(sfc_spec);
    }

    for (auto& sfb_spec: configuration.hardware_spec().sfb_spec()) {
        auto new_sfb = machine_spec->add_sfb_spec();
        new_sfb->CopyFrom(sfb_spec);
    }

    for(auto& datatype_spec: configuration.datatype_declaration()) {
        auto new_data_type = full_specification.add_datatype_declaration();
        new_data_type->CopyFrom(datatype_spec);
    }

    for (auto& resource_file_path: configuration.resource_file_path()) {
	string full_resource_file_path = resource_file_path;
        if (resource_file_path[0] == '~') {
		full_resource_file_path.replace(0,1, std::getenv("HOME"));
	}
        int fd = open(full_resource_file_path.c_str(), O_RDONLY);
        auto resource_spec = machine_spec->add_resource_spec();
        if( fd < 0 ) {
            std::cerr << " Error opening resource specification. File: " 
                      << full_resource_file_path << " doesn't exist!" << std::endl;
            return false;
        }

        google::protobuf::io::FileInputStream 
                                        fileInput(fd);
        fileInput.SetCloseOnDelete(true);

        if (!google::protobuf::TextFormat::Parse(&fileInput,
                                        resource_spec)) {
            std::cerr << std::endl << "Failed to parse resource spec file!" 
             << full_resource_file_path << std::endl;
            return false;
        }
    }

    auto globals = configuration.release_var_global();
    auto access = configuration.release_var_access();
    full_specification.set_allocated_config_access_pou_var(access);
    full_specification.set_allocated_config_global_pou_var(globals);

    full_specification.set_allocated_machine_spec(machine_spec);
    return true;
}

bool Utils::IsNumType(PCDataType * data_type) {
    if (data_type->__DataTypeCategory >= DataTypeCategory::USINT
        && data_type->__DataTypeCategory <= DataTypeCategory::LREAL)
        return true;
    return false;
}

bool Utils::IsBitType(PCDataType * data_type) {
    if (data_type->__DataTypeCategory >= DataTypeCategory::BOOL
        && data_type->__DataTypeCategory <= DataTypeCategory::LWORD)
        return true;
    return false;
}

bool Utils::IsRealType(PCDataType * data_type) {
    if (data_type->__DataTypeCategory == DataTypeCategory::REAL
        || data_type->__DataTypeCategory == DataTypeCategory::LREAL)
        return true;
    return false;
}


bool Utils::SameClassOfDataTypes(PCDataType * DT1, PCDataType * DT2) {
    if (DT1->__DataTypeCategory == DT2->__DataTypeCategory)
        return true;
    if (IsBitType(DT1) && IsBitType(DT2))
        return true;
    if (IsNumType(DT1) && IsNumType(DT2))
        return true;

    if ((DT1->__DataTypeCategory == DataTypeCategory::BYTE
        && DT2->__DataTypeCategory == DataTypeCategory::CHAR) ||
        (DT1->__DataTypeCategory == DataTypeCategory::CHAR
        && DT2->__DataTypeCategory == DataTypeCategory::BYTE))
        return true;

    return false;
    
}

PCDataType* Utils::GetMostAppropriateTypeCast(PCVariable * CR,
        std::vector<PCVariable*>& Operands) {

    assert(CR != nullptr);
    if (Operands.size() == 0)
        return CR->__VariableDataType;

    PCDataType * DesiredDataType = nullptr;
    for(int i = 0; i < Operands.size(); i++) {
        if (!Operands[i]->__IsTemporary && !DesiredDataType) {
            DesiredDataType = Operands[i]->__VariableDataType;
        } else if(!Operands[i]->__IsTemporary 
            && Operands[i]->__VariableDataType != DesiredDataType) {
                return nullptr;
        }
    }

    if(!DesiredDataType) {
        DesiredDataType = CR->__VariableDataType;

        for(int i = 0; i < Operands.size(); i++) {
            if (Operands[i]->__IsTemporary && 
                Operands[i]->__VariableDataType->__SizeInBits > 
                DesiredDataType->__SizeInBits)

                DesiredDataType = Operands[i]->__VariableDataType;
            else if (Operands[i]->__IsTemporary
                    && Operands[i]->__VariableDataType->__SizeInBits == 
                        DesiredDataType->__SizeInBits
                    && Operands[i]->__VariableDataType->__DataTypeCategory > 
                        DesiredDataType->__DataTypeCategory) {
                DesiredDataType = Operands[i]->__VariableDataType;
            }
        }

        
        

    }
    if (!SameClassOfDataTypes(DesiredDataType, CR->__VariableDataType))
            return nullptr;

    for(int i = 0; i < Operands.size(); i++) {
        if (!SameClassOfDataTypes(DesiredDataType,
            Operands[i]->__VariableDataType))
            return nullptr;
    }

    return DesiredDataType;
}

PCVariable* Utils::ReallocateTmpVariable(PCConfiguration * configuration,
                PCVariable * Var, PCDataType * new_data_type,
                string InitialValue) {

    assert(Var != nullptr && new_data_type != nullptr);
    assert(Var->__IsTemporary);
    assert(new_data_type->__DataTypeCategory != DataTypeCategory::ARRAY
    && new_data_type->__DataTypeCategory != DataTypeCategory::DERIVED
    && new_data_type->__DataTypeCategory != DataTypeCategory::POU);

    if (Var->__VariableDataType->__DataTypeCategory 
        == new_data_type->__DataTypeCategory)
        return Var;

    if (!Var->__IsTemporary)
        return nullptr;
    
    int new_size = new_data_type->__SizeInBits/8;

    if (new_size == 0)
        new_size ++;


    if (Var->__VariableName == "__CurrentResult") {
        Var->__MemoryLocation.ReallocateStaticMemory(new_size);
            
        Var->__ByteOffset = 0;
        Var->__BitOffset = 0;
        Var->__IsTemporary = true;
        Var->__VariableDataType = new_data_type;
        Var->__TotalSizeInBits = new_size * 8;

        Var->__VariableAttributes.RelativeOffset = 0;
        Var->__VariableAttributes.HoldVariablePtr = Var;
        Var->__VariableAttributes.NestedFieldName = "";
        Var->__VariableAttributes.ParentVariablePtr = Var;
        Var->__VariableAttributes.FieldDetails.__InitialValue
                        = new_data_type->__InitialValue;
        Var->__VariableAttributes.FieldDetails.__FieldTypeName
                        = new_data_type->__DataTypeName;
        Var->__VariableAttributes.FieldDetails.__FieldName = "";
        Var->__VariableAttributes.FieldDetails.__FieldTypeCategory
                        = new_data_type->__DataTypeCategory;
        
        Var->__VariableAttributes.FieldDetails.__RangeMax = 
                        new_data_type->__RangeMax;
        Var->__VariableAttributes.FieldDetails.__RangeMin = 
                        new_data_type->__RangeMin;
        Var->__VariableAttributes.FieldDetails.__FieldTypePtr = 
                        new_data_type;
        Var->__VariableAttributes.FieldDetails.__FieldInterfaceType = 
                        FieldInterfaceType::NA;
        Var->__VariableAttributes.FieldDetails.__FieldQualifier = 
                        FieldQualifiers::NONE;

        
        Var->__VariableAttributes.FieldDetails.__NDimensions = 1;

        Var->__VariableAttributes.FieldDetails.__Dimension1 = -1; 
        Var->__VariableAttributes.FieldDetails.__Dimension2 = -1;
        Var->__PrevValue = false;

        Var->SetField("", InitialValue);
        return Var;
    }
    assert(Var->__AssociatedResource != nullptr);
    auto Resource = (PCResourceImpl *) Var->__AssociatedResource;
    auto NewVar = Resource->GetTmpVariable(new_data_type->__DataTypeName,
        InitialValue);
    
    return NewVar;    
    
}

PCVariable* Utils::BOOL_TO_ANY(PCConfiguration * configuration, 
    PCVariable * Var, PCDataType * new_data_type) {

    if(!Var 
    || Var->__VariableDataType->__DataTypeCategory != DataTypeCategory::BOOL)
        return nullptr;

    auto StoredValue = Var->GetValueStoredAtField<bool>("",
            DataTypeCategory::BOOL);

    string Result;
    assert(DataTypeUtils::BoolToAny(StoredValue,
        new_data_type->__DataTypeCategory,Result));

    return ReallocateTmpVariable(configuration, Var, new_data_type, Result);
}


PCVariable* Utils::BYTE_TO_ANY(PCConfiguration * configuration, 
    PCVariable * Var, PCDataType * new_datatype) {

    
    if(!Var 
    || Var->__VariableDataType->__DataTypeCategory != DataTypeCategory::BYTE)
        return nullptr;

    auto StoredValue = Var->GetValueStoredAtField<uint8_t>("",
            DataTypeCategory::BYTE);
    string Result;
        assert(DataTypeUtils::ByteToAny(StoredValue,
            new_datatype->__DataTypeCategory,Result));

    return ReallocateTmpVariable(configuration, Var, new_datatype, Result);

};

PCVariable* Utils::CHAR_TO_ANY(PCConfiguration * configuration, 
    PCVariable * Var, PCDataType * new_datatype) {

    
    if(!Var 
    || Var->__VariableDataType->__DataTypeCategory != DataTypeCategory::CHAR)
        return nullptr;

    auto StoredValue = Var->GetValueStoredAtField<uint8_t>("",
            DataTypeCategory::CHAR);

    string Result;
    assert(DataTypeUtils::CharToAny(StoredValue,
            new_datatype->__DataTypeCategory,Result));

    return ReallocateTmpVariable(configuration, Var, new_datatype, Result);

};

PCVariable* Utils::WORD_TO_ANY(PCConfiguration * configuration, 
    PCVariable * Var, PCDataType * new_datatype) {

    if(!Var 
    || Var->__VariableDataType->__DataTypeCategory != DataTypeCategory::WORD)
        return nullptr;

    auto StoredValue = Var->GetValueStoredAtField<uint16_t>("",
            DataTypeCategory::WORD);

    string Result;
    assert(DataTypeUtils::WordToAny(StoredValue,
        new_datatype->__DataTypeCategory,Result));

    return ReallocateTmpVariable(configuration, Var, new_datatype, Result);
}

PCVariable* Utils::DWORD_TO_ANY(PCConfiguration * configuration, 
    PCVariable * Var, PCDataType * new_datatype) {


    if(!Var 
    || Var->__VariableDataType->__DataTypeCategory != DataTypeCategory::DWORD)
        return nullptr;

    auto StoredValue = Var->GetValueStoredAtField<uint32_t>("",
            DataTypeCategory::DWORD);

    string Result;
    assert(DataTypeUtils::DWordToAny(StoredValue,
        new_datatype->__DataTypeCategory,Result));

    return ReallocateTmpVariable(configuration, Var, new_datatype, Result);
}

PCVariable* Utils::LWORD_TO_ANY(PCConfiguration * configuration, 
    PCVariable * Var, PCDataType * new_datatype) {


    if(!Var 
    || Var->__VariableDataType->__DataTypeCategory != DataTypeCategory::LWORD)
        return nullptr;

    auto StoredValue = Var->GetValueStoredAtField<uint64_t>("",
            DataTypeCategory::LWORD);

    string Result;
    assert(DataTypeUtils::LWordToAny(StoredValue,
        new_datatype->__DataTypeCategory,Result));

    return ReallocateTmpVariable(configuration, Var, new_datatype, Result);
}

PCVariable* Utils::INT_TO_ANY(PCConfiguration * configuration, 
    PCVariable * Var, PCDataType * new_datatype) {


    if(!Var 
    || Var->__VariableDataType->__DataTypeCategory != DataTypeCategory::INT)
        return nullptr;

    auto StoredValue = Var->GetValueStoredAtField<int16_t>("",
            DataTypeCategory::INT);

    string Result;
    assert(DataTypeUtils::IntToAny(StoredValue,
        new_datatype->__DataTypeCategory,Result));

    return ReallocateTmpVariable(configuration, Var, new_datatype, Result);
}

PCVariable* Utils::SINT_TO_ANY(PCConfiguration * configuration, 
    PCVariable * Var, PCDataType * new_datatype) {


    if(!Var 
    || Var->__VariableDataType->__DataTypeCategory != DataTypeCategory::SINT)
        return nullptr;

    auto StoredValue = Var->GetValueStoredAtField<int8_t>("",
            DataTypeCategory::SINT);

    string Result;
    assert(DataTypeUtils::SIntToAny(StoredValue,
        new_datatype->__DataTypeCategory,Result));

    return ReallocateTmpVariable(configuration, Var, new_datatype, Result);

}

PCVariable* Utils::DINT_TO_ANY(PCConfiguration * configuration, 
    PCVariable * Var, PCDataType * new_datatype) {


    if(!Var 
    || Var->__VariableDataType->__DataTypeCategory != DataTypeCategory::DINT)
        return nullptr;

    auto StoredValue = Var->GetValueStoredAtField<int32_t>("",
            DataTypeCategory::DINT);

    string Result;
    assert(DataTypeUtils::DIntToAny(StoredValue,
        new_datatype->__DataTypeCategory,Result));

    return ReallocateTmpVariable(configuration, Var, new_datatype, Result);

}

PCVariable* Utils::LINT_TO_ANY(PCConfiguration * configuration, 
    PCVariable * Var, PCDataType * new_datatype) {


    if(!Var 
    || Var->__VariableDataType->__DataTypeCategory != DataTypeCategory::LINT)
        return nullptr;

    auto StoredValue = Var->GetValueStoredAtField<int64_t>("",
            DataTypeCategory::LINT);

    string Result;
    assert(DataTypeUtils::LIntToAny(StoredValue,
        new_datatype->__DataTypeCategory,Result));


    return ReallocateTmpVariable(configuration, Var, new_datatype, Result);

}

PCVariable* Utils::UINT_TO_ANY(PCConfiguration * configuration, 
    PCVariable * Var, PCDataType * new_datatype) {


    if(!Var 
    || Var->__VariableDataType->__DataTypeCategory != DataTypeCategory::UINT)
        return nullptr;

    auto StoredValue = Var->GetValueStoredAtField<uint16_t>("",
            DataTypeCategory::UINT);

    string Result;
    assert(DataTypeUtils::UIntToAny(StoredValue,
        new_datatype->__DataTypeCategory,Result));

    return ReallocateTmpVariable(configuration, Var, new_datatype, Result);
}

PCVariable* Utils::USINT_TO_ANY(PCConfiguration * configuration, 
    PCVariable * Var, PCDataType * new_datatype) {


    if(!Var 
    || Var->__VariableDataType->__DataTypeCategory != DataTypeCategory::USINT)
        return nullptr;

    auto StoredValue = Var->GetValueStoredAtField<uint8_t>("",
            DataTypeCategory::USINT);

    string Result;
    assert(DataTypeUtils::USintToAny(StoredValue,
        new_datatype->__DataTypeCategory,Result));

    return ReallocateTmpVariable(configuration, Var, new_datatype, Result);
}

PCVariable* Utils::UDINT_TO_ANY(PCConfiguration * configuration, 
    PCVariable * Var, PCDataType * new_datatype) {


    if(!Var 
    || Var->__VariableDataType->__DataTypeCategory != DataTypeCategory::UDINT)
        return nullptr;

    auto StoredValue = Var->GetValueStoredAtField<uint32_t>("",
            DataTypeCategory::UDINT);

    string Result;
    assert(DataTypeUtils::UDintToAny(StoredValue,
        new_datatype->__DataTypeCategory,Result));

    return ReallocateTmpVariable(configuration, Var, new_datatype, Result);
}

PCVariable* Utils::ULINT_TO_ANY(PCConfiguration * configuration, 
    PCVariable * Var, PCDataType * new_datatype) {


    if(!Var 
    || Var->__VariableDataType->__DataTypeCategory != DataTypeCategory::ULINT)
        return nullptr;

    auto StoredValue = Var->GetValueStoredAtField<uint64_t>("",
            DataTypeCategory::ULINT);

    string Result;
    assert(DataTypeUtils::UlintToAny(StoredValue,
        new_datatype->__DataTypeCategory,Result));

    return ReallocateTmpVariable(configuration, Var, new_datatype, Result);
}

PCVariable* Utils::REAL_TO_ANY(PCConfiguration * configuration, 
    PCVariable * Var, PCDataType * new_datatype) {


    if(!Var 
    || Var->__VariableDataType->__DataTypeCategory != DataTypeCategory::REAL)
        return nullptr;

    auto StoredValue = Var->GetValueStoredAtField<float>("",
            DataTypeCategory::REAL);

    string Result;
    assert(DataTypeUtils::RealToAny(StoredValue,
        new_datatype->__DataTypeCategory,Result));

    return ReallocateTmpVariable(configuration, Var, new_datatype, Result);
}

PCVariable* Utils::LREAL_TO_ANY(PCConfiguration * configuration, 
    PCVariable * Var, PCDataType * new_datatype) {


    if(!Var 
    || Var->__VariableDataType->__DataTypeCategory != DataTypeCategory::LREAL)
        return nullptr;

    auto StoredValue = Var->GetValueStoredAtField<double>("",
            DataTypeCategory::LREAL);

    string Result;
    assert(DataTypeUtils::LRealToAny(StoredValue,
        new_datatype->__DataTypeCategory,Result));

    return ReallocateTmpVariable(configuration, Var, new_datatype, Result);
}

PCVariable* Utils::TIME_TO_ANY(PCConfiguration * configuration, 
    PCVariable * Var, PCDataType * new_datatype) {


    if(!Var 
    || Var->__VariableDataType->__DataTypeCategory != DataTypeCategory::TIME)
        return nullptr;

    auto StoredValue = Var->GetValueStoredAtField<TimeType>("",
            DataTypeCategory::TIME);

    string Result;
    assert(DataTypeUtils::TimeToAny(StoredValue,
        new_datatype->__DataTypeCategory, Result));

    return ReallocateTmpVariable(configuration, Var, new_datatype, Result);
}


PCVariable* Utils::DT_TO_TOD(PCConfiguration * configuration, 
    PCVariable * Var, PCDataType * new_datatype) {
    
    if(!Var 
    || Var->__VariableDataType->__DataTypeCategory 
        != DataTypeCategory::DATE_AND_TIME)
        return nullptr;
    auto StoredValue = Var->GetValueStoredAtField<DateTODDataType>("",
            DataTypeCategory::DATE_AND_TIME);
    string Result, Hr, Min, Sec;
    Hr = std::to_string(StoredValue.Tod.Hr);
    Min = std::to_string(StoredValue.Tod.Min);
    Sec = std::to_string(StoredValue.Tod.Sec);

    if (Hr.length() < 2)
        Hr = "0" + Hr;
    if (Min.length() < 2)
        Min = "0" + Min;
    if (Sec.length() < 2)
        Sec = "0" + Sec;

    Result = "tod#" + Hr + ":" + Min + ":" + Sec;


    return ReallocateTmpVariable(configuration, Var, new_datatype, Result);
}

PCVariable* Utils::DT_TO_DATE(PCConfiguration * configuration, 
    PCVariable * Var, PCDataType * new_datatype) {

    if(!Var 
    || Var->__VariableDataType->__DataTypeCategory 
        != DataTypeCategory::DATE_AND_TIME)
        return nullptr;

    auto StoredValue = Var->GetValueStoredAtField<DateTODDataType>("",
            DataTypeCategory::DATE_AND_TIME);
    string Result;
    string Month = std::to_string(StoredValue.Date.Month);
    string Day = std::to_string(StoredValue.Date.Day);

    if (Month.length() < 2)
        Month = "0" + Month;
    if (Day.length() < 2)
        Day = "0" + Day;

    Result = "d#" + std::to_string(StoredValue.Date.Year) + "-"
                + Month + "-" + Day;

    return ReallocateTmpVariable(configuration, Var, new_datatype, Result);

}

bool Utils::ReadAccessCheck(PCConfiguration * configuration,
        string CallingPoUType, string NestedFieldName) {
    auto CallingPoU = configuration->LookupDataType(CallingPoUType);

    assert(!NestedFieldName.empty());
    std::vector<string> results;
    bool AccessedPoUField = false;

    boost::split(results, NestedFieldName,
                boost::is_any_of("."), boost::token_compress_on);

    if (!CallingPoU) {
        configuration->PCLogger->LogMessage(LogLevels::LOG_INFO,
            "READ: CallingPoU: " + CallingPoUType + " not found !");
        return false;
    }

    if (!CallingPoU->IsFieldPresent(NestedFieldName)){
        configuration->PCLogger->LogMessage(LogLevels::LOG_INFO,
            "READ: NestedField: " + NestedFieldName + " not found !");
        return false;
    }

    string CurrNestedFieldName = "";
    for (int i = 0; i < results.size(); i++) {
        
        PCDataTypeField Result;

        
        CurrNestedFieldName += (results[i] + ".");
           
        CallingPoU->GetPCDataTypeField(CurrNestedFieldName, Result);

        if (CallingPoUType != Result.__HoldingPoUType) {

            if (Result.__FieldTypeCategory == DataTypeCategory::POU
                && !AccessedPoUField) {
                AccessedPoUField = true;

                continue;

            } else {
                if (Result.__FieldInterfaceType 
                        == FieldInterfaceType::VAR_OUTPUT ||
                    Result.__FieldInterfaceType 
                        == FieldInterfaceType::VAR_IN_OUT ||
                    Result.__FieldInterfaceType 
                        == FieldInterfaceType::VAR_EXPLICIT_STORAGE ||
                    Result.__FieldInterfaceType 
                        == FieldInterfaceType::VAR_EXTERNAL) {
                    //CallingPoUType = Result.__HoldingPoUType;
                    continue;
                } else {
                    return false;
                }
            }
        } else {
            continue;
        }
    }

    return true;
}

bool Utils::WriteAccessCheck(PCConfiguration * configuration,
        string CallingPoUType, string NestedFieldName) {
    auto CallingPoU = configuration->LookupDataType(CallingPoUType);
    std::vector<string> results;
    bool AccessedPoUField = false;

    boost::split(results, NestedFieldName,
                boost::is_any_of("."), boost::token_compress_on);


 
    if (!CallingPoU) {
        configuration->PCLogger->LogMessage(LogLevels::LOG_INFO,
            "WRITE: CallingPoU: " + CallingPoUType + " not found !");
        return false;
    }

    if (!CallingPoU->IsFieldPresent(NestedFieldName)){
        configuration->PCLogger->LogMessage(LogLevels::LOG_INFO,
            "WRITE: NestedField: " + NestedFieldName + " not found !");
        return false;
    }

    string CurrNestedFieldName = "";
    for (int i = 0; i < results.size(); i++) {
        CurrNestedFieldName += (results[i] + ".");

        PCDataTypeField Result;

        CallingPoU->GetPCDataTypeField(CurrNestedFieldName, Result);

        if (CallingPoUType != Result.__HoldingPoUType) {
            if (Result.__FieldTypeCategory == DataTypeCategory::POU
                && !AccessedPoUField) {
                AccessedPoUField = true;

                if (Result.__FieldInterfaceType 
                        == FieldInterfaceType::VAR_INPUT)
                    return false;
                continue;

            } else {
                if (Result.__FieldInterfaceType 
                        == FieldInterfaceType::VAR_INPUT ||
                    Result.__FieldInterfaceType 
                        == FieldInterfaceType::VAR_IN_OUT ||
                    Result.__FieldInterfaceType 
                        == FieldInterfaceType::VAR_EXTERNAL ||
                    Result.__FieldInterfaceType 
                        == FieldInterfaceType::VAR_EXPLICIT_STORAGE) {
                    //CallingPoUType = Result.__HoldingPoUType;
                    continue;
                } else {
                    return false;
                }
            }
        } else {
            if (Result.__FieldInterfaceType == FieldInterfaceType::VAR_INPUT)
                return false;
            continue;
        }
    }
    return true;
}
bool Utils::does_file_exist(const char * filename){
    std::ifstream infile(filename);
    return infile.good();
}
char * Utils::make_mmap_shared(int nElements, string FileName) {
        constexpr auto PROT_RW = PROT_READ | PROT_WRITE;
        constexpr auto MAP_ALLOC = MAP_SHARED;

        bool file_exists = does_file_exist(FileName.c_str());

        FILE * fp;
        
        if(file_exists)
            fp = fopen((FileName).c_str(), "a+");
        else
            fp = fopen((FileName).c_str(), "w+");

        
        fseek(fp,0,SEEK_END);
        int file_size = ftell(fp);
        std::cout << "Initializing MMAP file: " << FileName << std::endl;
        if (!file_exists || file_size < sizeof(char)*nElements) {
                char init = '\0';
                for(int i = 0; i < nElements; i++)
                    fwrite(&init, sizeof(char), 1, fp);
        }
        int fd = fileno(fp);

        assert (nElements > 0 && fd != -1);
        errno = 0;
        auto ptr = mmap(0,
                        sizeof(char)*nElements, PROT_RW, MAP_ALLOC, fd, 0);
        //std::cout << "MMAP Initialization Status: " << std::strerror(errno) 
        //<< std::endl;
        fclose(fp);
        return (char *)ptr;

    }


void Utils::ValidatePOUDefinition(PCVariable * POUVar, 
                                            PCConfiguration * configuration) {

    PCDataType * PoUDataType = POUVar->__VariableDataType;
    if (PoUDataType->__PoUType == PoUType::NOA) {
        if (PoUDataType->__FieldsByInterfaceType[
           FieldIntfType::VAR_ACCESS].size()) {
               configuration->PCLogger->RaiseException("A non POU datatype: "
                + POUVar->__VariableName + " cannot have access fields!");
        }
        if (PoUDataType->__FieldsByInterfaceType[
           FieldIntfType::VAR_EXPLICIT_STORAGE].size()) {
               configuration->PCLogger->RaiseException("A non POU datatype: "
                + POUVar->__VariableName + " cannot have Directly "
                "Represented fields!");
        }
        if (PoUDataType->__FieldsByInterfaceType[
           FieldIntfType::VAR_EXTERNAL].size()) {
               configuration->PCLogger->RaiseException("A non POU datatype: "
                + POUVar->__VariableName + " cannot have external fields!");
        }
        if (PoUDataType->__FieldsByInterfaceType[
           FieldIntfType::VAR_IN_OUT].size()) {
               configuration->PCLogger->RaiseException("A non POU datatype: "
                + POUVar->__VariableName + " cannot have IN_OUT fields!");
        }
        if (PoUDataType->__FieldsByInterfaceType[
           FieldIntfType::VAR_GLOBAL].size()) {
               configuration->PCLogger->RaiseException("A non POU datatype: "
                + POUVar->__VariableName + " cannot have GLOBAL fields!");
        }
        if (PoUDataType->__FieldsByInterfaceType[
           FieldIntfType::VAR_INPUT].size()) {
               configuration->PCLogger->RaiseException("A non POU datatype: "
                + POUVar->__VariableName + " cannot have INPUT fields!");
        }
        if (PoUDataType->__FieldsByInterfaceType[
           FieldIntfType::VAR_OUTPUT].size()) {
               configuration->PCLogger->RaiseException("A non POU datatype: "
                + POUVar->__VariableName + " cannot have OUTPUT fields!");
        }
        if (PoUDataType->__FieldsByInterfaceType[
           FieldIntfType::VAR].size()) {
               configuration->PCLogger->RaiseException("A non POU datatype: "
                + POUVar->__VariableName + " cannot have VAR fields!");
        }
        if (PoUDataType->__FieldsByInterfaceType[
           FieldIntfType::VAR_TEMP].size()) {
               configuration->PCLogger->RaiseException("A non POU datatype: "
                + POUVar->__VariableName + " cannot have VAR_TEMP fields!");
        }
       
        return;

    } else if (PoUDataType->__PoUType == PoUType::FC) {
        if (PoUDataType->__FieldsByInterfaceType[
           FieldIntfType::VAR_ACCESS].size()) {
               configuration->PCLogger->RaiseException("A FC: "
                + POUVar->__VariableName + " cannot have access fields!");
        }
        if (PoUDataType->__FieldsByInterfaceType[
           FieldIntfType::VAR_EXPLICIT_STORAGE].size()) {
               configuration->PCLogger->RaiseException("A FC: "
                + POUVar->__VariableName + " cannot have Directly "
                "Represented fields!");
        }
        if (PoUDataType->__FieldsByInterfaceType[
           FieldIntfType::VAR_EXTERNAL].size()) {
               configuration->PCLogger->RaiseException("A FC: "
                + POUVar->__VariableName + " cannot have external fields!");
        }
        if (PoUDataType->__FieldsByInterfaceType[
           FieldIntfType::VAR_GLOBAL].size()) {
               configuration->PCLogger->RaiseException("A FC: "
                + POUVar->__VariableName + " cannot have GLOBAL fields!");
        }
        if (PoUDataType->__FieldsByInterfaceType[
           FieldIntfType::VAR].size()) {
               configuration->PCLogger->RaiseException("A FC: "
                + POUVar->__VariableName + " cannot have VAR fields!");
        }   
    } else if (PoUDataType->__PoUType == PoUType::FB) {
        if (PoUDataType->__FieldsByInterfaceType[
           FieldIntfType::VAR_ACCESS].size()) {
               configuration->PCLogger->RaiseException("A FB: "
                + POUVar->__VariableName + " cannot have access fields!");
        }
        if (PoUDataType->__FieldsByInterfaceType[
           FieldIntfType::VAR_EXPLICIT_STORAGE].size()) {
               configuration->PCLogger->RaiseException("A FB: "
                + POUVar->__VariableName + " cannot have Directly "
                "Represented fields!");
        }
        if (PoUDataType->__FieldsByInterfaceType[
           FieldIntfType::VAR_GLOBAL].size()) {
               configuration->PCLogger->RaiseException("A FB: "
                + POUVar->__VariableName + " cannot have GLOBAL fields!");
        }
    } 
}

bool Utils::IsFieldTypePtr(int FieldInterfaceType) {
    if (FieldInterfaceType == FieldIntfType::VAR_IN_OUT
        || FieldInterfaceType == FieldIntfType::VAR_ACCESS
        || FieldInterfaceType == FieldIntfType::VAR_EXTERNAL
        || FieldInterfaceType == FieldIntfType::VAR_EXPLICIT_STORAGE)
        return true;
    return false;
}

bool Utils::TestEQPtrs(PCVariable * Var1, PCVariable *  Var2) {
    if (!Var1 || !Var2)
        return false;

    if (Var1->__MemoryLocation == Var2->__MemoryLocation
        && Var1->__ByteOffset == Var2->__ByteOffset &&
        Var1->__BitOffset == Var2->__BitOffset)
        return true;
    
    if (Var1->__MemoryLocation == Var2->__MemoryLocation) {
        if (Var1->__ByteOffset == Var2->__ByteOffset)
            std::cout << "Unequal Bit Offset " << std::endl;
        else
            std::cout << "Unequal Byte Offset " << std::endl;
    } else {
        std::cout << "Unequal MemLocations " << std::endl;
    }

    return false;
}

string Utils::GetInitialValueForArrayIdx(int Idx, string InitialValue,
                                            PCDataType * ElementDataType,
                                            PCConfiguration * configuration) {
    std::vector<string> InitialValues;
    string Init;


    boost::trim_if(InitialValue,boost::is_any_of("\t ,{}"));
    boost::split(InitialValues, InitialValue,
    boost::is_any_of(",{}"), boost::token_compress_on);
    if (!InitialValue.empty() &&  Idx < InitialValues.size()) {
        Init = InitialValues[Idx];
    }
    else { 
        auto got 
        = configuration->__DataTypeDefaultInitialValues.find(
                ElementDataType->__DataTypeCategory);
        assert(got 
            != configuration->__DataTypeDefaultInitialValues.end());
        Init = got->second;
    }
    return Init;
}

string Utils::GetInstallationDirectory() {

    string InstallationDir = std::getenv("OSCADA_INSTALLATION");

    return InstallationDir;
}

string Utils::GetElementaryDataTypeName(int Category) {
    switch(Category) {
        case DataTypeCategory::BOOL :   
                        return "BOOL";
        case DataTypeCategory::BYTE :     
                        return "BYTE";
        case DataTypeCategory::WORD :
                        return "WORD";  
        case DataTypeCategory::DWORD : 
                        return "DWORD";    
        case DataTypeCategory::LWORD :  
                        return "LWORD";  
        case DataTypeCategory::CHAR : 
                        return "CHAR";   
        case DataTypeCategory::INT :
                        return "INT";      
        case DataTypeCategory::SINT : 
                        return "SINT";    
        case DataTypeCategory::DINT : 
                        return "DINT";    
        case DataTypeCategory::LINT : 
                        return "LINT";    
        case DataTypeCategory::UINT :  
                        return "UINT";   
        case DataTypeCategory::USINT : 
                        return "USINT";    
        case DataTypeCategory::UDINT : 
                        return "UDINT";    
        case DataTypeCategory::ULINT :  
                        return "ULINT";   
        case DataTypeCategory::REAL : 
                        return "REAL";   
        case DataTypeCategory::LREAL :
                        return "LREAL";     
        case DataTypeCategory::TIME :
                        return "TIME";     
        case DataTypeCategory::DATE :
                        return "DATE";     
        case DataTypeCategory::TIME_OF_DAY : 
                        return "TOD";    
        case DataTypeCategory::DATE_AND_TIME :  
                        return "DT";   
    }
    return "NA";
}

int Utils::GetVarOpType(int varop) {
    switch(varop) {
        case VariableOps::ADD:
        case VariableOps::SUB:
        case VariableOps::MUL:
        case VariableOps::DIV:
        case VariableOps::MOD:    return VarOpType::ARITHMETIC;

        case VariableOps::AND:
        case VariableOps::OR:
        case VariableOps::XOR:
        case VariableOps::LS:
        case VariableOps::RS:     return VarOpType::BITWISE;

        default :   return VarOpType::RELATIONAL;
    }
}

bool Utils::ExtractFromStorageSpec(string StorageSpec, 
                        int * memType, int * ByteOffset, int * BitOffset) {
    if (!boost::starts_with(StorageSpec, "%") || StorageSpec.length() < 4) {
        std::cout << "Incorrect storage specification: " << StorageSpec << std::endl;
        return false;
    }

    assert(memType != nullptr && ByteOffset != nullptr && BitOffset != nullptr);
    if (StorageSpec[1] == 'M') 
        *memType = (int)MemType::RAM_MEM;
    else if (StorageSpec[1] == 'I') 
        *memType = (int)MemType::INPUT_MEM;
    else if (StorageSpec[1] == 'Q') 
        *memType = (int)MemType::OUTPUT_MEM;
    else
        return false;

    if (StorageSpec[2] == 'W') {
        *ByteOffset = std::stoi(StorageSpec.substr(3,  string::npos));
        *BitOffset = 0;
    } else {
        *ByteOffset = std::stoi(StorageSpec.substr(2, StorageSpec.find('.')));
        *BitOffset = std::stoi(StorageSpec.substr(StorageSpec.find('.') + 1,
                                                    string::npos));
    }

    
    return true;
}


bool Utils::ExtractFromAccessStorageSpec(PCConfiguration * __configuration,
                        string StorageSpec,  int * memType, int * ByteOffset,
                        int * BitOffset, string& CandidateResourceName) {

    CandidateResourceName = "NONE";
    if (StorageSpec.length() < 4)
        return false;

    assert(memType != nullptr && ByteOffset != nullptr && BitOffset != nullptr);

    if (boost::starts_with(StorageSpec,"%")) {
        assert(StorageSpec[1] == 'M');
        *memType = (int)MemType::RAM_MEM;
        if (StorageSpec[2] == 'W') {
            *ByteOffset = std::stoi(StorageSpec.substr(3,  string::npos));
            *BitOffset = 0;
        } else {
            *ByteOffset = std::stoi(StorageSpec.substr(2, StorageSpec.find('.')));
            *BitOffset = std::stoi(StorageSpec.substr(StorageSpec.find('.') + 1,
                                                        string::npos));
        }
        return true;
    } else {
        std::vector<string> results;
        boost::split(results, StorageSpec, [](char c){return c == '.';});

        if(results.size() == 1) {// no dot found, this must be some field of global variable
            return false;
        } else {
            string candidate_resource = results[0];
            PCResource * resource = 
                __configuration->RegisteredResources->GetResource(
                                                    candidate_resource);

            if (resource == nullptr) {// no resource by this name, must be some
                                     // nested field of global variable
                return false;
            } else {
                string RemStorageSpec = StorageSpec.substr(
                    StorageSpec.find('.') + 1, string::npos);
                CandidateResourceName = candidate_resource;
                if (!Utils::ExtractFromStorageSpec(RemStorageSpec, memType,
                            ByteOffset, BitOffset))
                    return false; // this must be a nested field of some resource variable
                else
                    return true;
            }
        }
    }

    return false;
}

string Utils::ResolveAliasName(string AliasName,
                                        PCConfiguration * __configuration) {
    PCDataType * field_type_ptr 
                = __configuration->LookupDataType(AliasName);
    while (true) {
        if (field_type_ptr->__AliasName != field_type_ptr->__DataTypeName) {
            field_type_ptr 
                = __configuration->LookupDataType(
                            field_type_ptr->__DataTypeName);
            assert(field_type_ptr != nullptr);
        } else {
            return field_type_ptr->__DataTypeName;
        }
    }                                    
}

void Utils::InitializeDataType(PCConfiguration * __configuration,
                            PCDataType * __new_data_type,
                            const pc_specification::DataType& DataTypeSpec) {
    for (auto& field : DataTypeSpec.datatype_field()) {

        string initial_value;
        s64 range_min;
        s64 range_max;
        int field_qualifier;
        string field_datatype_name = field.field_datatype_name();

        
        PCDataType * field_type_ptr 
            = __configuration->LookupDataType(field.field_datatype_name());

        //std::cout << "Data Type Name = " << field.field_datatype_name() << std::endl;
        assert(field_type_ptr != nullptr);
        initial_value = field.has_initial_value() ? field.initial_value()
                                : field_type_ptr->__InitialValue;
        range_min = field.has_range_min() ? field.range_min() 
                                : field_type_ptr->__RangeMin;
        range_max = field.has_range_max() ? field.range_max()
                                : field_type_ptr->__RangeMax;

        field_qualifier = field.has_field_qualifier() ? field.field_qualifier()
                                : FieldQualifiers::NONE;

        // READ_ONLY and READ_WRITE can only be specified for access variables
        if (field_qualifier == FieldQualifiers::READ_ONLY
            || field_qualifier == FieldQualifiers::READ_WRITE)
            field_qualifier = FieldQualifiers::NONE;

        // field qualifiers R_EDGE and F_EDGE can only be specified for
        // VAR_INPUT or VAR_EXPLICIT_STORAGE bool variables 
        if (field_type_ptr->__DataTypeCategory != DataTypeCategory::BOOL
            && (field.intf_type() != FieldIntfType::VAR_INPUT
              || field.intf_type() != FieldIntfType::VAR_EXPLICIT_STORAGE)) {
            field_qualifier = FieldQualifiers::NONE; 
        }
        
        if (field.has_initial_value())
            assert(field.intf_type() != VAR_EXPLICIT_STORAGE
                    && field.intf_type() != VAR_ACCESS
                    && field.intf_type() != VAR_EXTERNAL);
    

        if (field.intf_type() != FieldIntfType::VAR_EXPLICIT_STORAGE) {
            if (field.has_dimension_1() && !field.has_dimension_2()) {
                __new_data_type->AddArrayDataTypeField(field.field_name(),
                        field_datatype_name, field.dimension_1(),
                        initial_value,
                        field.intf_type(),
                        field_qualifier,
                        range_min,
                        range_max);
            } else if (field.has_dimension_1() && field.has_dimension_2()) {

                __new_data_type->AddArrayDataTypeField(field.field_name(),
                        field_datatype_name, field.dimension_1(),
                        field.dimension_2(),
                        initial_value,
                        field.intf_type(),
                        field_qualifier,
                        range_min,
                        range_max);

            } else {
                __new_data_type->AddDataTypeField(field.field_name(),
                        field_datatype_name, initial_value,
                        field.intf_type(),
                        field_qualifier,
                        range_min,
                        range_max);
            }
        }
        else if (field.intf_type() 
                    == FieldIntfType::VAR_EXPLICIT_STORAGE
                && field.has_field_storage_spec()) {
                
                assert(DataTypeSpec.datatype_category() 
                        == DataTypeCategory::POU);
                assert(DataTypeSpec.pou_type() 
                        == pc_specification::PoUType::PROGRAM);
            int mem_type = 0;
            int ByteOffset = 0;
            int BitOffset = 0;
            if (field.field_storage_spec().has_full_storage_spec()) {
                //extract memtype, byte and bit offsets from string specification
                
                if (!Utils::ExtractFromStorageSpec(
                        field.field_storage_spec().full_storage_spec(),
                        &mem_type, &ByteOffset, &BitOffset))
                    __configuration->PCLogger->RaiseException(
                        "Incorrectly formatted storage specification !");

            } else {
                mem_type = (int)field.field_storage_spec().mem_type();
                ByteOffset = field.field_storage_spec().byte_offset();
                BitOffset = field.field_storage_spec().bit_offset();
            }

            if (field_type_ptr->__DataTypeCategory != DataTypeCategory::BOOL)
                assert(BitOffset == 0); //ignore bit offset
            if (field.has_dimension_1() && !field.has_dimension_2()) {
                __new_data_type->AddArrayDataTypeFieldAT(field.field_name(),
                        field_datatype_name, field.dimension_1(),
                        initial_value,
                        field_qualifier,
                        range_min,
                        range_max,
                        mem_type, ByteOffset, BitOffset);
            } else if (field.has_dimension_1() && field.has_dimension_2()) {

                __new_data_type->AddArrayDataTypeFieldAT(field.field_name(),
                        field_datatype_name, field.dimension_1(),
                        field.dimension_2(),
                        initial_value,
                        field_qualifier,
                        range_min,
                        range_max,
                        mem_type, ByteOffset, BitOffset);

            } else {
                __new_data_type->AddDataTypeFieldAT(field.field_name(),
                        field_datatype_name, initial_value,
                        field_qualifier,
                        range_min,
                        range_max,
                        mem_type, ByteOffset, BitOffset);
            }

        }
    }
}

bool Utils::GetFieldAttributesForAccessPath(string AccessPathStorageSpec,
                PCConfiguration * configuration,
                DataTypeFieldAttributes& FieldAttributes){

    assert(!AccessPathStorageSpec.empty());
    std::vector<string> results;

    boost::split(results, AccessPathStorageSpec,
                boost::is_any_of("."), boost::token_compress_on);
    PCResource * resource = configuration->RegisteredResources->GetResource(
                        results[0]);
    if (resource == nullptr) {
        if (configuration->__global_pou_var != nullptr
        && configuration->__global_pou_var->__VariableDataType->IsFieldPresent(
                                                AccessPathStorageSpec)){
            configuration->__global_pou_var->GetFieldAttributes(
                        AccessPathStorageSpec, FieldAttributes);
            return true;
        }
        return false;
        
    } else {
        
        if (results.size() <= 2) {
            configuration->PCLogger->RaiseException(
            "Improperly formatted AccessPathStorageSpec: "
            + AccessPathStorageSpec);
        }

        string PoUName = results[1];

        string PoUVariableName = AccessPathStorageSpec.substr(
                        results[0].length() + results[1].length() + 2,
                        string::npos);

        auto pou_var = resource->__ResourcePoUVars.find(PoUName)->second.get();
        if (pou_var == nullptr) {
            configuration->PCLogger->RaiseException(
                "POU NAME: " + PoUName + " not found !");
        }

        if (pou_var->__VariableDataType->IsFieldPresent(PoUVariableName)) {
            DataTypeFieldAttributes FieldAttributes;
            pou_var->GetFieldAttributes(PoUVariableName, FieldAttributes);
            return true;
        } 
    }
    return false;

};

PCVariable * Utils::GetVariable(string NestedFieldName,
                PCConfiguration * configuration) {
    auto desired_variable = configuration->GetExternVariable(NestedFieldName);

    if(desired_variable)
        return desired_variable;

    std::vector<string> results;

    boost::split(results, NestedFieldName,
                boost::is_any_of("."), boost::token_compress_on);
    DataTypeFieldAttributes FieldAttributes;
    PCResource * resource = configuration->RegisteredResources->GetResource(
                        results[0]);
    if (resource == nullptr || results.size() <= 2) {
        configuration->PCLogger->LogMessage(LogLevels::LOG_INFO,
            resource == nullptr ? "Resource: " + results[0] + " Not found!" :
            "Nested FieldName: " + NestedFieldName + " in incorrect format !");
        return nullptr;
    }


    // NestedFieldName of the form: resource_name.pou_name.nested_variable_name
    string PoUName = results[1];

    string PoUVariableName = NestedFieldName.substr(
                    results[0].length() + results[1].length() + 2,
                    string::npos);


    auto pou_var = resource->__ResourcePoUVars.find(PoUName)->second.get();
    if (pou_var == nullptr) {
        configuration->PCLogger->LogMessage(LogLevels::LOG_INFO,
            "POU NAME: " + PoUName + " not found !");
        return nullptr;
    }

    if (pou_var->__VariableDataType->IsFieldPresent(PoUVariableName)) {
        DataTypeFieldAttributes FieldAttributes;
        pou_var->GetFieldAttributes(PoUVariableName, FieldAttributes);
        if (!Utils::IsFieldTypePtr(
            FieldAttributes.FieldDetails.__FieldInterfaceType))
            return pou_var->GetPtrToField(PoUVariableName);
        else
            return pou_var->GetPtrStoredAtField(PoUVariableName);
    } else {
        configuration->PCLogger->LogMessage(LogLevels::LOG_INFO,
            "POU NAME: " + PoUName + " Variable: " + PoUVariableName 
            + " not found !");
        return nullptr;
    }
}

void Utils::InitializeAccessDataType(PCConfiguration * __configuration,
                                PCDataType * __new_data_type,
                                const pc_specification::DataType& DataTypeSpec) {

    for (auto& field : DataTypeSpec.datatype_field()) {
        string initial_value;
        s64 range_min;
        s64 range_max;
        int field_qualifier;
        string field_datatype_name = field.field_datatype_name();

        
        PCDataType * field_type_ptr 
            = __configuration->LookupDataType(field.field_datatype_name());

        assert(field_type_ptr != nullptr);
        initial_value = field.has_initial_value() ? field.initial_value()
                                : field_type_ptr->__InitialValue;
        range_min = field.has_range_min() ? field.range_min() 
                                : field_type_ptr->__RangeMin;
        range_max = field.has_range_max() ? field.range_max()
                                : field_type_ptr->__RangeMax;

        field_qualifier = field.has_field_qualifier() 
                ? field.field_qualifier()
                : FieldQualifiers::READ_ONLY;

        // R_EDGE and F_EDGE cannot be specified for access variables
        if (field_qualifier == FieldQualifiers::R_EDGE
            || field_qualifier == FieldQualifiers::F_EDGE)
            field_qualifier = FieldQualifiers::READ_ONLY;

        assert(!field.has_initial_value());
                

        int mem_type = 0;
        int ByteOffset = 0;
        int BitOffset = 0;
        string CandidateResourceName;
        if (field.field_storage_spec().has_full_storage_spec()) {
            //extract memtype, byte and bit offsets from string specification

            assert(DataTypeSpec.datatype_category() 
                    == DataTypeCategory::POU);
            assert(DataTypeSpec.pou_type() 
                    == pc_specification::PoUType::PROGRAM);
            
            if (!Utils::ExtractFromAccessStorageSpec(
                    __configuration,
                    field.field_storage_spec().full_storage_spec(),
                    &mem_type, &ByteOffset, &BitOffset,
                    CandidateResourceName)) {

                DataTypeFieldAttributes FieldAttributes;

                assert(Utils::GetFieldAttributesForAccessPath(
                    field.field_storage_spec().full_storage_spec(),
                    __configuration, FieldAttributes));

                if (field.has_dimension_1() && !field.has_dimension_2()) {
                    __new_data_type->AddArrayDataTypeField(field.field_name(),
                            field_datatype_name, field.dimension_1(),
                            initial_value,
                            //field.intf_type(),
                            FieldInterfaceType::VAR_IN_OUT,
                            field_qualifier,
                            range_min,
                            range_max,
                            field.field_storage_spec().full_storage_spec());
                } else if (field.has_dimension_1() && field.has_dimension_2()) {

                    __new_data_type->AddArrayDataTypeField(field.field_name(),
                            field_datatype_name, field.dimension_1(),
                            field.dimension_2(),
                            initial_value,
                            //field.intf_type(),
                            FieldInterfaceType::VAR_IN_OUT,
                            field_qualifier,
                            range_min,
                            range_max,
                            field.field_storage_spec().full_storage_spec());

                } else {

                    if (FieldAttributes
                        .FieldDetails.__FieldTypeCategory 
                        != DataTypeCategory::ARRAY) {

                        __new_data_type->AddDataTypeField(
                            field.field_name(),
                            field_datatype_name, initial_value,
                            //field.intf_type(),
                            FieldInterfaceType::VAR_IN_OUT,
                            field_qualifier,
                            range_min,
                            range_max,
                            field.field_storage_spec().full_storage_spec());
                    } else if(FieldAttributes.FieldDetails
                            .__NDimensions == 2){
                        __new_data_type->AddArrayDataTypeField(
                            field.field_name(),
                            FieldAttributes
                            .FieldDetails.__FieldTypePtr->__DataTypeName,
                            FieldAttributes.FieldDetails.__Dimension1,
                            FieldAttributes.FieldDetails.__Dimension2,
                            initial_value,
                            //field.intf_type(),
                            FieldInterfaceType::VAR_IN_OUT,
                            field_qualifier,
                            range_min,
                            range_max,
                            field.field_storage_spec().full_storage_spec());
                    } else {
                        __new_data_type->AddArrayDataTypeField(
                            field.field_name(),
                            FieldAttributes
                            .FieldDetails.__FieldTypePtr->__DataTypeName,
                            FieldAttributes.FieldDetails.__Dimension1,
                            initial_value,
                            //field.intf_type(),
                            FieldInterfaceType::VAR_IN_OUT,
                            field_qualifier,
                            range_min,
                            range_max,
                            field.field_storage_spec().full_storage_spec());
                    }
                }


                continue; // these fields are added as a pointer. we will
                            // set these pointers later when a variable of this
                            // data type is created.
            }

        } else {

            assert(DataTypeSpec.datatype_category() 
                    == DataTypeCategory::POU);
            assert(DataTypeSpec.pou_type() 
                    == pc_specification::PoUType::PROGRAM);

            mem_type = (int)field.field_storage_spec().mem_type();
            ByteOffset = field.field_storage_spec().byte_offset();
            BitOffset = field.field_storage_spec().bit_offset();

            assert(field.field_storage_spec().mem_type() == MemType::RAM_MEM);
        }

        if (field_type_ptr->__DataTypeCategory != DataTypeCategory::BOOL)
                assert(BitOffset == 0); //ignore bit offset

        if (field.has_dimension_1() && !field.has_dimension_2()) {
            __new_data_type->AddArrayDataTypeFieldAT(field.field_name(),
                    field_datatype_name, field.dimension_1(),
                    initial_value,
                    field_qualifier,
                    range_min,
                    range_max,
                    mem_type, ByteOffset, BitOffset, CandidateResourceName,
                    field.field_storage_spec().full_storage_spec());
        } else if (field.has_dimension_1() && field.has_dimension_2()) {

            __new_data_type->AddArrayDataTypeFieldAT(field.field_name(),
                    field_datatype_name, field.dimension_1(),
                    field.dimension_2(),
                    initial_value,
                    field_qualifier,
                    range_min,
                    range_max,
                    mem_type, ByteOffset, BitOffset, CandidateResourceName,
                    field.field_storage_spec().full_storage_spec());

        } else {
            __new_data_type->AddDataTypeFieldAT(field.field_name(),
                    field_datatype_name,
                    initial_value,
                    field_qualifier,
                    range_min,
                    range_max,
                    mem_type, ByteOffset, BitOffset, CandidateResourceName,
                    field.field_storage_spec().full_storage_spec());
        }

    }

}
