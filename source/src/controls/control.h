#ifndef UVENT_CONTROL_H
#define UVENT_CONTROL_H

#include "controls/machine.h"

void control_init();
void control_service();
double control_get_actuator_position();
double control_get_actuator_position_raw();
void control_zero_actuator_position();
void control_change_state(States);
void control_actuator_manual_move(Tick_Type tt, double angle, double speed);
States control_get_state();
const char* control_get_state_string();
const char** control_get_state_list(uint8_t*);
void control_display_storage();
double control_get_volume(C_Stat compliance = C_Stat::TWENTY);
void control_actuator_set_enable(bool en);
#endif