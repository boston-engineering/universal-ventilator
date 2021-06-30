#ifndef UVENT_LAYOUTS_H
#define UVENT_LAYOUTS_H

#include <controls/interface/interface.h>

// Main Screen
void setup_readouts();
void setup_controls();
void setup_buttons();

// Main screen readout functions
void setup_adjustable_readout(AdjValueType type, const char* override_str = nullptr);
void setup_adjustable_control(AdjValueType type);
void set_readout(AdjValueType type, double val);
double get_readout(AdjValueType type);

/*****************************************************/

// Config Screen
void transition_config();
void close_config();

// Config Windows
void config_window_diff_pressure_select();
void config_window_home_stepper();

#endif //UVENT_LAYOUTS_H
