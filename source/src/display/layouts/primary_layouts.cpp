#include <display/main_display.h>
#include <utilities/util.h>
#include <controls/control.h>
#include "layouts.h"

/************************************************/
/*        Static Defs & Function Callbacks      */
/************************************************/
static void on_readout_update(lv_event_t*);
static void update_readout_labels(AdjustableValue* this_ptr, lv_obj_t* spangroup);
static void readout_update_cb(AdjustableValue*, lv_event_t*);

static void on_control_button_press(lv_event_t*);
static void control_press_cb(AdjustableValue*, lv_event_t*);
static void update_control_label(AdjustableValue* this_ptr);
static void update_normal_label(AdjustableValue* this_ptr);
static void update_ie_control_label(AdjustableValue* this_ptr);

static void get_ie_control_label(AdjustableValue* this_ptr, lv_obj_t** obj_ptr);
static void update_ie_control_state(AdjustableValue* this_ptr, lv_obj_t* label);
static void toggle_ie_select();

static void trim_spans_to_size(lv_obj_t* spangroup, size_t final_size, bool refresh = false);

/***************************************************************/
/*      Functions to generate & fill entire screen areas       */
/***************************************************************/

void setup_readouts()
{
    lv_obj_t* parent = SCR_C(VISUAL_AREA_1);
    lv_obj_clean(parent);

    setup_adjustable_readout(TIDAL_VOLUME);
    setup_adjustable_readout(RESPIRATION_RATE);

    setup_ie_readout();

    setup_adjustable_readout(PEEP);
    setup_adjustable_readout(PIP);
    setup_adjustable_readout(CUR_PRESSURE);
}

void setup_controls()
{
    lv_obj_t* parent = SCR_C(CONTROL_AREA_1);
    lv_obj_clean(parent);

    lv_obj_remove_local_style_prop(parent, LV_STYLE_PAD_ROW, LV_PART_MAIN);

    setup_adjustable_control(TIDAL_VOLUME);
    setup_adjustable_control(RESPIRATION_RATE);

    // I:E has some weird behavior since it's technically 2 values
    setup_ie_controls();

    setup_adjustable_control(PEEP);
    setup_adjustable_control(PIP);
    setup_adjustable_control(PLATEAU_TIME);
}

void setup_buttons()
{
    // Wipe areas
    lv_obj_t* visual_area_3 = SCR_C(VISUAL_AREA_3);
    lv_obj_clean(visual_area_3);
    lv_obj_t* control_area_2 = SCR_C(CONTROL_AREA_2);
    lv_obj_clean(control_area_2);

    // VISUAL_AREA_3
    add_start_button();
    add_mute_button();

    // CONTROL_AREA_2
    add_settings_toggle_button("Settings &\nConfig");
}

void setup_visual_2()
{
    // Wipe areas
    lv_obj_t* visual_area_2 = SCR_C(VISUAL_AREA_2);
    lv_obj_clean(visual_area_2);

    lv_obj_t* chart_holder = lv_obj_create(visual_area_2);
    lv_obj_add_style(chart_holder, STYLE_PTR_CM(CHART_HOLDER), LV_PART_MAIN);
}

/*******************************************************************/
/*  Functions to generate components like the readouts & controls  */
/*******************************************************************/

static void configure_label_spangroup(lv_obj_t* obj)
{
    lv_spangroup_set_mode(obj, LV_SPAN_MODE_EXPAND);
    lv_spangroup_set_align(obj, LV_TEXT_ALIGN_RIGHT);
#if DEBUG_BORDER_READOUTS
    lv_obj_set_style_border_width(obj, 1 px, LV_PART_MAIN);
#endif
    lv_obj_add_style(obj, STYLE_PTR_CM(READOUT_VALUE_AMOUNT_TEXT), LV_PART_MAIN);
}

static void border_span(lv_span_t* span)
{
    lv_style_set_border_color(&span->style, lv_color_black());
    lv_style_set_border_width(&span->style, 1 px);
}

