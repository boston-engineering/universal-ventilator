#include "control.h"
#include "../config/uvent_conf.h"
#include "actuators/actuator.h"
#include "eeprom/storage.h"
#include "sensors/pressure_sensor.h"
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

/* State machine instance. Takes in a pointer to actuator
 * as there are actuator commands within the state machine.
 */
Machine machine(States::ST_STARTUP, &actuator);

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

double control_get_gauge_pressure()
{
    return gauge_sensor.get_pressure(units_pressure::cmH20);
}

double control_get_diff_pressure()
{
    return diff_sensor.get_pressure(units_pressure::cmH20);
}