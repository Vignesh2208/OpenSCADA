#include <sys/types.h>

#include "src/pc_emulator/include/kronos_api.h"

string GetNxtCommand(int assignedTracerID) {
    int ret = writeTracerResults(assignedTracerID, NULL, 0);
    if (ret < 0) return "STOP";
    return "CONT";
}
