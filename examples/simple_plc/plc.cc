#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/utils.h"
#include <iostream> 
#include <csignal> 
#include <ctime>
#include <ratio>
#include <chrono>
#include <thread>
#include <unistd.h>

using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldInterfaceType = pc_specification::FieldInterfaceType;


PCConfigurationImpl * configuration = nullptr;
  
void signal_handler( int signal_num ) { 
    std::cout << "Interrupted! Stopping PLC .... " << std::endl; 
     
    if (configuration != nullptr) {
        configuration->Cleanup();
        delete configuration;
    }

    std::cout << "PLC Stopped ..." << std::endl;
    // terminate program   
    exit(0);   
} 
  



int main() {

    signal(SIGINT, signal_handler);  
    string SpecDir = Utils::GetInstallationDirectory() 
            + "/examples/simple_plc";

    std::cout << "Reading System specification from: " 
        << SpecDir + "/system_specification.prototxt" << std::endl;
    configuration = new PCConfigurationImpl(SpecDir 
            + "/system_specification.prototxt");

    std::cout << "########################################" << std::endl;
    std::cout << "PLC Running ...."  << std::endl;
    configuration->RunPLC();

    PCResourceImpl * resource 
        = (PCResourceImpl*) configuration->RegisteredResources->GetResource(
                    "CPU_001");
    
    
    PCVariable * Program_1 = resource->GetPOU("PROGRAM_1");
    std::cout << "Function Call Count = " 
    << Program_1->GetValueStoredAtField<int16_t>("FnCallCount", DataTypeCategory::INT)
    << std::endl;
    
    while (true) {
        usleep(10000000);
    }
    
    return 0;
}