void setup_adjustable_readout(AdjValueType type, const char* override_str)
{
    auto value_class = &adjustable_values[type];
    AdjValueParams settings = value_class->get_settings();

    lv_obj_t* parent_container = SCR_C(VISUAL_AREA_1);

    lv_obj_t* obj = lv_obj_create(parent_container);
    lv_obj_add_event_cb(obj, on_readout_update, LV_EVENT_REFRESH, value_class);
    value_class->readout_update_cb = readout_update_cb;
    value_class->set_obj_measured(obj);
    lv_obj_add_style(obj, STYLE_PTR_CM(READOUT), LV_PART_MAIN);
    lv_obj_set_style_bg_color(obj, settings.main_color, LV_PART_MAIN);
    lv_obj_set_width(obj, LV_PCT(100));

    lv_obj_t* name_container = lv_obj_create(obj);
    lv_obj_add_style(name_container, STYLE_PTR_CM(READOUT_NAME_CONTAINER), LV_PART_MAIN);
    lv_obj_set_height(name_container, LV_PCT(100));

    lv_obj_t* value_container = lv_obj_create(obj);
    lv_obj_add_style(value_container, STYLE_PTR_CM(READOUT_VALUE_CONTAINER), LV_PART_MAIN);
    lv_obj_set_height(value_container, LV_PCT(100));
    lv_obj_set_width(value_container, LV_SIZE_CONTENT);

    lv_obj_t* name_label = lv_label_create(name_container);
    lv_obj_set_width(name_label, LV_PCT(100));
    lv_obj_add_style(name_label, STYLE_PTR_CM(READOUT_NAME_TEXT), LV_PART_MAIN);
    lv_label_set_text_fmt(name_label, settings.title);
    //

    lv_obj_t* value_amount_spangroup = lv_spangroup_create(value_container);
    lv_obj_t* value_unit_label;

    configure_label_spangroup(value_amount_spangroup);

    lv_span_t* primary_amount_span = lv_spangroup_new_span(value_amount_spangroup);
    border_span(primary_amount_span);

    if (override_str) {  // If we have a pre-formatted string like I:E, just use that

        lv_span_set_text(primary_amount_span, override_str);
    }
    else {            // Else get the actual thing we need

        double measured = *value_class->get_value_measured();
        const char* formatter = settings.measured_formatter;
        if (measured <= READOUT_VALUE_NONE) { // Use blanks since we haven't measured anything yet
            lv_span_set_text(primary_amount_span, "--");
        }
    }

    lv_spangroup_refr_mode(value_amount_spangroup);
    lv_obj_center(value_amount_spangroup);

    if (settings.unit) {
        value_unit_label = lv_label_create(value_container);
        lv_obj_add_style(value_unit_label, STYLE_PTR_CM(READOUT_VALUE_UNIT_TEXT), LV_PART_MAIN);
        lv_label_set_text_fmt(value_unit_label, settings.unit);
    }

#if DEBUG_BORDER_READOUTS
    lv_obj_set_style_border_width(name_container, 1 px, LV_PART_MAIN);
    lv_obj_set_style_border_width(value_container, 1 px, LV_PART_MAIN);
    lv_obj_set_style_border_width(name_label, 1 px, LV_PART_MAIN);
    if (settings.unit && value_unit_label) {
        lv_obj_set_style_border_width(value_unit_label, 1 px, LV_PART_MAIN);
    }
#endif
}

