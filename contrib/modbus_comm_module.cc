
/*
 * A binary which starts a Modbus server and attaches the server to a specific
 * CPU of a specific PLC. A modbus client or hmi can then query this server
 * to get /set values. The Modbus server uses the CommModule interface and 
 * can read from the CPU's input memory and write to the PLC's RAM memory
 * Usage:
 *  modbus_comm_module -f <path_plc_spec_prototxt> \
                       -i <ip_address_to_listen on> \
                       -p <listen_port> -r <cpu_resource_name>
 */
#include <iostream>
#include <cstdint>
#include <cstring>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>

extern "C"
{

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <modbus/modbus.h>   
}

#include "src/pc_emulator/ext_modules/include/comm_module.h"
#include "src/pc_emulator/include/utils.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldInterfaceType = pc_specification::FieldInterfaceType;

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

using namespace std;

void print_usage() {
    std::cout << "modbus_comm_module -f <path_plc_spec_prototxt> "
        "-i <ip_address_to_listen on>"
        " -p <listen_port> -r <resource_name>" << std::endl;
}

CommModule * comm_module = nullptr;
modbus_t *ctx;
uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
  
void signalHandler( int signal_num ) { 
    std::cout << "Interrupted! Stopping Comm Module .... " << std::endl; 
     
    if (comm_module != nullptr) {
        
        modbus_close(ctx);
        modbus_free(ctx);

        comm_module->Cleanup();
        delete comm_module;
    }

    std::cout << "Comm module Stopped ..." << std::endl;
    // terminate program   
    exit(0);   
} 

int main(int argc, char **argv) {
    InputParser input(argc, argv);
    modbus_mapping_t mb_mapping;
    int s, rc;


    signal(SIGINT, signalHandler);  

    if(input.cmdOptionExists("-h")){
        print_usage();
        exit(0);
    }

    if (!input.cmdOptionExists("-f") || !input.cmdOptionExists("-r")) {
        std::cout << "ERROR: Missing specification file/Resource Name! " << std::endl;
        print_usage();
        exit(0);
    }
    string ip_address = "127.0.0.1";
    string resource_name  = input.getCmdOption("-r");
    int port_no = 1502;


    if (input.cmdOptionExists("-i")) {
        ip_address = input.getCmdOption("-i");
    }

    if (input.cmdOptionExists("-p")) {
        port_no = std::stoi(input.getCmdOption("-p"));
    }
    string plc_filename = input.getCmdOption("-f");

    std::cout << "Initializing Comm Module ..." << std::endl;
    comm_module = new CommModule(plc_filename);

    ctx = modbus_new_tcp(ip_address.c_str(), port_no);
    if (ctx == NULL) {
        std::cout << "Failed to initialize ..." << std::endl;
        exit(-1);
    }
    mb_mapping.nb_bits = comm_module->GetRAMMemSize()*8;
    mb_mapping.nb_input_bits = comm_module->GetInputMemSize(resource_name)*8;
    mb_mapping.nb_input_registers = comm_module->GetInputMemSize(resource_name)/2;
    mb_mapping.nb_registers = comm_module->GetRAMMemSize()/2;
    mb_mapping.start_bits = 0;
    mb_mapping.start_input_bits = 0;
    mb_mapping.start_input_registers = 0;
    mb_mapping.start_registers = 0;

    mb_mapping.tab_bits = comm_module->GetPtrToRAMMemory();
    mb_mapping.tab_input_bits = comm_module->GetPtrToInputMemory(resource_name);
    mb_mapping.tab_registers = (uint16_t *) mb_mapping.tab_bits;
    mb_mapping.tab_input_registers = (uint16_t *) mb_mapping.tab_input_bits;

    while (1) {
        std::cout << "ModBUS waiting for new connection (Press Ctrl-C to quit)..." 
                << std::endl;
        fflush(stdout);
        s = modbus_tcp_listen(ctx, 1);
        modbus_tcp_accept(ctx, &s);
        memset(&query, 0, MODBUS_TCP_MAX_ADU_LENGTH);

        modbus_set_debug(ctx, TRUE);
        for (;;) {
            do {
		std::cout << "ModBUS waiting for new packet ..." 
                << std::endl;
                rc = modbus_receive(ctx, query);
            } while (rc == 0);

            if (rc == -1 && errno != EMBBADCRC) {
                break;
            }

	    std::cout << "ModBUS sending response ..." 
                << std::endl;
            rc = modbus_reply(ctx, query, rc, &mb_mapping);
            if (rc == -1) {
                break;
            }
        }

        std::cout << "Closing connection ..." << std::endl;
        if (s != -1) {
            close(s);
        }   
    }
    modbus_close(ctx);
    modbus_free(ctx);

    comm_module->Cleanup();
    delete comm_module;
    std::cout << "Comm module Stopped ..." << std::endl;

    return 0;
}
