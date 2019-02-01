#ifndef __PC_EMULATOR_INCLUDE_PC_CODE_CONTAINER_H__
#define __PC_EMULATOR_INCLUDE_PC_CODE_CONTAINER_H__

using namespace std;

#include <iostream>
#include <vector>
#include <climits>
#include <cstdlib>
#include <unordered_map>
#include "pc_datatype.h"

using namespace std;

namespace pc_emulator {
    class PCConfiguration;
    class PCResource;

    class InsnContainer {
        public:
            string InsnName;
            string InsnLabel;
            std::vector<string> OperandList;
            int InsnPosition;
        
            InsnContainer(string InsnName, string InsnLabel, int InsnPosition) :
                InsnName(InsnName), InsnLabel(InsnLabel),
                InsnPosition(InsnPosition) {};
            void AddOperand(string Operand) {
                OperandList.push_back(Operand);
            }
    };

    class PoUCodeContainer {
        public :
            PCConfiguration * __configuration;
            PCDataType * __PoUDataType;
            PCResource * __Resource;
            bool __Initialized;
            int __InsnCount;

            std::vector<InsnContainer *> __Insns;
            std::unordered_map<string, InsnContainer*> __InsnContainerByLabel;

            PoUCodeContainer(PCConfiguration* configuration, 
                            PCResource * Resource):
                __configuration(configuration), __Resource(Resource),
                __PoUDataType(nullptr), __Initialized(false),
                __InsnCount(0) {};

            void SetPoUDataType(PCDataType* PoUDataType) {
                __PoUDataType = PoUDataType;
                __Initialized = true;
            }
            PoUCodeContainer& AddInstruction(string InsnString);

            int GetTotalNumberInsns();
            InsnContainer * GetInsn(int pos);
            InsnContainer * GetInsnAT(string label);
            void Cleanup();
            
    };
}

#endif