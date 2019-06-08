
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
        configuration->Cleanup();
        delete configuration;
    }

    std::cout << "PLC Stopped ..." << std::endl;  
    exit(0);   
} 
  

int main(int argc, char **argv) {

    signal(SIGINT, signal_handler); 
    InputParser input(argc, argv);
    bool enable_kronos = false;

    if (!input.cmdOptionExists("-f")) {
        std::cout << "ERROR: Missing specification file! " << std::endl;
        std::cout << "Usage: plc_runner -f <path_to_system_spec prototxt file>"
                " [-e ] "
                << std::endl;
        exit(0);
    }

    if (input.cmdOptionExists("-e")) {
        enable_kronos = true;
    }

    string SpecFile =  input.getCmdOption("-f");

    configuration = new PCConfigurationImpl(SpecFile, enable_kronos);

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