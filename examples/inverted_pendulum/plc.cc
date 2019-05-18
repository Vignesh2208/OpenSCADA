
#include <iostream> 
#include <csignal> 
#include <ctime>
#include <ratio>
#include <chrono>
#include <thread>
#include <unistd.h>

#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/utils.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldInterfaceType = pc_specification::FieldInterfaceType;


PCConfigurationImpl * configuration = nullptr;


void signalHandler( int signum ) {
   std:: cout << "Interrupt signal (" << signum << ") received.\n";
   if (configuration != nullptr)
        configuration->stop = true;
}

int main() {

    signal(SIGINT, signalHandler);  
    string SpecDir =  "/home/moses/OpenSCADA/examples/inverted_pendulum";    
    std::cout << "Reading System specification from: " 
        << SpecDir + "/system_specification.prototxt" << std::endl;
    configuration = new PCConfigurationImpl(SpecDir 
            + "/system_specification.prototxt");

    std::cout << "########################################" << std::endl;
    std::cout << "Starting PLC Resources ...."  << std::endl;
    configuration->RunPLC();

    std::cout << "Finished Running PLC ..." << std::endl;
    PCResourceImpl * resource 
        = (PCResourceImpl*) configuration->RegisteredResources->GetResource(
                    "CPU_001");
    
    std::cout << "Cleaning up configuration ... " << std::endl;
    configuration->Cleanup();
    delete configuration;
    
    return 0;
}