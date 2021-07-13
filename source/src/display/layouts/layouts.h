#ifndef UVENT_LAYOUTS_H
#define UVENT_LAYOUTS_H

#include "../config/uvent_conf.h"
#include <controls/interface/interface.h>

typedef void (* LabelConfigCb)(lv_obj_t* label);

lv_obj_t* open_option_dialog(const char* title, bool enable_close_button = false);

/*****************************************************/

// Main Screen
void setup_readouts();
void setup_controls();
void setup_buttons();

// Main screen readout functions
void setup_adjustable_readout(AdjValueType type, const char* override_str = nullptr);
void setup_adjustable_control(AdjValueType type);
// I:E Ratio is special
void setup_ie_readout();
void setup_ie_controls();
// Button functions
void add_start_button();
void add_mute_button();
lv_obj_t* add_settings_button(const char* title, lv_obj_t* parent = SCR_C(CONTROL_AREA_2));
lv_obj_t* add_settings_toggle_button(const char* title, lv_obj_t* parent = SCR_C(CONTROL_AREA_2));

/*****************************************************/

extern lv_obj_t* active_floating_window;

// Config Screen
void setup_config_window();

// Config Windows
void config_window_diff_pressure_select();
void config_window_home_stepper();

#endif //UVENT_LAYOUTS_H
