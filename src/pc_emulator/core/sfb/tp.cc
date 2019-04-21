#include "src/pc_emulator/include/sfb/tp.h"
#include "src/pc_emulator/include/utils.h"
#include "src/pc_emulator/include/pc_clock.h"


using namespace std;
using namespace pc_emulator;
using namespace pc_specification;

void TP::Execute(PCVariable * __CurrentResult, PCVariable * SFB) {
    assert(SFB != nullptr && SFB->__VariableDataType->__DataTypeName
        == __SFBName);

    bool IN = SFB->GetValueStoredAtField<bool>("IN", DataTypeCategory::BOOL);
    bool IN_PREV = SFB->GetValueStoredAtField<bool>("IN_PREV",
                    DataTypeCategory::BOOL);
    bool TO_STOP = SFB->GetValueStoredAtField<bool>("TO_STOP",
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
    if (IN && !IN_PREV && !TIMER_STARTED) {
        StartTime = CurrentTime;
        SFB->SetField("IN_PREV", "TRUE");
        SFB->SetField("TIMER_STARTED", "TRUE");
        SFB->SetField("START_TIME", "t#" + std::to_string(StartTime.SecsElapsed)
                     + "s");
        SFB->SetField("Q", "TRUE");
        SFB->SetField("ET", "t#0s");
        SFB->SetField("TO_STOP", "FALSE");
        return;
    } 

    if (!IN && IN_PREV) {
        TO_STOP = true;
        SFB->SetField("TO_STOP", "TRUE");
    }
    
    if (TIMER_STARTED) {
        ElapsedTime = CurrentTime - StartTime;
        if (ElapsedTime < PT) {
            SFB->SetField("ET", "t#" + std::to_string(ElapsedTime.SecsElapsed) 
                        + "s");
            SFB->SetField("Q", "TRUE");
        } else {
            SFB->SetField("Q", "FALSE");

            if (!TO_STOP)
                SFB->SetField("ET", "t#" + std::to_string(PT.SecsElapsed) 
                        + "s");
            else {
                SFB->SetField("TIMER_STARTED", "FALSE");
                SFB->SetField("TO_STOP", "TRUE");
                SFB->SetField("ET", "t#0s");
            }
        }
    }



    if (IN)
        SFB->SetField("IN_PREV", "TRUE");
    else
        SFB->SetField("IN_PREV", "FALSE");
}