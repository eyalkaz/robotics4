#include <Krembot/controller/krembot_controller.h>
#include "controllers/foraging/krembot.ino.h"

enum State{
    go_to_field,
    go_to_base,
    try_to_hit,
    search_food,
    go_to_target_field_deg,
    turn_search
};

class fast_team_14_controller : public foraging_controller {
private:
public:
    void setup() override;
    void loop() override;
    bool start = true, calc_field = true;
    State state;
    CVector2 base_pos, pos;
    CDegrees deg, target_deg, start_circle_deg, target_field_deg;
    SandTimer sandTimer_turn, sandTimer_go;
    int direction = 0;
    double angleDiff, angle;
};


REGISTER_CONTROLLER(fast_team_14_controller, "fast_team_14_controller")
