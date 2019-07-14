
/*
 * A binary which accepts a PLC specification prototx file, and launches and
 * runs the underlying PLC. Optionally it may also be instructed to run the
 * PLC in virtual time by registering with Kronos.
 * Usage: plc_runner [Options] -f <path_to_system_spec prototx file> 
 * 
 *        Optional options:
 *              -e: 1 or 0 to enable/disable kronos"
                -n: num_insns_per_round - only valid if Kronos is enabled
                -s: relative cpu speed - only valid if Kronos is enabled
 * 
*/
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

class InputParser{
    public:
        InputParser (int &argc, char **argv){
            for (int i=1; i < argc; ++i)
                this->tokens.push_back(std::string(argv[i]));
        }
        const std::string& getCmdOption(const std::string &option) const{
            std::vector<std::string>::const_iterator itr;
            itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
            if (itr != this->tokens.end() && ++itr != this->tokens.end()){
                return *itr;
            }
            static const std::string empty_string("");
            return empty_string;
        }
        bool cmdOptionExists(const std::string &option) const{
            return std::find(this->tokens.begin(), this->tokens.end(), option)
                   != this->tokens.end();
        }
    private:
        std::vector <std::string> tokens;
};

void signal_handler( int signal_num ) { 
    std::cout << "Interrupted! Stopping PLC .... " << std::endl; 
     
    if (configuration != nullptr) {
        configuration->StopAllResources();
    }  
} 
  

int main(int argc, char **argv) {

    signal(SIGINT, signal_handler); 
    InputParser input(argc, argv);
    bool enable_kronos = false;

    long num_insns_per_round = 1000000;
    long per_round_inc_ns;
    float relative_cpu_speed = 1.0;

    if (!input.cmdOptionExists("-f")) {
        std::cout << "ERROR: Missing specification file! " << std::endl;
        std::cout << "Usage: plc_runner -f <path_to_system_spec prototxt file>"
                " [-e <1 or 0 to enable/disable kronos>] "
                " [-n <num_insns_per_round - only valid if Kronos is enabled >] "
                " [-s <relative cpu speed - only valid if Kronos is enabled>]"
                << std::endl;
        exit(0);
    }

    if (input.cmdOptionExists("-e")) {
        string is_virtual = input.getCmdOption("-e");

        if (is_virtual == "1") {
            std::cout << "Starting PLC under virtual time ..." << std::endl;
            enable_kronos = true;
        }
        
    }


    if (input.cmdOptionExists("-n")) {
        num_insns_per_round = std::stol(input.getCmdOption("-n"));        
    }

    if (input.cmdOptionExists("-s")) {
        relative_cpu_speed = std::stof(input.getCmdOption("-s"));
        if (relative_cpu_speed <= 0.0) 
            relative_cpu_speed = 1.0;        
    }

    per_round_inc_ns = num_insns_per_round /  relative_cpu_speed;

    string SpecFile =  input.getCmdOption("-f");

    configuration = new PCConfigurationImpl(SpecFile, enable_kronos,
            per_round_inc_ns);

    std::cout << "########################################" << std::endl;
    std::cout << "Starting PLC Resources ...."  << std::endl;
    configuration->RunPLC();

    std::cout << "########################################" << std::endl;
    std::cout << "Finished Running PLC ... " << std::endl;
    std::cout << "Cleaning up ... " << std::endl;
    configuration->Cleanup();
    delete configuration;
    
    return 0;
}