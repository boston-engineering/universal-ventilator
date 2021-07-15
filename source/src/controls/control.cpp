#include "control.h"
#include <display/main_display.h>
#include <utilities/util.h>
#include "../config/uvent_conf.h"
#include "actuators/actuator.h"
#include "eeprom/storage.h"
#include "sensors/pressure_sensor.h"
#include "waveform.h"
#include <AccelStepper.h>
#include <DueTimer.h>
#include <ams_as5048b.h>

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

void loop_test_readout()
{

    static uint32_t timing_step_counter = 0;
    static uint32_t timing_refresh_counter = 0;

    if (has_time_elapsed(&timing_step_counter, TEST_READOUT_STEP_INTERVAL)) {
        for (auto& adjustable_value : adjustable_values) {
            if (adjustable_value.value_type == IE_RATIO_LEFT || adjustable_value.value_type == IE_RATIO_RIGHT)
                continue;
            AdjValueParams settings = adjustable_value.get_settings();
            long add_val = random(0, long(settings.step) + 1);
            double new_val = *adjustable_value.get_value_measured() + add_val;
            if (new_val > settings.max_value) {
                new_val = settings.min_value;
            }
            adjustable_value.set_value_measured(new_val);
        }
    }

    if (has_time_elapsed(&timing_refresh_counter, TEST_READOUT_REFRESH_INTERVAL)) {
        for (auto& adjustable_value : adjustable_values) {
            adjustable_value.refresh_readout();
        }
    }
}

void loop_update_readouts()
{
    static uint32_t sensor_poll_counter = 0;
    if (has_time_elapsed(&sensor_poll_counter, SENSOR_POLL_INTERVAL)) {

        set_readout(AdjValueType::TIDAL_VOLUME, control_get_degrees_to_volume_ml());
        set_readout(AdjValueType::CUR_PRESSURE, control_get_gauge_pressure());

        // Refresh all of the readout labels
        for (auto& value : adjustable_values) {
            value.refresh_readout();
        }
    }
}

void control_update_waveform_param(AdjValueType type, float new_value)
{
    if (type >= AdjValueType::ADJ_VALUE_COUNT) {
        return;
    }
    waveform_params* wave_params = control_get_waveform_params();
    switch (type) {
        case TIDAL_VOLUME:
            wave_params->volume_ml = new_value;
            LV_LOG_USER("Tidal Volume is now %.1f", new_value);
            break;
        case RESPIRATION_RATE:
            wave_params->bpm = (uint16_t) new_value;
            LV_LOG_USER("Respiration Rate is now %.1f", new_value);
            break;
        case PLATEAU_TIME:
            // TODO
            LV_LOG_USER("TO ADD: PLATEAU TIME");
            break;
        case IE_RATIO_LEFT:
            wave_params->ie_i = new_value;
            LV_LOG_USER("IE Inspiration is now %.1f", new_value);
            break;
        case IE_RATIO_RIGHT:
            wave_params->ie_e = new_value;
            LV_LOG_USER("IE Expiration is now %.1f", new_value);
            break;
        default:
            break;
    }
    control_calculate_waveform();
}

static void load_stored_target(AdjustableValue* value, uvent_settings& settings)
{
    double val = 0;
    switch (value->value_type) {
        case TIDAL_VOLUME:
            val = settings.tidal_volume;
            break;
        case RESPIRATION_RATE:
            val = settings.respiration_rate;
            break;
        case PEEP:
            val = settings.peep_limit;
            break;
        case PIP:
            val = settings.pip_limit;
            break;
        case PLATEAU_TIME:
            val = settings.plateau_time;
            break;
        case IE_RATIO_LEFT:
            val = settings.ie_ratio_left;
            break;
        case IE_RATIO_RIGHT:
            val = settings.ie_ratio_right;
            break;
        default:
            break;
    }
    value->set_value_target(val);
}

void init_adjustable_values()
{
    uvent_settings settings{};
    storage.get_settings(settings);

    for (uint i = 0; i < AdjValueType::ADJ_VALUE_COUNT; i++) {
        AdjustableValue* value_class = &adjustable_values[i];
        value_class->init(static_cast<AdjValueType>(i));
        load_stored_target(value_class, settings);
        value_class->set_value_measured(READOUT_VALUE_DEFAULT);
    }
    adjustable_values[AdjValueType::IE_RATIO_LEFT].set_selected(false);
}

void set_readout(AdjValueType type, double val)
{
    if (type >= AdjValueType::ADJ_VALUE_COUNT) {
        return;
    }
    adjustable_values[type].set_value_measured(val);
}

double get_readout(AdjValueType type)
{
    if (type >= AdjValueType::ADJ_VALUE_COUNT) {
        return -1;
    }
    return *adjustable_values[type].get_value_measured();
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
#if ENABLE_CONTROL
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
    if (settings.diff_pressure_type == PRESSURE_SENSOR_TYPE_0) {
        diff_sensor.init(MAX_DIFF_PRESSURE_TYPE_0, MIN_DIFF_PRESSURE_TYPE_0, RESISTANCE_1, RESISTANCE_2,
                settings.dpressure_offset_adc_counts);
    }
    else if (settings.diff_pressure_type == PRESSURE_SENSOR_TYPE_1) {
        diff_sensor.init(MAX_DIFF_PRESSURE_TYPE_1, MIN_DIFF_PRESSURE_TYPE_1, RESISTANCE_1, RESISTANCE_2,
                settings.dpressure_offset_adc_counts);
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
#endif
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

bool control_is_crc_ok()
{
    return storage.is_crc_ok();
}

double control_get_degrees_to_volume(C_Stat compliance)
{
    return actuator.degrees_to_volume(compliance);
}

double control_get_degrees_to_volume_ml(C_Stat compliance)
{
    return actuator.degrees_to_volume(compliance) * 1000;
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

void control_waveform_display_details()
{
    waveform.display_details();
}

double control_get_gauge_pressure()
{
    return gauge_sensor.get_pressure(units_pressure::cmH20);
}

double control_get_diff_pressure()
{
    return diff_sensor.get_pressure(units_pressure::cmH20);
}