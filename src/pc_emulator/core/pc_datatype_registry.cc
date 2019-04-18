#include "src/pc_emulator/include/pc_datatype_registry.h"
#include "src/pc_emulator/include/pc_logger.h"
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_datatype.h"
#include "src/pc_emulator/include/task.h"

using namespace pc_emulator;
using namespace std;
using namespace pc_specification;

using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldInterfaceType = pc_specification::FieldInterfaceType;
using LogLevels = pc_specification::LogLevels;


void DataTypeRegistry::RegisterDataType(string DataTypeName,
                                        std::unique_ptr<PCDataType> DataType) {
    assert (__configuration != nullptr);
    assert (__configuration->PCLogger != nullptr);
    std::string LogMsg;

    auto got = __Registry.find (DataTypeName);
    if (got != __Registry.end()) {
        LogMsg = "Data Type " + DataTypeName + " Already Defined !";
        __configuration->PCLogger->RaiseException(LogMsg);
    } else {
        __Registry.insert(std::make_pair(DataTypeName,
                                        std::move(DataType)));
        LogMsg =  "Registered New Data Type: " + DataTypeName;
        __configuration->PCLogger->LogMessage(LogLevels::LOG_INFO, LogMsg);
    }
}

void DataTypeRegistry::GetAllRegisteredElementaryDataTypes(
                    std::vector<PCDataType*>& ElementaryDataTypes) {
    
    ElementaryDataTypes.push_back( __Registry.find ("BOOL")->second.get());
    ElementaryDataTypes.push_back( __Registry.find ("BYTE")->second.get());
    ElementaryDataTypes.push_back( __Registry.find ("WORD")->second.get());
    ElementaryDataTypes.push_back( __Registry.find ("DWORD")->second.get());
    ElementaryDataTypes.push_back( __Registry.find ("LWORD")->second.get());
    ElementaryDataTypes.push_back( __Registry.find ("INT")->second.get());
    ElementaryDataTypes.push_back( __Registry.find ("SINT")->second.get());
    ElementaryDataTypes.push_back( __Registry.find ("DINT")->second.get());
    ElementaryDataTypes.push_back( __Registry.find ("LINT")->second.get());
    ElementaryDataTypes.push_back( __Registry.find ("UINT")->second.get());
    ElementaryDataTypes.push_back( __Registry.find ("USINT")->second.get());
    ElementaryDataTypes.push_back( __Registry.find ("UDINT")->second.get());
    ElementaryDataTypes.push_back( __Registry.find ("ULINT")->second.get());
    ElementaryDataTypes.push_back( __Registry.find ("REAL")->second.get());
    ElementaryDataTypes.push_back( __Registry.find ("LREAL")->second.get());
    ElementaryDataTypes.push_back( __Registry.find ("TIME")->second.get());
    ElementaryDataTypes.push_back( __Registry.find ("DATE")->second.get());
    ElementaryDataTypes.push_back( __Registry.find ("TOD")->second.get());
    ElementaryDataTypes.push_back( __Registry.find ("DT")->second.get());

}

PCDataType * DataTypeRegistry::GetDataType(string DataTypeName) {
    assert (__configuration != nullptr);
    auto got = __Registry.find (DataTypeName);
    if (got == __Registry.end()) {
        //LogMsg = "Data Type " + DataTypeName + " Not Found !";
        //PCLogger->RaiseException(LogMsg);
    } else {
        return got->second.get();
    }
    return nullptr;
}

void DataTypeRegistry::Cleanup() {
    for ( auto it = __Registry.begin(); it != __Registry.end(); 
            ++it ) {
            PCDataType * __AccessedDataType = it->second.get();
            __AccessedDataType->Cleanup();
    }
}