void setup_adjustable_control(AdjValueType type)
{
    auto value_class = &adjustable_values[type];
    auto settings = value_class->get_settings();

    lv_obj_t* obj = lv_obj_create(SCR_C(CONTROL_AREA_1));
    value_class->control_press_cb = control_press_cb;
    value_class->set_obj_target(obj);
    lv_obj_add_style(obj, STYLE_PTR_CM(CONTROL), LV_PART_MAIN);
    lv_obj_set_width(obj, LV_PCT(100));

    lv_obj_t* reduce_button = lv_btn_create(obj);
    lv_obj_add_event_cb(reduce_button, on_control_button_press, LV_EVENT_ALL, value_class);
    lv_obj_add_style(reduce_button, STYLE_PTR_CM(CONTROL_BUTTON), LV_PART_MAIN);
    lv_obj_set_size(reduce_button, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    reduce_button->user_data = &button_type_data[ButtonType::DECREMENT];

    lv_obj_t* reduce_button_label = lv_label_create(reduce_button);

    lv_obj_add_style(reduce_button_label, STYLE_PTR_CM(CONTROL_BUTTON_TEXT), LV_PART_MAIN);
    lv_label_set_text_fmt(reduce_button_label, "-");
    lv_obj_align(reduce_button_label, LV_ALIGN_CENTER, -1, -1);

    lv_obj_t* text_container = lv_obj_create(obj);
    lv_obj_add_style(text_container, STYLE_PTR_CM(CONTROL_TEXT_CONTAINER), LV_PART_MAIN);
    lv_obj_set_size(text_container, LV_PCT(100), LV_PCT(100));

    lv_obj_t* top_container = lv_obj_create(text_container);
    lv_obj_add_style(top_container, STYLE_PTR_CM(CONTROL_TEXT_CONTAINER_TOP), LV_PART_MAIN);

    lv_obj_t* value_label = lv_label_create(top_container);
    lv_obj_t* unit_label;
    lv_obj_set_width(value_label, LV_SIZE_CONTENT);
    lv_obj_add_style(value_label, STYLE_PTR_CM(CONTROL_TEXT_VALUE), LV_PART_MAIN);
    const char* formatter = settings.target_formatter;
    // Make sure to cast if we're trying for an int
    if (strcmp("%d", formatter) == 0 || strcmp("%i", formatter) == 0) {
        lv_label_set_text_fmt(value_label, formatter, (int32_t) *value_class->get_value_target());
    }
    else {
        lv_label_set_text_fmt(value_label, formatter, *value_class->get_value_target());
    }

    if (settings.unit) {
        unit_label = lv_label_create(top_container);
        lv_obj_set_width(unit_label, LV_SIZE_CONTENT);
        lv_obj_add_style(unit_label, STYLE_PTR_CM(CONTROL_TEXT_VALUE), LV_PART_MAIN);
        lv_obj_set_style_text_font(unit_label, &lv_font_montserrat_14, LV_PART_MAIN);
        lv_label_set_text_fmt(unit_label, "%s", settings.unit);
    }

    lv_obj_t* name_label = lv_label_create(text_container);
    lv_obj_set_width(name_label, LV_PCT(100));
    lv_obj_add_style(name_label, STYLE_PTR_CM(CONTROL_TEXT_NAME), LV_PART_MAIN);
    lv_label_set_text_fmt(name_label, settings.control_title);

    lv_obj_t* increase_button = lv_btn_create(obj);
    lv_obj_add_event_cb(increase_button, on_control_button_press, LV_EVENT_ALL, value_class);
    lv_obj_add_style(increase_button, STYLE_PTR_CM(CONTROL_BUTTON), LV_PART_MAIN);
    lv_obj_set_size(increase_button, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    increase_button->user_data = &button_type_data[ButtonType::INCREMENT];

    lv_obj_t* increase_button_label = lv_label_create(increase_button);
    lv_obj_add_style(increase_button_label, STYLE_PTR_CM(CONTROL_BUTTON_TEXT), LV_PART_MAIN);
    lv_label_set_text_fmt(increase_button_label, "+");
    lv_obj_center(increase_button_label);

#if DEBUG_BORDER_CONTROLS
    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(reduce_button, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(reduce_button_label, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(increase_button, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(increase_button_label, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(text_container, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(top_container, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(value_label, 1, LV_PART_MAIN);
    if (settings.unit && unit_label) {
        lv_obj_set_style_border_width(unit_label, 1, LV_PART_MAIN);
    }
    lv_obj_set_style_border_width(name_label, 1, LV_PART_MAIN);
#endif
}

void setup_ie_readout()
{
    char ie_ratio[12];
    double left = get_readout(AdjValueType::IE_RATIO_LEFT);
    double right = get_readout(AdjValueType::IE_RATIO_RIGHT);
    const char* left_format = is_whole(left) ? "%ld" : "%.1f";
    const char* right_format = is_whole(right) ? "%ld" : "%.1f";
    char buf_left[5];
    char buf_right[5];
    if (left <= READOUT_VALUE_NONE) {
        snprintf(buf_left, 5, "--");
    }
    else {
        snprintf(buf_left, 5, left_format, is_whole(left) ? (int32_t) left : left);
    }

    if (right <= READOUT_VALUE_NONE) {
        snprintf(buf_right, 5, "--");
    }
    else {
        snprintf(buf_right, 5, right_format, is_whole(right) ? (int32_t) right : right);
    }

    snprintf(ie_ratio, 12, "%s : %s", buf_left, buf_right);
    setup_adjustable_readout(IE_RATIO_LEFT, ie_ratio);

    adjustable_values[IE_RATIO_LEFT].readout_update_cb = [](AdjustableValue* this_ptr, lv_event_t* evt) {

    };
}

static void ie_label_text(lv_obj_t* label, double val)
{
    if (is_whole(val)) {
        lv_label_set_text_fmt(label, "%ld", (uint32_t) val);
    }
    else {
        lv_label_set_text_fmt(label, "%.1f", val);
    }
}

void setup_ie_controls()
{
    AdjustableValue* left_class = &adjustable_values[IE_RATIO_LEFT];
    AdjustableValue* right_class = &adjustable_values[IE_RATIO_RIGHT];

    lv_obj_t* obj = lv_obj_create(SCR_C(CONTROL_AREA_1));
    left_class->control_press_cb = control_press_cb;
    right_class->control_press_cb = control_press_cb;
    left_class->set_obj_target(obj);
    right_class->set_obj_target(obj);
    lv_obj_add_style(obj, STYLE_PTR_CM(CONTROL), LV_PART_MAIN);
    lv_obj_set_width(obj, LV_PCT(100));

    lv_obj_t* reduce_button = lv_btn_create(obj);
    lv_obj_add_event_cb(reduce_button, on_control_button_press, LV_EVENT_ALL, left_class);
    lv_obj_add_event_cb(reduce_button, on_control_button_press, LV_EVENT_ALL, right_class);
    lv_obj_add_style(reduce_button, STYLE_PTR_CM(CONTROL_BUTTON), LV_PART_MAIN);
    lv_obj_set_size(reduce_button, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    reduce_button->user_data = &button_type_data[ButtonType::DECREMENT];

    lv_obj_t* reduce_button_label = lv_label_create(reduce_button);

    lv_obj_add_style(reduce_button_label, STYLE_PTR_CM(CONTROL_BUTTON_TEXT), LV_PART_MAIN);
    lv_label_set_text_fmt(reduce_button_label, "-");
    lv_obj_align(reduce_button_label, LV_ALIGN_CENTER, -1, -1);

    lv_obj_t* text_container = lv_obj_create(obj);
    auto change_selected_cb = [](lv_event_t* evt) {
        if (!(evt->code == LV_EVENT_PRESSED || evt->code == LV_EVENT_LONG_PRESSED
                || evt->code == LV_EVENT_LONG_PRESSED_REPEAT)) {
            return;
        }
        toggle_ie_select();
    };
    lv_obj_add_event_cb(text_container, change_selected_cb, LV_EVENT_ALL, nullptr);
    lv_obj_add_style(text_container, STYLE_PTR_CM(CONTROL_TEXT_CONTAINER), LV_PART_MAIN);
    lv_obj_set_size(text_container, LV_PCT(100), LV_PCT(100));

    lv_obj_t* top_container = lv_obj_create(text_container);
    lv_obj_add_flag(top_container, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_style(top_container, STYLE_PTR_CM(CONTROL_TEXT_CONTAINER_IE_TOP), LV_PART_MAIN);

    lv_obj_t* value_label_left = lv_label_create(top_container);
    lv_obj_t* value_label_divider = lv_label_create(top_container);
    lv_obj_t* value_label_right = lv_label_create(top_container);

    // Declare 3 labels for each part of the text
    lv_obj_set_width(value_label_left, LV_SIZE_CONTENT);
    lv_obj_add_flag(value_label_left, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_set_width(value_label_divider, LV_SIZE_CONTENT);
    lv_obj_add_flag(value_label_divider, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_set_width(value_label_right, LV_SIZE_CONTENT);
    lv_obj_add_flag(value_label_right, LV_OBJ_FLAG_EVENT_BUBBLE);


    // Set the styles. Larger part is the selected value
    lv_obj_add_style(value_label_left, STYLE_PTR_CM(CONTROL_TEXT_VALUE), LV_PART_MAIN);
    lv_obj_set_style_text_color(value_label_left, color_text_disabled, LV_PART_MAIN);
    lv_obj_set_style_text_font(value_label_left, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_add_style(value_label_divider, STYLE_PTR_CM(CONTROL_TEXT_VALUE), LV_PART_MAIN);
    lv_obj_add_style(value_label_right, STYLE_PTR_CM(CONTROL_TEXT_VALUE), LV_PART_MAIN);

    double left_val = *left_class->get_value_target();
    double right_val = *right_class->get_value_target();

    ie_label_text(value_label_left, left_val);
    lv_label_set_text_fmt(value_label_divider, ":");
    ie_label_text(value_label_right, right_val);

    lv_obj_t* name_label = lv_label_create(text_container);
    lv_obj_set_width(name_label, LV_PCT(100));
    lv_obj_add_style(name_label, STYLE_PTR_CM(CONTROL_TEXT_NAME), LV_PART_MAIN);
    lv_label_set_text_fmt(name_label, "I:E Ratio");

    lv_obj_t* increase_button = lv_btn_create(obj);
    lv_obj_add_event_cb(increase_button, on_control_button_press, LV_EVENT_ALL, left_class);
    lv_obj_add_event_cb(increase_button, on_control_button_press, LV_EVENT_ALL, right_class);
    lv_obj_add_style(increase_button, STYLE_PTR_CM(CONTROL_BUTTON), LV_PART_MAIN);
    lv_obj_set_size(increase_button, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    increase_button->user_data = &button_type_data[ButtonType::INCREMENT];

    lv_obj_t* increase_button_label = lv_label_create(increase_button);
    lv_obj_add_style(increase_button_label, STYLE_PTR_CM(CONTROL_BUTTON_TEXT), LV_PART_MAIN);
    lv_label_set_text_fmt(increase_button_label, "+");
    lv_obj_center(increase_button_label);

#if DEBUG_BORDER_CONTROLS
    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(reduce_button, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(reduce_button_label, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(increase_button, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(increase_button_label, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(text_container, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(top_container, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(value_label_left, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(value_label_divider, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(value_label_right, 1, LV_PART_MAIN);

    lv_obj_set_style_border_width(name_label, 1, LV_PART_MAIN);
#endif
}

/************************************************/
/*          Functions to generate buttons       */
/************************************************/

void add_start_button()
{
    lv_obj_t* button = lv_btn_create(SCR_C(VISUAL_AREA_3));
    lv_obj_set_height(button, LV_PCT(100));
    lv_obj_add_style(button, STYLE_PTR_CM(OPTION_BUTTON), LV_PART_MAIN);
    lv_obj_add_flag(button, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_style_bg_color(button, color_start_button, LV_PART_MAIN);
    lv_obj_set_style_bg_color(button, color_button_alert, LV_STATE_CHECKED);
    lv_obj_set_style_flex_grow(button, 2, LV_PART_MAIN);

    lv_obj_t* label = lv_label_create(button);
    lv_obj_set_width(label, LV_SIZE_CONTENT);
    lv_obj_add_style(label, STYLE_PTR_CM(OPTION_BUTTON_TEXT), LV_PART_MAIN);
    lv_label_set_text_fmt(label, "Start");
    lv_obj_align_to(label, button, LV_ALIGN_CENTER, 0, 0);
    lv_obj_center(label);

    auto on_press_cb = [](lv_event_t* evt) {
        lv_obj_t* btn = lv_event_get_target(evt);
        lv_obj_t* label = lv_obj_get_child(btn, 0);
        if (evt->code == LV_EVENT_VALUE_CHANGED) {
            bool enabled = (strcmp(lv_label_get_text(label), "Standby") == 0);
            if (enabled) {
                lv_label_set_text_fmt(label, "Start");
                control_change_state(States::ST_OFF);
            }
            else {
                lv_label_set_text_fmt(label, "Standby");
                control_change_state(States::ST_INSPR);
            }
#if ENABLE_CONTROL
            control_write_ventilator_params();
#endif
        }
    };

    lv_obj_add_event_cb(button, on_press_cb, LV_EVENT_ALL, nullptr);
}

void add_mute_button()
{
    lv_obj_t* button = lv_btn_create(SCR_C(VISUAL_AREA_3));
    lv_obj_set_height(button, LV_PCT(100));
    lv_obj_set_width(button, LV_SIZE_CONTENT);
    lv_obj_add_style(button, STYLE_PTR_CM(OPTION_BUTTON), LV_PART_MAIN);
    lv_obj_add_flag(button, LV_OBJ_FLAG_CHECKABLE);

    lv_obj_set_style_bg_color(button, color_button_alert, LV_STATE_CHECKED);
    lv_obj_set_style_flex_grow(button, FLEX_GROW, LV_PART_MAIN);

    lv_obj_t* label = lv_label_create(button);
    lv_obj_set_width(label, LV_SIZE_CONTENT);
    lv_obj_add_style(label, STYLE_PTR_CM(OPTION_BUTTON_TEXT), LV_PART_MAIN);
    lv_label_set_text_fmt(label, "Mute\nAlarms");
    lv_obj_align_to(label, button, LV_ALIGN_CENTER, 0, 0);
    lv_obj_center(label);
}

lv_obj_t* add_settings_button(const char* title, lv_obj_t* parent)
{
    lv_obj_t* button = lv_btn_create(parent);
    lv_obj_set_width(button, LV_PCT(100));
    lv_obj_add_style(button, STYLE_PTR_CM(OPTION_BUTTON), LV_PART_MAIN);
    lv_obj_set_style_pad_top(button, 4 px, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(button, 4 px, LV_PART_MAIN);

    lv_obj_t* label = lv_label_create(button);
    lv_label_set_text_fmt(label, title);
    lv_obj_add_style(label, STYLE_PTR_CM(OPTION_BUTTON_TEXT), LV_PART_MAIN);
    lv_obj_align_to(label, button, LV_ALIGN_CENTER, 0, 0);
    lv_obj_center(label);

    return button;
}

lv_obj_t* add_settings_toggle_button(const char* title, lv_obj_t* parent)
{
    lv_obj_t* button = add_settings_button(title, parent);

    lv_obj_set_style_bg_color(button, color_button_alert, LV_STATE_CHECKED);
    lv_obj_add_flag(button, LV_OBJ_FLAG_CHECKABLE);

    return button;
}

/************************************************/
/*     Event Handlers (callback dispatcher)     */
/************************************************/

static void on_control_button_press(lv_event_t* evt)
{
    auto value_class = static_cast<AdjustableValue*>(evt->user_data);
    if (!value_class->is_selected()) {
        return;
    }
    value_class->on_control_button_press(evt);
}

static void on_readout_update(lv_event_t* evt)
{
    auto value_class = static_cast<AdjustableValue*>(evt->user_data);
    value_class->on_readout_update(evt);
}

/************************************************/
/*               Readout Callbacks              */
/************************************************/

static void trim_spans_to_size(lv_obj_t* spangroup, size_t final_size, bool refresh)
{
    size_t size = lv_spangroup_get_child_cnt(spangroup);
    if (size == final_size) {
        return;
    }
    else if (size < final_size) {

        for (; size < final_size; size++) {
            lv_span_t* span = lv_spangroup_new_span(spangroup);
            border_span(span);
        }
        return;
    }
    else {

        while (size > final_size) {
            lv_span_t* last = lv_spangroup_get_child(spangroup, -1);
            lv_spangroup_del_span(spangroup, last);
            size--;
        }
    }

    if (refresh) {
        lv_spangroup_refr_mode(spangroup);
    }
}

static void update_readout_labels(AdjustableValue* this_ptr, lv_obj_t* spangroup)
{

    auto settings = this_ptr->get_settings();
    lv_span_t* primary_span;
    double measured = *this_ptr->get_value_measured();
    const char* formatter = settings.measured_formatter;

    if (measured <= READOUT_VALUE_NONE) { // Use blanks since we haven't measured anything yet
        trim_spans_to_size(spangroup, 1);
        primary_span = lv_spangroup_get_child(spangroup, 0);
        lv_span_set_text(primary_span, "--");
    }
    else {

        size_t spanlist_size = lv_spangroup_get_child_cnt(spangroup);
        char buf[12];
        // Get the int portion by default, we'll get the decimal portion later for float options
        snprintf(buf, 12, "%ld", (int32_t) measured);

        if (spanlist_size < 1) {
            primary_span = lv_spangroup_new_span(spangroup);
        }
        else {
            primary_span = lv_spangroup_get_child(spangroup, 0);
        }

        lv_span_set_text(primary_span, buf);

        // Make sure to cast if we're trying for an int
        if (strcmp("%d", formatter) == 0 || strcmp("%ld", formatter) == 0 || strcmp("%i", formatter) == 0) {
            trim_spans_to_size(spangroup, 1);
        }
        else {

            // Create a new span just for decimal values with smaller font
            lv_span_t* decimal_span;
            double decimal_portion = 0;
            if (measured != 0) {
                decimal_portion = abs(measured - ((int32_t) measured));
            }
            snprintf(buf, 12, formatter, decimal_portion);

            if (spanlist_size == 1) {
                decimal_span = lv_spangroup_new_span(spangroup);
                lv_style_set_text_font(&decimal_span->style, &lv_font_montserrat_28);
                border_span(decimal_span);
            }
            else {
                trim_spans_to_size(spangroup, 2);
                decimal_span = lv_spangroup_get_child(spangroup, 1);
            }

            // The formatter will give 0.xxxx, we need to cut off the 0 to attach to the large portion
            lv_span_set_text(decimal_span, &buf[1]);
        }
    }

    lv_spangroup_refr_mode(spangroup);
}

static void readout_update_cb(AdjustableValue* this_ptr, lv_event_t* evt)
{
    if (evt->code != LV_EVENT_REFRESH) {
        return;
    }

    lv_obj_t* target = this_ptr->get_obj_measured();
    lv_obj_t* value_container = lv_obj_get_child(target, 1);
    lv_obj_t* value_amount_spangroup = lv_obj_get_child(value_container, 0);

    if (!value_amount_spangroup) {
        return;
    }

    update_readout_labels(this_ptr, value_amount_spangroup);
}

/************************************************/
/*               Control Callbacks              */
/************************************************/

static void control_press_cb(AdjustableValue* this_ptr, lv_event_t* evt)
{
    if (!(evt->code == LV_EVENT_PRESSED || evt->code == LV_EVENT_LONG_PRESSED
            || evt->code == LV_EVENT_LONG_PRESSED_REPEAT)) {
        return;
    }

    auto target = lv_event_get_target(evt);
    auto settings = this_ptr->get_settings();
    if (!target || !target->user_data)
        return;
    auto data = (ButtonData*) target->user_data;

    double val = *this_ptr->get_value_target();
    switch (data->type) {
        case DECREMENT:
            val -= settings.step;
            break;
        case INCREMENT:
            val += settings.step;
            break;
        default:
            Serial.println(data->type);
            return;
    }

    if (this_ptr->value_type == IE_RATIO_LEFT || this_ptr->value_type == IE_RATIO_RIGHT) {
        this_ptr->set_value_target(floor(val * 10.0 + 0.5) / 10.0);
    }
    else {
        this_ptr->set_value_target(val);
    }

#if ENABLE_CONTROL
    control_update_waveform_param(this_ptr->value_type, (float) *this_ptr->get_value_target());
#endif
    update_control_label(this_ptr);
}

static void update_control_label(AdjustableValue* this_ptr)
{
    if (this_ptr->value_type == IE_RATIO_LEFT || this_ptr->value_type == IE_RATIO_RIGHT) {
        update_ie_control_label(this_ptr);
    }
    else {
        update_normal_label(this_ptr);
    }
}

static void update_normal_label(AdjustableValue* this_ptr)
{
    auto settings = this_ptr->get_settings();
    lv_obj_t* target = this_ptr->get_obj_target();
    lv_obj_t* text_container = lv_obj_get_child(target, 1);
    lv_obj_t* top_text_container = lv_obj_get_child(text_container, 0);
    lv_obj_t* quantity_label = lv_obj_get_child(top_text_container, 0);

    const char* formatter = settings.target_formatter;
    // Make sure to cast if we're trying for an int
    if (strcmp("%d", formatter) == 0 || strcmp("%i", formatter) == 0) {
        lv_label_set_text_fmt(quantity_label, formatter, (int32_t) *this_ptr->get_value_target());
    }
    else {
        lv_label_set_text_fmt(quantity_label, formatter, *this_ptr->get_value_target());
    }
}

static void update_ie_control_label(AdjustableValue* this_ptr)
{
    lv_obj_t* selected_label;
    get_ie_control_label(this_ptr, &selected_label);

    double val = *this_ptr->get_value_target();

    if (is_whole(val)) {
        lv_label_set_text_fmt(selected_label, "%d", (int32_t) val);
    }
    else {
        lv_label_set_text_fmt(selected_label, "%.1f", val);
    }

}

static void toggle_ie_select()
{
    AdjustableValue* left_class = &adjustable_values[IE_RATIO_LEFT];
    AdjustableValue* right_class = &adjustable_values[IE_RATIO_RIGHT];

    lv_obj_t* left_label;
    lv_obj_t* right_label;

    get_ie_control_label(left_class, &left_label);
    get_ie_control_label(right_class, &right_label);

    left_class->toggle_selected();
    right_class->toggle_selected();

    update_ie_control_state(left_class, left_label);
    update_ie_control_state(right_class, right_label);
}

/************************************************/
/*   Utility methods for callbacks & handlers   */
/************************************************/

static void get_ie_control_label(AdjustableValue* this_ptr, lv_obj_t** obj_ptr)
{
    lv_obj_t* target = this_ptr->get_obj_target();
    lv_obj_t* text_container = lv_obj_get_child(target, 1);
    lv_obj_t* top_text_container = lv_obj_get_child(text_container, 0);

    if (this_ptr->value_type == IE_RATIO_LEFT) {
        *obj_ptr = lv_obj_get_child(top_text_container, 0);
    }
    else {
        *obj_ptr = lv_obj_get_child(top_text_container, 2);
    }
}

static void update_ie_control_state(AdjustableValue* this_ptr, lv_obj_t* label)
{

    if (this_ptr->is_selected()) {
        lv_obj_remove_local_style_prop(label, LV_STYLE_TEXT_FONT, LV_PART_MAIN);
        lv_obj_remove_local_style_prop(label, LV_STYLE_TEXT_COLOR, LV_PART_MAIN);
    }
    else {
        lv_obj_set_style_text_font(label, &lv_font_montserrat_20, LV_PART_MAIN);
        lv_obj_set_style_text_color(label, color_text_disabled, LV_PART_MAIN);
    }
}