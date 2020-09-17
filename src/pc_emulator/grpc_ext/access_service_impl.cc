


#include "src/pc_emulator/include/pc_variable.h"
#include "src/pc_emulator/include/pc_datatype.h"
#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_resource.h"
#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/proto/configuration.pb.h"
#include "src/pc_emulator/grpc_ext/include/access_service_impl.h"
#include "src/pc_emulator/grpc_ext/include/ext_interface_grpc_api.h"
#include "src/pc_emulator/proto/system_specification.pb.h"


using namespace std;
using namespace pc_emulator;


using SystemConfiguration = pc_system_specification::SystemConfiguration;

void AccessServiceImpl::read_directory(
    const std::string& name, std::vector<string>& v) {
    boost::filesystem::path p(name);
    boost::filesystem::directory_iterator start(p);
    boost::filesystem::directory_iterator end;
    std::transform(start, end, std::back_inserter(v), path_leaf_string());
}

bool AccessServiceImpl::CheckProtoFormat(string ProtoFilePath) {
    
    SystemConfiguration configuration;
    int fileDescriptor = open(ProtoFilePath.c_str(),
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

    return true;
}

grpc::Status AccessServiceImpl::SetSensorInput(ServerContext*, 
                const SensorInput* sensor_desc, mem_access::Status* status) {
    auto got = ExtInterface.find(sensor_desc->plc_id());
    if (got == ExtInterface.end()) {
        //std::cout << "PLC: " << sensor_desc->plc_id() << " not found!" 
        //    << std::endl;
        status->set_status("NOT_FOUND");
        return grpc::Status::OK;
    }
    auto PLCExtInterface = got->second.get();

    string ram_f_name = "/tmp/OpenSCADA/" 
    + PLCExtInterface->__ConfigInterface.__ConfigurationName + "/" + PLCExtInterface->__ConfigInterface.__ConfigurationName + "_RAM";
    if (!Utils::does_file_exist(ram_f_name.c_str())) {
	std::cout << "Set Sensor Input. RAM file does not exist ! " << std::endl;
        PLCExtInterface->__ConfigInterface.Cleanup();
        ExtInterface.erase(sensor_desc->plc_id());
        status->set_status("NOT_FOUND");
        return grpc::Status::OK;  
    }

    PLCExtInterface->SetSensorInput(sensor_desc->resourcename(), 
            sensor_desc->memtype(), sensor_desc->byteoffset(), 
            sensor_desc->bitoffset(), sensor_desc->variabledatatypename(),
            sensor_desc->valuetoset());
    status->set_status("SUCCESS");
    return grpc::Status::OK;

}
                
grpc::Status AccessServiceImpl::GetActuatorOutput(ServerContext*, 
    const ActuatorOutput* actuator_desc, mem_access::Result* result) {
    
    auto got = ExtInterface.find(actuator_desc->plc_id());
    if (got == ExtInterface.end()) {
        //std::cout << "PLC: " << actuator_desc->plc_id() << " not found!" 
        //    << std::endl;
        result->set_status("NOT_FOUND");
        result->set_value("NONE");
        return grpc::Status::OK;
    }
    auto PLCExtInterface = got->second.get();

    string ram_f_name = "/tmp/OpenSCADA/" 
    + PLCExtInterface->__ConfigInterface.__ConfigurationName + "/" + PLCExtInterface->__ConfigInterface.__ConfigurationName + "_RAM";
    if (!Utils::does_file_exist(ram_f_name.c_str())) {
	std::cout << "Get Actuator Output. RAM file: " << ram_f_name << " does not exist ! " << std::endl;
        PLCExtInterface->__ConfigInterface.Cleanup();
        ExtInterface.erase(actuator_desc->plc_id());
        result->set_status("NOT_FOUND");
        result->set_value("NONE");
        return grpc::Status::OK;  
    }

    string value = PLCExtInterface->GetActuatorOutput(actuator_desc->resourcename(), 
            actuator_desc->memtype(), actuator_desc->byteoffset(), 
            actuator_desc->bitoffset(), actuator_desc->variabledatatypename());
    
    //std::cout << "Value returned = " << value << std::endl;
    result->set_status("SUCCESS");
    result->set_value(value);
    return grpc::Status::OK;
}
