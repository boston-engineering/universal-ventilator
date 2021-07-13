#include "layouts.h"

lv_obj_t* active_floating_window = nullptr;

static void open_stepper_confirm_dialog(lv_event_t* evt);

static lv_obj_t* add_config_button(const char* title)
{
    lv_obj_t* button = lv_btn_create(SCR_C(CONTROL_AREA_1));
    lv_obj_set_width(button, LV_PCT(100));
    lv_obj_add_style(button, STYLE_PTR_CM(OPTION_BUTTON), LV_PART_MAIN);
    lv_obj_set_style_pad_top(button, 4 px, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(button, 4 px, LV_PART_MAIN);

    lv_obj_t* label = lv_label_create(button);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_border_width(label, 0, LV_PART_MAIN);
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

    lv_obj_add_event_cb(button, open_stepper_confirm_dialog, LV_EVENT_RELEASED, nullptr);
}

static void quick_flex_obj(lv_obj_t* obj, lv_flex_flow_t flow = LV_FLEX_FLOW_COLUMN, bool allow_grow = true)
{
    lv_obj_set_style_flex_flow(obj, flow, LV_PART_MAIN);
    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN);
    if (flow == LV_FLEX_FLOW_COLUMN) {
        lv_obj_set_style_pad_row(obj, 4 px, LV_PART_MAIN);
    }
    else {
        lv_obj_set_style_pad_column(obj, 4 px, LV_PART_MAIN);
    }
    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN);
    if (allow_grow) {
        lv_obj_set_style_flex_grow(obj, FLEX_GROW, LV_PART_MAIN);
    }
}

static void default_window_container_styles(lv_obj_t* obj)
{
    lv_obj_set_style_pad_all(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(obj, 0 px, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(obj, 0 px, LV_PART_MAIN);
    lv_obj_set_width(obj, LV_PCT(100));
}

static void open_stepper_confirm_dialog(lv_event_t* evt)
{
    if (active_floating_window) {
        Serial.println("Can't open a second window");
        return;
    }
    Serial.println("Pressed Home Stepper Button");
    // TODO disable start button, settings buttons, etc until home is complete
    lv_obj_t* window = open_option_dialog("Confirm Action", true);
    lv_obj_t* main_area = lv_win_get_content(window);
    lv_obj_set_style_pad_left(main_area, 0 px, LV_PART_MAIN);
    lv_obj_set_style_pad_right(main_area, 0 px, LV_PART_MAIN);
    lv_obj_set_style_pad_top(main_area, 4 px, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(main_area, 4 px, LV_PART_MAIN);
    quick_flex_obj(main_area);

    lv_obj_t* label_container = lv_obj_create(main_area);
    quick_flex_obj(label_container);
    default_window_container_styles(label_container);

    lv_obj_t* warning_label_1 = lv_label_create(label_container);
    lv_obj_set_style_text_align(warning_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(warning_label_1, &lv_font_montserrat_28, LV_PART_MAIN);
    lv_label_set_text(warning_label_1, "WARNING");

    lv_obj_t* warning_label_2 = lv_label_create(label_container);
    lv_label_set_long_mode(warning_label_2, LV_LABEL_LONG_WRAP);
    lv_label_set_text(
            warning_label_2,
            "This operation will completely stop the unit before proceeding.\n"
            "Are you sure you wish to continue?"
    );
    lv_obj_set_style_text_align(warning_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(warning_label_2, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_set_width(warning_label_2, LV_PCT(100));

    lv_obj_t* button_container = lv_obj_create(main_area);
    quick_flex_obj(button_container, LV_FLEX_FLOW_ROW, false);
    default_window_container_styles(button_container);
    lv_obj_set_height(button_container, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_column(button_container, 8 px, LV_PART_MAIN);
    lv_obj_set_style_pad_left(button_container, 4 px, LV_PART_MAIN);
    lv_obj_set_style_pad_right(button_container, 4 px, LV_PART_MAIN);

    lv_obj_t* no_button = lv_btn_create(button_container);
    lv_obj_set_style_bg_color(no_button, LV_COLOR_MAKE(215, 215, 215), LV_PART_MAIN);
    lv_obj_set_width(no_button, LV_SIZE_CONTENT);
    lv_obj_set_style_flex_grow(no_button, FLEX_GROW, LV_PART_MAIN);
    lv_obj_t* no_label = lv_label_create(no_button);
    lv_label_set_text(no_label, "NO");
    lv_obj_add_style(no_label, STYLE_PTR_CM(OPTION_BUTTON_TEXT), LV_PART_MAIN);
    lv_obj_align_to(no_label, no_button, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_center(no_label);

    lv_obj_t* yes_button = lv_btn_create(button_container);
    lv_obj_set_width(yes_button, LV_SIZE_CONTENT);
    lv_obj_set_style_flex_grow(yes_button, FLEX_GROW, LV_PART_MAIN);
    lv_obj_t* yes_label = lv_label_create(yes_button);
    lv_label_set_text(yes_label, "YES");
    lv_obj_set_style_text_align(yes_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(yes_label, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_align_to(yes_label, yes_button, LV_ALIGN_CENTER, 0, 0);
    lv_obj_center(yes_label);
}

lv_obj_t* open_option_dialog(const char* title, bool enable_close_button)
{
    lv_obj_t* window = lv_win_create(lv_scr_act(), 60);
    lv_obj_add_style(window, STYLE_PTR_CM(POPUP_WINDOW), LV_PART_MAIN);

    lv_obj_t* label = lv_win_add_title(window, title);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, LV_PART_MAIN);

    if (enable_close_button) {
        lv_obj_t* close_button = lv_win_add_btn(window, LV_SYMBOL_CLOSE, 40);
        lv_obj_add_event_cb(close_button, [](lv_event_t* evt) {
            active_floating_window = nullptr;

            if (!evt->user_data) {
                return;
            }

            Serial.println("Closing option dialog...");
            auto* win = (lv_obj_t*) lv_event_get_user_data(evt);
            lv_obj_del(win);
        }, LV_EVENT_CLICKED, window);
    }

    lv_obj_align_to(window, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);
    active_floating_window = window;
    return window;
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
