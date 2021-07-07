#include <display/layouts/layouts.h>
#include "screen.h"

MainScreen::MainScreen()
        : Screen()
{

}

void MainScreen::init()
{
    Screen::init();
}

void MainScreen::setup()
{
    Screen::setup();

    setup_readouts();
    setup_controls();

    // FULL SCREEN
    add_dividers();

    // VISUAL_AREA_2
    add_chart();

    setup_buttons();
    attach_settings_cb();

}

void MainScreen::attach_settings_cb()
{
    // Add callback to settings button to setup the screen
    lv_obj_t* settings_button_container = SCR_C(CONTROL_AREA_2);
    lv_obj_t* settings_button = lv_obj_get_child(settings_button_container, 0);

    if (settings_button) {
        auto on_settings_check = [](lv_event_t* evt) {
            lv_obj_t* target = lv_event_get_target(evt);
            lv_state_t state = lv_obj_get_state(target);
            if (!lv_obj_has_flag(target, LV_OBJ_FLAG_CHECKABLE)) {
                return;
            }

            // TODO check to make sure the ventilator is stopped before opening the settings menu
            // If started, display the confirm dialog for turning off the ventilator

            auto* screen_ptr = (MainScreen*) evt->user_data;

            if ((state & LV_STATE_CHECKED) != 0) {
                Serial.println("Is Checked");
                screen_ptr->open_config();
            }
            else {
                Serial.println("Not Checked");
                setup_controls();
            }
        };

        lv_obj_add_event_cb(settings_button, on_settings_check, LV_EVENT_VALUE_CHANGED, this);
    }
}

void MainScreen::open_config()
{
    setup_config_window();
}


