#include <iostream>
#include <cstdint>
#include <cstring>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>

#include "src/pc_emulator/include/pc_configuration.h"
#include "src/pc_emulator/include/pc_resource.h"
#include "src/pc_emulator/include/pc_pou_code_container.h"

using namespace std;
using namespace pc_emulator;

PoUCodeContainer& PoUCodeContainer::AddInstruction(string InsnString) {
    std::vector<string> results;
    boost::trim_if(InsnString, boost::is_any_of(" "));
    boost::split(results, InsnString,
                boost::is_any_of(" "), boost::token_compress_on);
    string InsnLabel;
    string InsnName;
    size_t StartIdx = 1;

    /*if (results.size() < 2) {
        __configuration->PCLogger->RaiseException("Incorrectly formatted "
            "instruction: " + InsnString);
    }*/

    if (boost::ends_with(results[0],":")) { //first element is label
        InsnLabel = results[0].substr(0, results[0].length() - 1);
        InsnName = results[1];
    } else {
        InsnLabel = "";
        InsnName = results[0];
    }

    auto container = std::unique_ptr<InsnContainer>(new InsnContainer(
                                            InsnName, InsnLabel, __InsnCount));
    __InsnCount ++;

    if(!InsnLabel.empty()) {

        if(__InsnContainerByLabel.find(InsnLabel) 
                    != __InsnContainerByLabel.end()) {
            __configuration->PCLogger->RaiseException("Same label cannot be "
                    "assigned more than once!");
        }
        __InsnContainerByLabel.insert(std::make_pair(InsnLabel, 
                                                        container.get()));
        StartIdx ++;
    }

    if (InsnName != "CAL" && InsnName != "CALC" && InsnName != "CALCN") {
        for(;StartIdx < results.size(); StartIdx ++) {
            string Op = results[StartIdx];

            if (Op.empty())
                continue;
            if (boost::ends_with(results[StartIdx], ",")) 
                Op = results[StartIdx].substr(0, results[StartIdx].length()-1);
            container->AddOperand(Op);
        }
    } else {
        string Op;
        if (!InsnLabel.empty())
            Op = InsnString.substr(results[0].length() 
                    + results[1].length() + 2);
        else
            Op = InsnString.substr(results[0].length() + 1);
        
        auto pos = Op.find("(");
        assert(pos != string::npos); // no arguments
        string PoUName = Op.substr(0, pos);
        boost::trim_if(PoUName, boost::is_any_of("\t ,()"));
        container->AddOperand(PoUName);
        container->AddOperand(Op.substr(pos, string::npos));
        
    }

    __Insns.push_back(std::move(container));
    return *this;
}

void PoUCodeContainer::Cleanup() {
    __Insns.clear();
}

int PoUCodeContainer::GetTotalNumberInsns() {
    return __InsnCount;
}

InsnContainer * PoUCodeContainer::GetInsn(int pos) {
    if (pos < __InsnCount && pos >= 0) {
        return __Insns[pos].get();
    }
    return nullptr;
}

InsnContainer * PoUCodeContainer::GetInsnAT(string label) {
    auto got = __InsnContainerByLabel.find(label);
    if (got == __InsnContainerByLabel.end())
        return nullptr;
    return got->second;
}