#include "src/pc_emulator/include/sfc/atan.h"
#include "src/pc_emulator/include/utils.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

void ATAN::Execute(PCVariable * __CurrentResult,
    std::vector<PCVariable*>& Operands) {
    auto configuration = __AssociatedResource->__configuration;
    auto CR = __CurrentResult;
    if (!Utils::IsRealType(CR->__VariableDataType)) {
        configuration->PCLogger->RaiseException("ATAN SFC error: CR is not "
            " a real number");
    }

    if (CR->__VariableDataType->__DataTypeCategory == DataTypeCategory::REAL) {
        float RealValue = CR->GetValueStoredAtField<float>("",
                DataTypeCategory::REAL);
        RealValue = atan(RealValue);
        CR->SetField("", &RealValue, sizeof(RealValue));
    } else {
        double LRealValue = CR->GetValueStoredAtField<double>("",
                DataTypeCategory::LREAL);
        LRealValue = atan(LRealValue);
        CR->SetField("", &LRealValue, sizeof(LRealValue));
    }
}