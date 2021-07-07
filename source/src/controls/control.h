#ifndef UVENT_CONTROL_H
#define UVENT_CONTROL_H

#include "controls/machine.h"
#include "interface/interface.h"

void init_adjustable_values();

void loop_test_readout();

void set_readout(AdjValueType type, double val);
double get_readout(AdjValueType type);

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
double control_get_degrees_to_volume(C_Stat compliance = C_Stat::FIFTY);
double control_calc_volume_to_degrees(C_Stat compliance, double volume);
void control_actuator_set_enable(bool en);
void control_display_storage();
waveform_params* control_get_waveform_params(void);
void control_calculate_waveform();
double control_get_gauge_pressure();
double control_get_diff_pressure();
#endif