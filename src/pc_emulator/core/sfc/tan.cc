#include "src/pc_emulator/include/sfc/tan.h"
#include "src/pc_emulator/include/utils.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

void TAN::Execute(std::vector<PCVariable*>& Operands) {
    auto configuration = __AssociatedResource->__configuration;
    auto CR = __AssociatedResource->__CurrentResult;
    if (!Utils::IsRealType(CR->__VariableDataType)) {
        configuration->PCLogger->RaiseException("TAN SFC error: CR is not "
            " a real number");
    }

    if (CR->__VariableDataType->__DataTypeCategory == DataTypeCategory::REAL) {
        float RealValue = CR->GetValueStoredAtField<float>("",
                DataTypeCategory::REAL);
        RealValue = tan(RealValue);
        CR->SetField("", &RealValue, sizeof(RealValue));
    } else {
        double LRealValue = CR->GetValueStoredAtField<double>("",
                DataTypeCategory::LREAL);
        LRealValue = tan(LRealValue);
        CR->SetField("", &LRealValue, sizeof(LRealValue));
    }
}