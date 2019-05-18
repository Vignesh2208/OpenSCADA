#ifndef __PC_EMULATOR_GRPC_EXT_ACCESS_SERVICE_IMPL_H__
#define __PC_EMULATOR_GRPC_EXT_ACCESS_SERVICE_IMPL_H__



#include <iostream>
#include <fstream>
#include <string>
#include <numeric>
#include <cstdint>
#include <cstring>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <vector>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <cerrno>
#include <stdio.h>
#include <string>

#include "src/pc_emulator/proto/mem_access.grpc.pb.h"
#include "src/pc_emulator/grpc_ext/include/ext_interface_grpc_api.h"
#include <boost/filesystem.hpp>
 
struct path_leaf_string
{
    std::string operator()(const boost::filesystem::directory_entry& entry) const
    {
        return entry.path().leaf().string();
    }
};
 


using namespace std;
using namespace mem_access;
using grpc::ServerContext;

namespace pc_emulator {
    class AccessServiceImpl final : public AccessService::Service {
        public:
            explicit AccessServiceImpl(string PLCSpecificationsDir) {
                std::vector<string> v;
                read_directory(PLCSpecificationsDir, v);

                for (int i = 0; i < v.size(); i++) {
                    if (boost::algorithm::ends_with(v[i], ".prototxt")
                        && CheckProtoFormat(
                            PLCSpecificationsDir + "/" + v[i])) {
                        std::cout << "Parsing PLC Config File: " << v[i] 
                                  << std::endl;
                        auto PLCConfig = std::unique_ptr<ExtInterfaceAPI>(
                            new ExtInterfaceAPI(
                                PLCSpecificationsDir + "/" + v[i]));
                        string PLCName 
                            = PLCConfig->__ConfigInterface.__ConfigurationName;
                        ExtInterface.insert(std::make_pair(PLCName,
                            std::move(PLCConfig)));   
                    }
                }
            }

            void read_directory(const std::string& name,
                    std::vector<string>& v);

            bool CheckProtoFormat(string ProtoFilePath);

            ~AccessServiceImpl() {};

            grpc::Status SetSensorInput(ServerContext*, 
                const SensorInput*, mem_access::Status* ) override;
            
            grpc::Status GetActuatorOutput(ServerContext*, 
                const ActuatorOutput*, mem_access::Result* ) override;
        

        private:
            unordered_map<string, std::unique_ptr<ExtInterfaceAPI>> ExtInterface;
        
    };

}
#endif