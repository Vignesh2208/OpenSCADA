#ifndef __PC_EMULATOR_INCLUDE_KRONOS_API_H__
#define __PC_EMULATOR_INCLUDE_KRONOS_API_H__

#include <iostream>
#include <cstdint>
#include <cstring>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>

extern "C"
{
#include <Kronos_functions.h>  
#include <kronos_utility_functions.h>   
}


using namespace std;
typedef unsigned long u32;

#define TRACER_RESULTS 'J'

//! Synchronization Functions 
string GetNxtCommand(int assignedTracerID);



#endif
