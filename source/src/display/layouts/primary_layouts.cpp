#include <display/main_display.h>
#include "layouts.h"

// Static callback function defs
static void on_readout_update(lv_event_t*);
static void readout_update_cb(AdjustableValue*, lv_event_t*);

static void on_control_button_press(lv_event_t*);
static void control_press_cb(AdjustableValue*, lv_event_t*);
static void update_control_label(AdjustableValue* this_ptr);

// Functions to generate & fill entire screen areas

void setup_readouts()
{
    lv_obj_t* parent = SCR_C(VISUAL_AREA_1);
    lv_obj_clean(parent);

    setup_adjustable_readout(TIDAL_VOLUME);
    setup_adjustable_readout(RESPIRATION_RATE);
    // TODO add I:E
    setup_adjustable_readout(PEEP);
    setup_adjustable_readout(PIP);
    setup_adjustable_readout(CUR_PRESSURE);
}

void setup_controls()
{
    lv_obj_t* parent = SCR_C(CONTROL_AREA_1);
    lv_obj_clean(parent);
    setup_adjustable_control(TIDAL_VOLUME);
    setup_adjustable_control(RESPIRATION_RATE);
    // TODO add I:E
    setup_adjustable_control(PEEP);
    setup_adjustable_control(PIP);
    setup_adjustable_control(PLATEAU_TIME);
}

void setup_buttons()
{
    lv_obj_t* parent = SCR_C(VISUAL_AREA_3);
    lv_obj_clean(parent);
}

// Functions to generate components like the readouts & controls

void setup_adjustable_readout(AdjValueType type, const char* override_str)
{
    auto value_class = &adjustable_values[type];
    AdjValueParams settings = value_class->get_settings();

    if (!settings.create_readout) {
        return;
    }

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

    lv_obj_t* value_amount_label = lv_label_create(value_container);
    lv_obj_t* value_unit_label;
    lv_obj_set_width(value_amount_label, LV_SIZE_CONTENT);
    lv_obj_add_style(value_amount_label, STYLE_PTR_CM(READOUT_VALUE_AMOUNT_TEXT), LV_PART_MAIN);

    if (override_str) {  // If we have a pre-formatted string like I:E, just use that

        lv_label_set_text_fmt(value_amount_label, override_str);

    }
    else {            // Else get the actual thing we need

        double* measured = value_class->get_value_measured();
        const char* formatter = settings.measured_formatter;
        if (*measured < 0) { // Use blanks since we haven't measured anything yet
            lv_label_set_text_fmt(value_amount_label, "--");
        }
        else {
            // Make sure to cast if we're trying for an int
            if (strcmp("%d", formatter) == 0 || strcmp("%i", formatter) == 0) {
                lv_label_set_text_fmt(value_amount_label, formatter, (int32_t) *value_class->get_value_measured());
            }
            else {
                lv_label_set_text_fmt(value_amount_label, formatter, *value_class->get_value_measured());
            }
        }
    }

    lv_obj_center(value_amount_label);

    if (settings.unit) {
        value_unit_label = lv_label_create(value_container);
        lv_obj_add_style(value_unit_label, STYLE_PTR_CM(READOUT_VALUE_UNIT_TEXT), LV_PART_MAIN);
        lv_label_set_text_fmt(value_unit_label, settings.unit);
    }

#if DEBUG_BORDER_READOUTS
    lv_obj_set_style_border_width(name_container, 1 px, LV_PART_MAIN);
    lv_obj_set_style_border_width(value_container, 1 px, LV_PART_MAIN);
    lv_obj_set_style_border_width(name_label, 1 px, LV_PART_MAIN);
    lv_obj_set_style_border_width(value_amount_label, 1 px, LV_PART_MAIN);
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
    lv_obj_add_event_cb(reduce_button, on_control_button_press, LV_EVENT_PRESSED, value_class);
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
    lv_obj_add_event_cb(increase_button, on_control_button_press, LV_EVENT_PRESSED, value_class);
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

/**************************************
 * Callback functions & event handlers
 ***************************************/

// Event handlers to run callbacks

static void on_control_button_press(lv_event_t* evt)
{
    auto value_class = static_cast<AdjustableValue*>(evt->user_data);
    value_class->on_control_button_press(evt);
}

static void on_readout_update(lv_event_t* evt)
{
    auto value_class = static_cast<AdjustableValue*>(evt->user_data);
    value_class->on_readout_update(evt);
}

// Readout callbacks

static void readout_update_cb(AdjustableValue* this_ptr, lv_event_t* evt)
{
    if (evt->code != LV_EVENT_REFRESH) {
        return;
    }

    auto settings = this_ptr->get_settings();
    lv_obj_t* target = this_ptr->get_obj_measured();
    lv_obj_t* value_container = lv_obj_get_child(target, 1);
    lv_obj_t* value_amount_label = lv_obj_get_child(value_container, 0);

    double* measured = this_ptr->get_value_measured();
    const char* formatter = settings.measured_formatter;
    if (*measured < 0) { // Use blanks since we haven't measured anything yet
        lv_label_set_text_fmt(value_amount_label, "--");
    }
    else {
        // Make sure to cast if we're trying for an int
        if (strcmp("%d", formatter) == 0 || strcmp("%i", formatter) == 0) {
            lv_label_set_text_fmt(value_amount_label, formatter, (int32_t) *this_ptr->get_value_measured());
        }
        else {
            lv_label_set_text_fmt(value_amount_label, formatter, *this_ptr->get_value_measured());
        }
    }
}

// Control callbacks

static void control_press_cb(AdjustableValue* this_ptr, lv_event_t* evt)
{
    auto target = lv_event_get_target(evt);
    auto settings = this_ptr->get_settings();
    if (!target || !target->user_data)
        return;
    auto data = (ButtonData*) target->user_data;
//    Serial.print("Got Button Press. Type is ");

    double val = *this_ptr->get_value_target();
    switch (data->type) {
        case DECREMENT:
//            Serial.println("Decrement");
            val -= settings.step;
            break;
        case INCREMENT:
//            Serial.println("Increment");
            val += settings.step;
            break;
        default:
            Serial.println(data->type);
            return;
    }

    this_ptr->set_value_target(val);
    update_control_label(this_ptr);
//  Serial.println("Finished updating label");
}

static void update_control_label(AdjustableValue* this_ptr)
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