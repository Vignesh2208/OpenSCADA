#include "src/pc_emulator/include/sfc/abs.h"
#include "src/pc_emulator/include/utils.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

void ABS::Execute(std::vector<PCVariable*>& Operands) {
    auto configuration = __AssociatedResource->__configuration;
    auto CR = __AssociatedResource->__CurrentResult;
    if (Utils::IsNumType(CR->__VariableDataType)) {
        int8_t SintValue;
        int16_t IntValue;
        int32_t DIntValue;
        int64_t LIntValue;
        float RealValue;
        double LRealValue;

        switch(CR->__VariableDataType->__DataTypeCategory) {
            case DataTypeCategory::SINT:
                        SintValue = CR->GetValueStoredAtField<int8_t>("",
                            DataTypeCategory::SINT);
                        if (SintValue < 0)
                            SintValue = -1*SintValue;
                        CR->SetField("", &SintValue, sizeof(SintValue));
                        break;
            case DataTypeCategory::INT:
                        IntValue = CR->GetValueStoredAtField<int16_t>("",
                            DataTypeCategory::INT);
                        if (IntValue < 0)
                            IntValue = -1*IntValue;
                        CR->SetField("", &IntValue, sizeof(IntValue));
                        break;
            case DataTypeCategory::DINT:
                        DIntValue = CR->GetValueStoredAtField<int32_t>("",
                            DataTypeCategory::DINT);
                        if (DIntValue < 0)
                            DIntValue = -1*DIntValue;
                        CR->SetField("", &DIntValue, sizeof(DIntValue));
                        break;
            case DataTypeCategory::LINT:
                        LIntValue = CR->GetValueStoredAtField<int64_t>("",
                            DataTypeCategory::LINT);
                        if (LIntValue < 0)
                            LIntValue = -1*LIntValue;
                        CR->SetField("", &LIntValue, sizeof(LIntValue));
                        break;
            case DataTypeCategory::REAL:
                        RealValue = CR->GetValueStoredAtField<float>("",
                            DataTypeCategory::REAL);
                        if (RealValue < 0.0)
                            RealValue = -1.0*RealValue;
                        CR->SetField("", &RealValue, sizeof(RealValue));
                        break;
            case DataTypeCategory::LREAL:
                        LRealValue = CR->GetValueStoredAtField<double>("",
                            DataTypeCategory::LREAL);
                        if (LRealValue < 0.0)
                            LRealValue = -1.0*LRealValue;
                        CR->SetField("", &LRealValue, sizeof(LRealValue));
                        break;
            default:    configuration->PCLogger->LogMessage(LogLevels::LOG_INFO,
                            "ABS has not effect"
                            " for: " + CR->__VariableDataType->__DataTypeName);
                        break;
        }
    } else {
        configuration->PCLogger->RaiseException("ABS SFC error: CR is not "
            " a number");
    }
}