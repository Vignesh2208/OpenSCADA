#include <iostream>
#include <fstream>
#include <string>
#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>

#include "src/pc_emulator/grpc_ext/include/access_service_impl.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerReader;
using grpc::ServerWriter;

using namespace std;
using namespace pc_emulator;
using namespace pc_specification;
using namespace mem_access;

void RunServer(string PLCSpecificationsDir) {
      string server_address("0.0.0.0:50051");
      AccessServiceImpl service(PLCSpecificationsDir);
      ServerBuilder builder;
      builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
      builder.RegisterService(&service);
      std::unique_ptr<Server> server(builder.BuildAndStart());

      //interval_map_example();

      cout << "GRPC Server listening on " << server_address << endl;
      server->Wait();
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cout << "Not enough arguments!. Usage ./grpc_server_main <path_to_plc_spec_directory>"
            << std::endl;
        return 0;
    }
    RunServer(argv[1]);
    return 0;
}