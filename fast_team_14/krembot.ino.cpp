#include "krembot.ino.h"



double threshold = 0.35, angle_threshold = 5;
int count = 0, time_before_direction = 10000;

void fast_team_14_controller::setup() {
    krembot.setup();
    writeTeamColor();
    base_pos = foragingMsg.base1;
    teamName = "fast_team_14_controller";
    state = State::go_to_field;
    direction = rand()%4;
    target_field_deg =CDegrees(90*direction);
    sandTimer_calc.start(time_before_direction);
    
}

void fast_team_14_controller::loop() {
    krembot.loop();
    pos = posMsg.pos;
    deg = posMsg.degreeX;
    //printf("pos = (%f,%f)\n", pos.GetX(), pos.GetY());
    if(hasFood){
        start = true;
        calc_field = true;
        state = State::go_to_base;
    }
    switch (state) {
        case State::try_to_hit: {
            printf("trying to hit\n");
            if (krembot.RgbaFront.readRGBA().Blue > 10){
                drive(100, 0);
            }
            else{
                state = State::search_food;
            }
            break;
        }
        case State::go_to_target_field_deg: {
            if(sandTimer_calc.finished()){
                direction = rand()%4;
                target_field_deg =CDegrees(90*direction);
                sandTimer_calc.start(time_before_direction);
            }

            angleDiff = (deg - target_field_deg).UnsignedNormalize().GetValue();
            if ((angleDiff > angle_threshold) && angleDiff < (360- angle_threshold)){
                drive(0, 50);
            }
            else{
                sandTimer_go.start(2000);
                state = State::go_to_field;
            }   
            break;
        }
        case State::go_to_field: {
            if(calc_field){
                angleDiff = (deg - target_field_deg).UnsignedNormalize().GetValue();
                state = State::go_to_target_field_deg;
                calc_field = false;
                break;
            }
            if(sandTimer_go.finished()){
                calc_field = true;
                state = State::search_food;
            }
            else{
                drive(200,0);
            }
            break;
        }
        case State::search_food: {
            // printf("searching food\n");
         if(hasFood){
            state = State::go_to_base;
            break;   
         }
         if (krembot.RgbaFront.readRGBA().Blue > 5 && krembot.RgbaFront.readRGBA().Green > 5){
            printf("here1\n");
            drive(100,0);
            break;
         }

        // if (krembot.RgbaFront.readRGBA().Blue > 10){
        //     printf("here2\n");
        //     state = State::try_to_hit;
        //     break;
        // }
        else{
            angleDiff = (deg - start_circle_deg).UnsignedNormalize().GetValue();
            if (sandTimer_turn.finished() && (angleDiff < angle_threshold) || angleDiff > (360- angle_threshold)){
                //finished 360 without finding
                // printf("tired of searching\n");
                state = State::go_to_field;
                break;
            }
            else{
                drive(0, 50);
            }
        }
            break;
        }


        case State::go_to_base: {
            if (!hasFood){
                //lost food search it
                start_circle_deg = deg;
                sandTimer_turn.start(2000);
                state = State::search_food;
                break;
            }
            //go_to_base
            if(start){
                if(pos.GetX() == base_pos.GetX()){
                    target_deg = CDegrees(180);
                }
                else{
                    angle = atan((pos.GetY() - base_pos.GetY())/(pos.GetX() - base_pos.GetX()));
                    angle = angle  * (180.0 / M_PI);
                    // il be honset need to understand why this work, but it is
                    if (((pos.GetX() - base_pos.GetX())) > 0){
                        angle += 180;
                    }
                    target_deg = CDegrees(angle);
                }
                start = false;
            }

            angleDiff = (deg - target_deg).UnsignedNormalize().GetValue();
            if ((angleDiff > angle_threshold) && angleDiff < (360- angle_threshold)){
                drive(0, 20);
            }
            else{
                float distance = krembot.RgbaFront.readRGBA().Distance;
                if(distance > threshold){
                    drive(100, 0);
                }
                else{
                    drive(50,90);
                    start = true;
                }
                
            }
            break;
        }
    }
}

