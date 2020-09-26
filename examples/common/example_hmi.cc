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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <modbus/modbus.h>   
}


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

bool to_stop = false;

void signal_handler( int signal_num ) { 
    std::cout << "Interrupted! Stopping HMI .... " << std::endl; 
    to_stop = true;
} 


void print_usage() {
    std::cout << "example_hmi -i <ip_address_to_connect> -p <port_nuumber to connect> -l <log-file-path>" << std::endl;
}

using namespace std;

int main(int argc, char **argv) {

    modbus_mapping_t mb_mapping;
    int s, rc, header_length, i;
    uint16_t *input_mem_registers = NULL;
    uint8_t *input_mem_bytes;
    modbus_t *ctx;
    std::ofstream output;
    int redir_fd, stdout_fd;

    std::string logFilePath;
    string ip_address = "127.0.0.1";
    int port_no = 1502;
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf

    int input_mem_size_bytes = 10000;
    InputParser input(argc, argv);

    if(input.cmdOptionExists("-h")){
        print_usage();
        exit(0);
    }

    if (input.cmdOptionExists("-i")) {
        ip_address = input.getCmdOption("-i");
    }

    if (input.cmdOptionExists("-p")) {
        port_no = std::stoi(input.getCmdOption("-p"));
    }

    if (input.cmdOptionExists("-l")) {
        logFilePath = input.getCmdOption("-l");
        output.open(logFilePath, std::ofstream::out | std::ofstream::trunc);
    	std::cout.rdbuf(output.rdbuf()); //redirect std::cout to logFilePath
        stdout_fd = dup(STDOUT_FILENO);
	redir_fd = open(logFilePath.c_str(), O_WRONLY);
	dup2(redir_fd, STDOUT_FILENO);
	close(redir_fd);  // for redirecting Printfs
    }
    

    float current_theta =  0.0;


    input_mem_registers = (uint16_t *) malloc((input_mem_size_bytes /2));
    input_mem_bytes = (uint8_t *) input_mem_registers;
    memset(input_mem_registers, 0, (input_mem_size_bytes /2));

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    ctx = modbus_new_tcp(ip_address.c_str(), port_no);
    //modbus_set_debug(ctx, TRUE);
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
            break;
        }

	input_mem_bytes = (uint8_t *) input_mem_registers;
        memcpy(&current_theta, &input_mem_bytes[1], sizeof(float));

        auto curr = std::chrono::steady_clock::now();
        auto elapsed_time = std::chrono::duration_cast<
            std::chrono::milliseconds>(curr - begin).count();
        std::cout << "Elapsed Time (milli-sec): " << elapsed_time << ", Curr Theta: " << current_theta << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        fflush(stdout);

    }

    modbus_close(ctx);
    modbus_free(ctx);

    std::cout << "Stopped HMI ..." << std::endl;
    std::cout.rdbuf(coutbuf);
    if (logFilePath != "") {
	output.close();
	fflush(stdout);
	dup2(stdout_fd, STDOUT_FILENO);
	close(stdout_fd);
    }

    return 0;
}
