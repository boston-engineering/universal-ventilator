#include "layouts.h"

lv_obj_t* active_floating_window = nullptr;

static lv_obj_t* add_config_button(const char* title)
{
    lv_obj_t* button = lv_btn_create(SCR_C(CONTROL_AREA_1));
    lv_obj_set_width(button, LV_PCT(100));
    lv_obj_add_style(button, STYLE_PTR_CM(OPTION_BUTTON), LV_PART_MAIN);
    lv_obj_set_style_pad_top(button, 4 px, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(button, 4 px, LV_PART_MAIN);

    lv_obj_t* label = lv_label_create(button);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_border_width(label, 1, LV_PART_MAIN);
    lv_label_set_text_fmt(label, title);
    lv_obj_set_width(label, LV_PCT(100));
    lv_obj_add_style(label, STYLE_PTR_CM(OPTION_BUTTON_TEXT), LV_PART_MAIN);
    lv_obj_align_to(label, button, LV_ALIGN_CENTER, 0, 0);
    lv_obj_center(label);

    return button;
}

static void add_pressure_select_button()
{

    lv_obj_t* button = add_config_button("Select Diff. Pressure Sensor");
    auto on_press = [](lv_event_t* evt) {
        Serial.println("Pressed Diff. Sensor Button");
    };

    lv_obj_add_event_cb(button, on_press, LV_EVENT_PRESSED, nullptr);
}

static void add_stepper_home_button()
{

    lv_obj_t* button = add_config_button("Stepper Motor Homing Sequence");
    auto on_press = [](lv_event_t* evt) {
        Serial.println("Pressed Home Stepper Button");
        // TODO disable start button, settings buttons, etc until home is complete
        // TODO add confirm dialog
    };

    lv_obj_add_event_cb(button, on_press, LV_EVENT_PRESSED, nullptr);
}

void setup_config_window()
{
    lv_obj_t* parent = SCR_C(CONTROL_AREA_1);
    lv_obj_clean(parent);
    lv_obj_set_style_pad_all(parent, 10 px, LV_PART_MAIN);
    lv_obj_set_style_pad_row(parent, 10 px, LV_PART_MAIN);
    lv_obj_refresh_style(parent, LV_PART_MAIN, LV_STYLE_PROP_ANY);

    add_pressure_select_button();
    add_stepper_home_button();
}
