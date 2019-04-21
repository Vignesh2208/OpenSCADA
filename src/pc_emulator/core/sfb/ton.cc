#include "src/pc_emulator/include/sfb/ton.h"
#include "src/pc_emulator/include/utils.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

void TON::Execute(PCVariable * __CurrentResult, PCVariable * SFB) {
    assert(SFB != nullptr && SFB->__VariableDataType->__DataTypeName
        == __SFBName);

    std::cout << "Executing TON\n";
    bool IN = SFB->GetValueStoredAtField<bool>("IN", DataTypeCategory::BOOL);
    bool IN_PREV = SFB->GetValueStoredAtField<bool>("IN_PREV",
                    DataTypeCategory::BOOL);
    bool TIMER_STARTED = SFB->GetValueStoredAtField<bool>("TIMER_STARTED",
                DataTypeCategory::BOOL);
    TimeType PT = SFB->GetValueStoredAtField<TimeType>("PT", 
        DataTypeCategory::TIME);
    TimeType StartTime = SFB->GetValueStoredAtField<TimeType>("START_TIME", 
        DataTypeCategory::TIME);
    TimeType CurrentTime, ElapsedTime;
    
    Clock * clk = __AssociatedResource->clock.get();
    assert(clk != nullptr);
    CurrentTime.SecsElapsed = clk->GetCurrentTime();
    if (IN && !IN_PREV) {
        StartTime = CurrentTime;
        SFB->SetField("IN_PREV", "TRUE");
        SFB->SetField("START_TIME", "t#" + std::to_string(StartTime.SecsElapsed)
                     + "s");
        SFB->SetField("Q", "FALSE");
        SFB->SetField("ET", "t#0s");
        SFB->SetField("TIMER_STARTED", "TRUE");
        std::cout << "Finished TON\n";
        return;
    } 

    if (!IN && IN_PREV) {
        SFB->SetField("IN_PREV", "TRUE");
        SFB->SetField("START_TIME", "t#0s");
        SFB->SetField("Q", "FALSE");
        SFB->SetField("ET", "t#0s");
        SFB->SetField("TIMER_STARTED", "FALSE");
        std::cout << "Finished TON\n";
        return;
    }
    
    
    if (TIMER_STARTED) {
        ElapsedTime = CurrentTime - StartTime;
        if (ElapsedTime < PT) {
            SFB->SetField("ET", "t#" + std::to_string(ElapsedTime.SecsElapsed) 
                        + "s");
            SFB->SetField("Q", "FALSE");
        } else {
            SFB->SetField("Q", "TRUE");
            SFB->SetField("ET", "t#" + std::to_string(PT.SecsElapsed) 
                    + "s");
            
        }
    }

    if (IN)
        SFB->SetField("IN_PREV", "TRUE");
    else
        SFB->SetField("IN_PREV", "FALSE");
    std::cout << "Finished TON\n";
}