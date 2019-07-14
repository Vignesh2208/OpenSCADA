#include <sys/types.h>

#include "src/pc_emulator/include/kronos_api.h"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

int GetNxtCommand(string& result) {
    char nxt_cmd[MAX_PAYLOAD];
    std::vector<std::string> NestedFields;

    
    for(int i = 0; i < MAX_PAYLOAD; i++)
        nxt_cmd[i] = '\0';

    sprintf(nxt_cmd, "%c,", TRACER_RESULTS);

    if (result != "0") {
        result = "-" + result + ",";
    } else {
        result = "0,";
    }
    memcpy(nxt_cmd + strlen(nxt_cmd), result.c_str(), result.length());

    auto ret = write_tracer_results(nxt_cmd);

    if (ret < 0)
        return ret;

    result = nxt_cmd;

    if (boost::starts_with(result, "STOP")) {
        result = "STOP";
        return 1;
    }

    //std::cout << "Kronos returned: " << result << std::endl;
    std::cout << "####### My Pid = " << gettid() << " Kronos returned: " << result << std::endl;
    boost::trim_if(result, boost::is_any_of("\t .[]"));
    boost::split(NestedFields, result,
                boost::is_any_of(",|"), boost::token_compress_on);
    //assert(NestedFields.size() == 2);
    //result = NestedFields[1];
    result = "10000";
    return 1;
}
