#include "krembot.ino.h"
#include <iostream>
#include <string>

using namespace std;

float calculateDistance(CVector2 current_pos, CVector2 target_pos)
{
    float dx = target_pos.GetX() - current_pos.GetX();
    float dy = target_pos.GetY() - current_pos.GetY();
    return sqrt(dx * dx + dy * dy);
}

void slow_team_14_controller::setup()
{
    krembot.setup();
    writeTeamColor();
    teamName = "slow_team_14_controller";
    base1_pos = foragingMsg.base1;
    base2_pos = foragingMsg.base2;
    state = State::wander;
}

void slow_team_14_controller::loop()
{
    krembot.loop();
    pos = posMsg.pos;
    deg = posMsg.degreeX;
    float base1_distance = calculateDistance(pos, base1_pos);
    float base2_distance = calculateDistance(pos, base2_pos);
    target_pos = base1_distance < base2_distance ? base1_pos : base2_pos;
    float distanceFront = krembot.RgbaFront.readRGBA().Distance;
    float distanceFrontRight = krembot.RgbaFrontRight.readRGBA().Distance;
    float distanceFrontLeft = krembot.RgbaFrontLeft.readRGBA().Distance;
    Colors color = krembot.RgbaFront.readColor();

    switch (state)
    {
    case State::move_to_target:
    {
        // If target found go wander
        if (fabs(pos.GetX() - target_pos.GetX()) < 0.1 && fabs(pos.GetY() - target_pos.GetY()) < 0.1)
        {
            cout << "Success!" << endl;
            state = State::wander;
            break;
        }

        // If food lost go wander
        if (!hasFood)
        {
            state = State::wander;
            break;
        }

        // Calculate angle to target in degrees
        CDegrees angleToTarget = ToDegrees(ATan2(target_pos.GetY() - posMsg.pos.GetY(),
                                                 target_pos.GetX() - posMsg.pos.GetX()));

        // Calculate the angle difference
        CDegrees angleDiff = angleToTarget - posMsg.degreeX;

        // Go to target
        drive(100, angleDiff.GetValue());
        break;
    }

    case State::wander:
    {
        // If food found go to target
        if (hasFood)
        {
            state = State::move_to_target;
            break;
        }

        // If the robot encounters a wall, move to turn state
        if (distanceFront < 15 || distanceFrontRight < 15)
        {
            sandTimer.start(200);
            state = State::turn;
            dir = 1;
            break;
        }

        else if (distanceFrontLeft < 15)
        {
            sandTimer.start(200);
            // If the robot encounters a wall, move to turn state
            state = State::turn;
            dir = -1;
            break;
        }

        drive(100, 0);
        break;
    }

    // Turning state
    case State::turn:
    {
        if (sandTimer.finished())
            state = State::wander;
        else
            drive(100, 90 * dir);
        break;
    }
    }
}
