#include <assert.h>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <regex>
#include <vector>
#include <queue>
#include <fstream>
#include <cstdio>

#include "src/pc_emulator/grpc_ext/include/ext_interface_grpc_api.h"

using namespace std;
using namespace pc_emulator;
using namespace pc_specification;
using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldIntfType = pc_specification::FieldInterfaceType;



void ExtInterfaceAPI::SetSensorInput(
                string ResourceName, int MemType, int ByteOffset, int BitOffset,
                string VariableDataTypeName, string Value) {

    if (MemType != pc_specification::MemType::INPUT_MEM) {
        std::domain_error("Sensor cannot access OUTPUT memory!");
    }
    auto V = __ConfigInterface.GetVariablePointerToResourceMem(
                ResourceName, MemType, ByteOffset, BitOffset,
                VariableDataTypeName);

    std::cout << "Called SetSensorInput: " << ResourceName
            << " " << MemType << " " << ByteOffset << " " << BitOffset
            << " " << VariableDataTypeName << " " << Value << std::endl;
    if (V != nullptr) {
        V->SetField("", Value);
    }
}

string ExtInterfaceAPI::GetActuatorOutput(
    string ResourceName, int MemType, int ByteOffset, int BitOffset,
    string VariableDataTypeName) {

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

    auto V = __ConfigInterface.GetVariablePointerToResourceMem(
                ResourceName, MemType, ByteOffset, BitOffset,
                VariableDataTypeName);
    if (V == nullptr)
        return "";

    switch(V->__VariableDataType->__DataTypeCategory) {
        case DataTypeCategory::BOOL:
            resbool = V->GetValueStoredAtField<bool>("", DataTypeCategory::BOOL);
            return std::to_string(resbool);

        case DataTypeCategory::BYTE:
            resbyte = V->GetValueStoredAtField<uint8_t>("", DataTypeCategory::BYTE);
            return "16#" + std::to_string(resbyte);

        case DataTypeCategory::WORD:
            resword = V->GetValueStoredAtField<uint16_t>("", DataTypeCategory::WORD);
            return "16#" + std::to_string(resword);

        case DataTypeCategory::DWORD:
            resdword = V->GetValueStoredAtField<uint32_t>("", DataTypeCategory::DWORD);
            return "16#" + std::to_string(resdword);

        case DataTypeCategory::LWORD:
            reslword = V->GetValueStoredAtField<uint64_t>("", DataTypeCategory::LWORD);
            return "16#" + std::to_string(reslword);

        case DataTypeCategory::CHAR:
            reschar = V->GetValueStoredAtField<uint8_t>("", DataTypeCategory::CHAR);
            return std::to_string(reschar);

        case DataTypeCategory::INT:
            resint = V->GetValueStoredAtField<int16_t>("", DataTypeCategory::INT);
            return std::to_string(resint);

        case DataTypeCategory::SINT:
            ressint = V->GetValueStoredAtField<int8_t>("", DataTypeCategory::SINT);
            return std::to_string(ressint);

        case DataTypeCategory::DINT:
            resdint = V->GetValueStoredAtField<int32_t>("", DataTypeCategory::DINT);
            return std::to_string(resdint);

        case DataTypeCategory::LINT:
            reslint = V->GetValueStoredAtField<int64_t>("", DataTypeCategory::LINT);
            return std::to_string(reslint);

        case DataTypeCategory::UINT:
            resuint = V->GetValueStoredAtField<uint16_t>("", DataTypeCategory::UINT);
            return std::to_string(resuint);

        case DataTypeCategory::USINT:
            resusint = V->GetValueStoredAtField<uint8_t>("", DataTypeCategory::USINT);
            return std::to_string(resusint);

        case DataTypeCategory::UDINT:
            resudint = V->GetValueStoredAtField<uint32_t>("", DataTypeCategory::UDINT);
            return std::to_string(resudint);

        case DataTypeCategory::ULINT:
            resulint = V->GetValueStoredAtField<uint64_t>("", DataTypeCategory::ULINT);
            return std::to_string(resulint);

        case DataTypeCategory::REAL:
            resreal = V->GetValueStoredAtField<float>("", DataTypeCategory::REAL);
            return std::to_string(resreal);

        case DataTypeCategory::LREAL:
            reslreal = V->GetValueStoredAtField<double>("", DataTypeCategory::LREAL);
            return std::to_string(reslreal);

        case DataTypeCategory::TIME:
            restime = V->GetValueStoredAtField<TimeType>("", DataTypeCategory::TIME);
            return "t#" + std::to_string(restime.SecsElapsed) + "s";

        case DataTypeCategory::DATE:
            resdate = V->GetValueStoredAtField<DateType>("", DataTypeCategory::DATE);
            return "d#" + DataTypeUtils::DateToDTString(resdate);

        case DataTypeCategory::TIME_OF_DAY:
            restod = V->GetValueStoredAtField<TODDataType>("", DataTypeCategory::TIME_OF_DAY);
            return "tod#" + std::to_string(restod.Hr) + ":" + std::to_string(restod.Min)
                + ":" + std::to_string(restod.Sec);

        case DataTypeCategory::DATE_AND_TIME:
            resdt = V->GetValueStoredAtField<DateTODDataType>("", DataTypeCategory::DATE_AND_TIME);
            return "dt#" + DataTypeUtils::DTToDTString(resdt);

        default:
            return "";

    }
}

