
#include <iostream> 
#include <csignal> 
#include <ctime>
#include <ratio>
#include <chrono>
#include <thread>
#include <unistd.h>
extern "C"
{
    #include <Kronos_functions.h>   
}

#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/include/kronos_api.h"

using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldInterfaceType = pc_specification::FieldInterfaceType;


PCConfigurationImpl * configuration = nullptr;
bool exp_started = false;
  
void signal_handler( int signal_num ) { 
    std::cout << "Interrupted! Stopping PLC .... " << std::endl; 
     
    if (configuration != nullptr && exp_started == true) {
        std::cout << "Stopping Kronos experiment ..." << std::endl;
        stopExp();
        configuration->WaitForCompletion();
        configuration->Cleanup();
        delete configuration;
    }

    std::cout << "PLC Stopped ..." << std::endl;
    // terminate program   
    exit(0);   
} 
  



int main() {

    signal(SIGINT, signal_handler);  
    string SpecDir =  "/home/moses/OpenSCADA/examples/vt_plc";

    std::cout << "######## Initializing Kronos ##########" << std::endl;
    initializeExp(1);

    std::cout << "Reading System specification from: " 
        << SpecDir + "/system_specification.prototxt" << std::endl;
    configuration = new PCConfigurationImpl(SpecDir 
            + "/system_specification.prototxt");

    std::cout << "########################################" << std::endl;
    std::cout << "Starting PLC Resources ...."  << std::endl;
    configuration->LaunchPLC();

    std::cout << "Synchronizing and Freezing ...."  << std::endl;
    while (synchronizeAndFreeze(1) < 0) {
        std::cout << "Sync and Freeze Failed. Retrying in 1 sec\n";
        usleep(1000000);
    }

    std::cout << "Synchronizing and Freezing: SUCCESS"  << std::endl;

    int n_inc_per_round_us = 10;
    int run_time_us = 2000000;
    double curr_time = 0.0;
    int total_rounds = run_time_us/n_inc_per_round_us;

    for(int i = 0; i < total_rounds; i++) {
        
        progress_n_rounds(1);
        exp_started = true;

        if (i != 0 && i % 10000 == 0) {
            curr_time += 0.1;
            std::cout << "Curr Time: " << curr_time << std::endl;
        }
        
    }

    std::cout << "Stopping Kronos experiment ..." << std::endl;
    stopExp();
    configuration->WaitForCompletion();

    PCResourceImpl * resource 
        = (PCResourceImpl*) configuration->RegisteredResources->GetResource(
                    "CPU_001");

    PCVariable * Program_1 = resource->GetPOU("PROGRAM_1");
    std::cout << "Function Call Count = " 
    << Program_1->GetValueStoredAtField<int16_t>("FnCallCount", DataTypeCategory::INT)
    << std::endl;
    
    std::cout << "Cleaning up configuration ... " << std::endl;
    configuration->Cleanup();
    delete configuration;
    
    return 0;
}