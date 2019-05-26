#ifndef MODBUS_COMM_MODULE_H__
#define MODBUS_COMM_MODULE_H__

#include <iostream>
#include <cstdint>
#include <cstring>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>

extern "C"
{
#include <modbus.h>   
}

#include "src/pc_emulator/ext_modules/comm_module.h"
#include "src/pc_emulator/include/utils.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldInterfaceType = pc_specification::FieldInterfaceType;



#endif