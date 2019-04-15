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
    class PCConfigurationImpl;
    class PCResourceImpl;

    //! Class which holds one instruction of a POU's code body
    class InsnContainer {
        public:
            string InsnName; /*!< Instruction opcode name */
            string InsnLabel;   /*!< Label associated with this line of code */
            std::vector<string> OperandList;    /*!< List of operands */
            int InsnPosition;   /*!< Position of this line of code among all lines */
        
            //!Constructor
            InsnContainer(string InsnName, string InsnLabel, int InsnPosition) :
                InsnName(InsnName), InsnLabel(InsnLabel),
                InsnPosition(InsnPosition) {};

            //! Adds a new operand to the operand list
            void AddOperand(string Operand) {
                OperandList.push_back(Operand);
            }
    };

    //! Class which holds all instructions of a POU's code body
    class PoUCodeContainer {
        public :
            PCConfigurationImpl * __configuration; /*!< Associated configuraiton */
            PCDataType * __PoUDataType; /*!< DataType definition for the POU */
            PCResourceImpl * __Resource;    /*!< Associated Resource */
            bool __Initialized; /*!< Set to true after PoUDataType is set */
            int __InsnCount;    /*!< Number of instructions to hold */

            std::vector<std::unique_ptr<InsnContainer>> 
                __Insns; /*!< Set of all instructions in this POU */
            std::unordered_map<string, InsnContainer*> 
                __InsnContainerByLabel; /*!< Instructions stored by label */

            //! Constructor            
            PoUCodeContainer(PCConfigurationImpl * configuration, 
                            PCResourceImpl * Resource):
                __configuration(configuration), __Resource(Resource),
                __PoUDataType(nullptr), __Initialized(false),
                __InsnCount(0) {};

            //!Initializes Code Container object
            void SetPoUDataType(PCDataType* PoUDataType) {
                __PoUDataType = PoUDataType;
                __Initialized = true;
            }

            //! Adds a new line of code which is resolved to an Insn Obj
            PoUCodeContainer& AddInstruction(string InsnString);

            //! Returns total number of instructions in this POU
            int GetTotalNumberInsns();

            //! Returns instruction at a specific position
            InsnContainer * GetInsn(int pos);

            //! Returns instruction by label
            InsnContainer * GetInsnAT(string label);

            //! Some clean up book keeping operations
            void Cleanup();
            
    };
}

#endif