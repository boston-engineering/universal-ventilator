#ifndef UVENT_CONTROL_H
#define UVENT_CONTROL_H

#include <display/screens/screen.h>
#include "controls/machine.h"
#include "interface/interface.h"

void init_adjustable_values();

void loop_test_readout(lv_timer_t*);
/**
 * Timer function to poll sensors and update values accordingly.
 * This should not be called manually under normal circumstances
 * @param timer
 */
void loop_update_readouts(lv_timer_t* timer);

double get_control_target(AdjValueType type);
void set_readout(AdjValueType type, double val);
double get_readout(AdjValueType type);

void control_update_waveform_param(AdjValueType type, float new_value);

void control_init();
void control_service();
double control_get_actuator_position();
int8_t control_get_actuator_position_raw(double& angle);
void control_zero_actuator_position();
void control_write_ventilator_params();
void control_change_state(States);
void control_actuator_manual_move(Tick_Type tt, double angle, double speed);
States control_get_state();
const char* control_get_state_string();
const char** control_get_state_list(uint8_t*);
void control_display_storage();
bool control_is_crc_ok();
double control_get_degrees_to_volume(C_Stat compliance = C_Stat::FIFTY);
double control_get_degrees_to_volume_ml(C_Stat compliance = C_Stat::FIFTY);
double control_calc_volume_to_degrees(C_Stat compliance, double volume);
void control_actuator_set_enable(bool en);
waveform_params* control_get_waveform_params(void);
void control_calculate_waveform();
void control_waveform_display_details();
double control_get_gauge_pressure();
double control_get_diff_pressure();
void control_alarm_snooze();
String control_get_alarm_text();
int16_t control_get_alarm_count();
void control_set_alarm_all_off();
#endif