#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/include/insns/shl_insn.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

/*
 * Sets the Current result accumulator to the passed operand.
 */
void SHL_Insn::Execute(std::vector<PCVariable*>& Operands, bool isNegated) {
    auto configuration = __AssociatedResource->__configuration;
    auto CR = __AssociatedResource->__CurrentResult;

    uint8_t ByteValue;
    uint16_t WordValue;
    uint32_t DWordValue;
    uint64_t LWordValue;
    int16_t shift_amt;

    
    if (!Utils::IsBitType(CR->__VariableDataType)) {
        configuration->PCLogger->RaiseException("SHL insn error: "
            "CR is not a Bit type");
    }

    if (Operands.size() != 1) {
        configuration->PCLogger->RaiseException("SHL insn error: "
            "Exactly one operand expected!");
    }
    PCVariable * Operand;
    Operand = Operands[0];
    if (Operands[0]->__IsVariableContentTypeAPtr) {
        Operand = Operand->GetPtrStoredAtField("");
        assert(Operand != nullptr);
    }

    if (Operand->__VariableDataType->__DataTypeCategory <
            DataTypeCategory::INT ||
        Operand->__VariableDataType->__DataTypeCategory > 
            DataTypeCategory::ULINT) {
        configuration->PCLogger->RaiseException("SHL insn error: "
            "Operand Type should be INT type");
    }

  

    switch(Operand->__VariableDataType->__DataTypeCategory) {
        case DataTypeCategory::INT:

            shift_amt = Operand->GetValueStoredAtField<int16_t>("",
                        DataTypeCategory::INT);
            break;
        case DataTypeCategory::SINT:

            shift_amt = Operand->GetValueStoredAtField<int8_t>("",
                        DataTypeCategory::SINT);
            break;
        case DataTypeCategory::DINT:

            shift_amt = Operand->GetValueStoredAtField<int32_t>("",
                        DataTypeCategory::DINT);
            break;
        case DataTypeCategory::LINT:

            shift_amt = Operand->GetValueStoredAtField<int64_t>("",
                        DataTypeCategory::LINT);
            break;
        case DataTypeCategory::UINT:

            shift_amt = Operand->GetValueStoredAtField<uint16_t>("",
                        DataTypeCategory::UINT);
            break;
        case DataTypeCategory::USINT:

            shift_amt = Operand->GetValueStoredAtField<uint8_t>("",
                        DataTypeCategory::USINT);
            break;
        case DataTypeCategory::UDINT:

            shift_amt = Operand->GetValueStoredAtField<uint32_t>("",
                        DataTypeCategory::UDINT);
            break;
        case DataTypeCategory::ULINT:

            shift_amt = Operand->GetValueStoredAtField<uint64_t>("",
                        DataTypeCategory::ULINT);
            break;
    }
    
    if (shift_amt < 0) {
        configuration->PCLogger->RaiseException("SHL insn error: "
            "Cannot shift by negative bits!");
    }

    switch(CR->__VariableDataType->__DataTypeCategory) {
        case DataTypeCategory::BYTE:
            ByteValue = CR->GetValueStoredAtField<uint8_t>("",
                    DataTypeCategory::BYTE);
            ByteValue = ByteValue << shift_amt;
            CR->SetField("", &ByteValue, sizeof(ByteValue));
            break;
        case DataTypeCategory::WORD:
            WordValue = CR->GetValueStoredAtField<uint16_t>("",
                    DataTypeCategory::WORD);
            WordValue = WordValue << shift_amt;
            CR->SetField("", &WordValue, sizeof(WordValue));
            break;
        case DataTypeCategory::DWORD:
            DWordValue = CR->GetValueStoredAtField<uint32_t>("",
                    DataTypeCategory::DWORD);
            DWordValue = DWordValue << shift_amt;
            CR->SetField("", &DWordValue, sizeof(DWordValue));
            break;
        case DataTypeCategory::LWORD:
            LWordValue = CR->GetValueStoredAtField<uint64_t>("",
                    DataTypeCategory::LWORD);
            LWordValue = LWordValue << shift_amt;
            CR->SetField("", &LWordValue, sizeof(LWordValue));
            break;
    }

}