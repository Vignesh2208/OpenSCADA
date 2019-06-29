#include <iostream>
#include <cstdint>
#include <cstring>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>

extern "C"
{

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <modbus/modbus.h>   
}

bool to_stop = false;

void signal_handler( int signal_num ) { 
    std::cout << "Interrupted! Stopping HMI .... " << std::endl; 
    to_stop = true;
} 

using namespace std;

int main(int argc, char **argv) {
    modbus_mapping_t mb_mapping;
    int s, rc, header_length, i;
    uint16_t *input_mem_registers = NULL;
    uint8_t *input_mem_bytes;
    modbus_t *ctx;

    ofstream logFile;
    std::remove("/tmp/hmi_readings.txt");
    logFile.open("/tmp/hmi_readings.txt");
    string ip_address = "127.0.0.1";
    int port_no = 1502;
    int input_mem_size_bytes = 10000;

    float current_theta =  0.0;


    input_mem_registers = (uint16_t *) malloc((input_mem_size_bytes /2));
    input_mem_bytes = (uint8_t *) input_mem_registers;
    memset(input_mem_registers, 0, (input_mem_size_bytes /2));

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    ctx = modbus_new_tcp(ip_address.c_str(), port_no);
    modbus_set_debug(ctx, TRUE);
    modbus_set_error_recovery(ctx,
                              MODBUS_ERROR_RECOVERY_LINK |
                              MODBUS_ERROR_RECOVERY_PROTOCOL);

    std::cout << "\n Connecting to ModBus Slave " << std::endl;

    if (modbus_connect(ctx) == -1) {
        fprintf(stdout, "Connection failed: %s\n", modbus_strerror(errno));
	fflush(stdout);
        modbus_free(ctx);
        return 0;
    }

    std::cout << "\nConnected to ModBus Slave " << std::endl;

    auto begin = std::chrono::steady_clock::now();
    

    
    while (true) {

        if (to_stop)
            break;

    
        // reads registers starting at 0 and ending at 3 i.e, registers 0 to 3
        rc = modbus_read_input_registers(ctx, 0, 3, input_mem_registers);

        if (rc < 0) {
            std::cout << "Error reading input registers ! Stopping HMI "
            << std::endl;
        }

	input_mem_bytes = (uint8_t *) input_mem_registers;
        memcpy(&current_theta, &input_mem_bytes[1], sizeof(float));

        auto curr = std::chrono::steady_clock::now();
        auto elapsed_time = std::chrono::duration_cast<
            std::chrono::milliseconds>(curr - begin).count();
        logFile << elapsed_time << "," << current_theta << std::endl;
        logFile.flush();

        std::cout << "Current Theta: " << current_theta << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    }


    
    close:
    modbus_close(ctx);
    modbus_free(ctx);
    logFile.close();

    std::cout << "Stopped HMI ..." << std::endl;

    return 0;
}
