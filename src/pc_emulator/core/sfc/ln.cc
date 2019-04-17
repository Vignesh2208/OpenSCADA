#include "src/pc_emulator/include/sfc/ln.h"
#include "src/pc_emulator/include/utils.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

void LN::Execute(std::vector<PCVariable*>& Operands) {
    auto configuration = __AssociatedResource->__configuration;
    auto CR = __AssociatedResource->__CurrentResult;
    if (!Utils::IsRealType(CR->__VariableDataType)) {
        configuration->PCLogger->RaiseException("LN SFC error: CR is not "
            " a real number");
    }

    if (CR->__VariableDataType->__DataTypeCategory == DataTypeCategory::REAL) {
        float RealValue = CR->GetValueStoredAtField<float>("",
                DataTypeCategory::REAL);
        if (RealValue < 0)
            configuration->PCLogger->RaiseException("LN exception: negative domain");
        RealValue = log(RealValue);
        CR->SetField("", &RealValue, sizeof(RealValue));
    } else {
        double LRealValue = CR->GetValueStoredAtField<double>("",
                DataTypeCategory::LREAL);
        if (LRealValue < 0)
            configuration->PCLogger->RaiseException("LN exception: negative domain");
        LRealValue = log(LRealValue);
        CR->SetField("", &LRealValue, sizeof(LRealValue));
    }
}