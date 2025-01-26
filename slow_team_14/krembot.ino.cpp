#include "krembot.ino.h"
#include <iostream>
#include <string>
#include <cmath>

using namespace std;

// Calculate the distance between current position and the base position
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
    state = State::search_food; // Start from looking for food
    sandTimer.start(2000);
}

void slow_team_14_controller::loop()
{
    krembot.loop();

    // Update position and degree
    pos = posMsg.pos;
    deg = posMsg.degreeX;

    // Update distannce from bases
    float base1_distance = calculateDistance(pos, base1_pos);
    float base2_distance = calculateDistance(pos, base2_pos);
    target_pos = base1_distance < base2_distance ? base1_pos : base2_pos;

    // Update distannce from obstacles
    float distanceFront = krembot.RgbaFront.readRGBA().Distance;
    float distanceFrontRight = krembot.RgbaFrontRight.readRGBA().Distance;
    float distanceFrontLeft = krembot.RgbaFrontLeft.readRGBA().Distance;

    switch (state)
    {
    case State::move_to_target:
    {
        // If target found go wander
        if (fabs(pos.GetX() - target_pos.GetX()) < 0.1 && fabs(pos.GetY() - target_pos.GetY()) < 0.1)
        {
            state = State::wander;
            break;
        }

        // If food lost go wander
        if (!hasFood)
        {
            state = State::search_food;
            break;
        }

        // If robot detected try bump it
        if (krembot.RgbaFront.readRGBA().Red > 10 || krembot.RgbaFront.readRGBA().Green > 10)
        {
            state = State::try_bump;
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
        // If food found go to move_to_target
        if (hasFood)
        {
            state = State::move_to_target;
            break;
        }

        // if food is detected go grab it
        if (krembot.RgbaFront.readRGBA().Blue > 10 || krembot.RgbaFrontRight.readRGBA().Blue > 10 || krembot.RgbaFrontLeft.readRGBA().Blue > 10 || krembot.RgbaRight.readRGBA().Blue > 10 || krembot.RgbaLeft.readRGBA().Blue > 10)
        {
            state = State::search_food;
            sandTimer.start(2000);
            break;
        }

        // If the robot encounters a wall from right side, go to turn state with direction left
        if (distanceFront < 15 || distanceFrontRight < 15)
        {
            sandTimer.start(200);
            state = State::turn;
            dir = 1;
            break;
        }

        // If the robot encounters a wall from left side, go to turn state with direction right
        else if (distanceFrontLeft < 15)
        {
            sandTimer.start(200);
            state = State::turn;
            dir = -1;
            break;
        }

        // Move forward
        drive(100, 0);
        break;
    }

    case State::search_food:
    {
        // Move to target if food found
        if (hasFood)
        {
            state = State::move_to_target;
            break;
        }

        // If the robot detected food go grab it
        if (krembot.RgbaFront.readRGBA().Blue > 10)
        {
            drive(100, 0);
        }

        // not detected food turn around until time is over
        else
        {
            // Finish turning
            if (sandTimer.finished())
            {
                state = State::wander;
            }
            // Keep turning
            else
            {
                drive(0, 100);
            }
        }
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

    // Try to bump into fast team robot while moving to target
    case State::try_bump:
    {
        // If robot detected try to bump it
        if (krembot.RgbaFront.readRGBA().Red > 10 || krembot.RgbaFront.readRGBA().Green > 10)
        {
            drive(100, 0);
        }
        // not detected, go to target
        else
        {
            state = State::move_to_target;
        }
        break;
    }
    }
}
