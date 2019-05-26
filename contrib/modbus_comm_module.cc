
#include "src/pc_emulator/ext_modules/include/comm_module.h"
#include "contrib/modbus_comm_module.h"

using namespace std;

void print_usage() {
    std::cout << "./modbus_comm_module -f <path_plc_spec_prototxt> -i <ip_address_to_list>"
        " -p <listen_port> -r <resource_name>" << std::endl;

CommModule * comm_module = nullptr;
modbus_t *ctx;
uint8_t *query;
  
void signalHandler( int signal_num ) { 
    std::cout << "Interrupted! Stopping Comm Module .... " << std::endl; 
     
    if (comm_module != nullptr) {
        comm_module->Cleanup();
        delete comm_module;
        free(query);
        modbus_close(ctx);
        modbus_free(ctx);
    }

    std::cout << "Comm module Stopped ..." << std::endl;
    // terminate program   
    exit(0);   
} 

int main(int argc, char **argv) {
    InputParser input(argc, argv);
    modbus_mapping_t mb_mapping;
    int s, rc, header_length;


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

    comm_module = new CommModule(plc_filename);

    ctx = modbus_new_tcp(ip_address.c_str(), port_no);
    mb_mapping.nb_bits = comm_module->GetRAMMemSize()*8;
    mb_mapping.nb_input_bits = comm_module->GetInputMemSize(resource_name)*8;
    mb_mapping.nb_input_registers = comm_module->GetInputMemSize(resource_name);
    mb_mapping.nb_registers = comm_module->GetRAMMemSize();
    mb_mapping.start_bits = 0;
    mb_mapping.start_input_bits = 0;
    mb_mapping.start_input_registers = 0;
    mb_mapping.start_registers = 0;

    mb_mapping.tab_bits = comm_module->GetPtrToRAMMemory();
    mb_mapping.tab_input_bits = comm_module->GetPtrToInputMemory(resource_name);
    mb_mapping.tab_registers = mb_mapping.tab_bits;
    mb_mapping.tab_input_registers = mb_mapping.tab_input_bits;

    std::cout << "ModBUS server started (Press Ctrl-C to quit)..." << std::endl;
    s = modbus_tcp_listen(ctx, 1);
    modbus_tcp_accept(ctx, &s);
    query = malloc(MODBUS_TCP_MAX_ADU_LENGTH);

    header_length = modbus_get_header_length(ctx);
    modbus_set_debug(ctx, TRUE);


    for (;;) {
        do {
            rc = modbus_receive(ctx, query);
        } while (rc == 0);

        if (rc == -1 && errno != EMBBADCRC) {
            break;
        }

        rc = modbus_reply(ctx, query, rc, mb_mapping);
        if (rc == -1) {
            break;
        }
    }

    printf("Quit the loop: %s\n", modbus_strerror(errno));
    if (s != -1) {
        close(s);
    }
    
    free(query);
    modbus_close(ctx);
    modbus_free(ctx);

    comm_module->Cleanup();
    delete comm_module;
    std::cout << "Comm module Stopped ..." << std::endl;

    return 0;
}