#include <Krembot/controller/krembot_controller.h>
#include "controllers/foraging/krembot.ino.h"

enum State
{
    wander,
    move_to_target,
    turn,
    try_bump,
    search_food,
};

class slow_team_14_controller : public foraging_controller
{
private:
public:
    void setup() override;
    void loop() override;

    State state;
    SandTimer sandTimer;
    CVector2 pos;
    CVector2 base1_pos;
    CVector2 base2_pos;
    CVector2 target_pos;
    CDegrees deg;
    int dir = 1;
};

REGISTER_CONTROLLER(slow_team_14_controller, "slow_team_14_controller")
