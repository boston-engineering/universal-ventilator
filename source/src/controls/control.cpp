#include "control.h"
#include <display/main_display.h>
#include "../config/uvent_conf.h"
#include "actuators/actuator.h"
#include "eeprom/storage.h"
#include "sensors/pressure_sensor.h"
#include "waveform.h"
#include <AccelStepper.h>
#include <DueTimer.h>
#include <ams_as5048b.h>

static void on_control_button_press(lv_event_t* evt)
{
    auto value_class = static_cast<AdjustableValue<double>*>(evt->user_data);
    value_class->on_control_button_press(evt);
}

static void update_control_label(AdjustableValue<double>* this_ptr)
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

static void control_press_cb(AdjustableValue<double>* this_ptr, lv_event_t* evt)
{
    auto target = lv_event_get_target(evt);
    auto settings = this_ptr->get_settings();
    if (!target || !target->user_data)
        return;
    auto data = (ButtonData*) target->user_data;
//    Serial.print("Got Button Press. Type is ");

    double val = 0;
    switch (data->type) {
        case DECREMENT:
//            Serial.println("Decrement");
            val = max(*this_ptr->get_value_target() - settings.step, settings.min_value);
            break;
        case INCREMENT:
//            Serial.println("Increment");
            val = min(*this_ptr->get_value_target() + settings.step, settings.max_value);
            break;
        default:
            Serial.println(data->type);
            return;
    }

    this_ptr->set_value_target(val);
    update_control_label(this_ptr);
//    Serial.println("Finished updating label");
}

// Instance to control the paddle
Actuator actuator;

// Storage instance
Storage storage;

// Gauge Pressure Sensor instance
PressureSensor gauge_sensor = {PRESSURE_GAUGE_PIN};

// Differential Pressure Sensor instance
PressureSensor diff_sensor = {PRESSURE_DIFF_PIN};
// Waveform instance
Waveform waveform;

/* State machine instance. Takes in a pointer to actuator
 * as there are actuator commands within the state machine.
 */
Machine machine(States::ST_STARTUP, &actuator, &waveform);

void setup_controlled_value(AdjValueType type);
void setup_adjustable_readout(AdjValueType type);
void setup_adjustable_control(AdjValueType type);

void init_adjustable_values()
{
    // TODO Read stored values from EEPROM on startup
    for (uint i = 0; i < AdjValueType::ADJ_VALUE_COUNT; i++) {
        auto value_class = &adjustable_values[i];
        value_class->init(static_cast<AdjValueType>(i));
        *value_class->get_value_measured() = -1;
    }

    setup_controlled_value(TIDAL_VOLUME);
    setup_controlled_value(RESPIRATION_RATE);
    // TODO add I:E
    setup_controlled_value(PEEP);
    setup_controlled_value(PIP);
    setup_controlled_value(CUR_PRESSURE);
    setup_controlled_value(PLATEAU_TIME);

}

void setup_controlled_value(AdjValueType type)
{
    auto value_class = &adjustable_values[type];
    auto settings = value_class->get_settings();

    if (settings.create_readout) {
        setup_adjustable_readout(type);
    }

    if (settings.create_control) {
        setup_adjustable_control(type);
    }
}

void setup_adjustable_readout(AdjValueType type)
{
    auto value_class = &adjustable_values[type];
    auto settings = value_class->get_settings();

    lv_obj_t* parent_container = SCR_C(VISUAL_AREA_1);

    lv_obj_t* obj = lv_obj_create(parent_container);
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

void control_handler()
{
    static bool ledOn = false;

    // LED to visually show state machine is running.
    digitalWrite(DEBUG_LED, ledOn);

    // Toggle the LED.
    ledOn = !ledOn;

    // Run the state machine
    machine.run();
}

/* Interrupt callback to service the actuator
 * The actuator, in this case a stepper is serviced periodically
 * to "step" the motor.
 */
void actuator_handler()
{
    actuator.run();
}

/* Initilaize sub-systems
 */
void control_init()
{
    // Debug led setup
    pinMode(DEBUG_LED, OUTPUT);

    //  Storage init
    storage.init();

    // Initialize the actuator
    actuator.init();

    /* On startup, the angle feedback sensor needs to be programmed with a calibrated
     * zero. Usually this is an OTP value burned into the angle register, but it is risky to
     * do that, as the magnet may fall or go out-of alignment during transport and the zero cannot
     * be programmed again(because OTP).
     * To avoid the risk, the EEPROM stores the zero value, which is burned in during manufacture.
     * This value can be changed by manually "re-homing" the actuator at the user's discretion.
     *
     * The below few lines, load the zero value from the EEPROM into the angle sensor register
     * at statup. Control can then use the value from the angle sensor to home the actuator.
     */
    uvent_settings settings;
    storage.get_settings(settings);
    actuator.set_zero_position(settings.actuator_home_offset_adc_counts);

    // Initialize the Gauge Pressure Sensor
    gauge_sensor.init(MAX_GAUGE_PRESSURE, MIN_GAUGE_PRESSURE, RESISTANCE_1, RESISTANCE_2, settings.gpressure_offset_adc_counts);

    // Initialize the Differential Pressure Sensor
    if (settings.diff_pressure_type == 0) {
        diff_sensor.init(MAX_DIFF_PRESSURE_TYPE_0, MIN_DIFF_PRESSURE_TYPE_0, RESISTANCE_1, RESISTANCE_2, settings.dpressure_offset_adc_counts);
    }
    else if (settings.diff_pressure_type == 1) {
        diff_sensor.init(MAX_DIFF_PRESSURE_TYPE_1, MIN_DIFF_PRESSURE_TYPE_1, RESISTANCE_1, RESISTANCE_2, settings.dpressure_offset_adc_counts);
    }

    // Initialize the state machine
    machine.setup();

    /* Setup a timer and a function handler to run
     * the state machine.
     */
    Timer0.attachInterrupt(control_handler);
    Timer0.start(CONTROL_HANDLER_PERIOD_US);

    /* Setup a timer and a function handler to run
     * the actuation.
     */
    Timer1.attachInterrupt(actuator_handler);
    Timer1.start(ACTUATOR_HANDLER_PERIOD_US);

    // Check EEPROM CRC. Load defaults if CRC fails.
    if (!storage.is_crc_ok()) {
        Serial.println("CRC failed. Loading defaults.");
        // No settings found, or settings corrupted.
        storage.load_defaults();
    }
}

/* Called by loop() from main, this function services control
 * parametes and functions.
 * Since this is called by the loop(), the call interval
 * in not periodic.
 */
void control_service()
{
}

/* Get the current angular position of the actuator
 */
double control_get_actuator_position()
{
    return actuator.get_position();
}

/* Get the current raw register value of the angle sensor.
 * This is a debug stub used to get the offset to set home.
 */
double control_get_actuator_position_raw()
{
    return actuator.get_position_raw();
}

/* Set the current angular position of the actuator as home
 */
void control_zero_actuator_position()
{
    uvent_settings settings;
    storage.get_settings(settings);

    // Zero the angle sensor and write the value to the EEPROM
    settings.actuator_home_offset_adc_counts = actuator.set_current_position_as_zero();

    // Store this value to the eeprom.
    storage.set_settings(settings);
}

/* Request to switch the state of the state machine.
 */
void control_change_state(States new_state)
{
    machine.change_state(new_state);
}

void control_actuator_manual_move(Tick_Type tt, double angle, double speed)
{
    actuator.set_position(tt, angle);
    actuator.set_speed(tt, speed);
}

States control_get_state()
{
    return machine.get_current_state();
}

const char* control_get_state_string()
{
    return machine.get_current_state_string();
}

const char** control_get_state_list(uint8_t* size)
{
    return machine.get_state_list(size);
}

void control_display_storage()
{
    storage.display_storage();
}

double control_get_degrees_to_volume(C_Stat compliance)
{
    return actuator.degrees_to_volume(compliance);
}

double control_calc_volume_to_degrees(C_Stat compliance, double volume)
{
    return actuator.volume_to_degrees(compliance, volume);
}

void control_actuator_set_enable(bool en)
{
    actuator.set_enable(en);
}

waveform_params* control_get_waveform_params(void)
{
    return (waveform.get_params());
}

void control_calculate_waveform()
{
    waveform.calculate_waveform();
}

double control_get_gauge_pressure()
{
    return gauge_sensor.get_pressure(units_pressure::cmH20);
}

double control_get_diff_pressure()
{
    return diff_sensor.get_pressure(units_pressure::cmH20);
}