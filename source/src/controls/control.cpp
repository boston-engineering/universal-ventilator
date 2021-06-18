#include <DueTimer.h>
#include <ams_as5048b.h>
#include <AccelStepper.h>
#include "../config/uvent_conf.h"
#include "actuators/actuator.h"
#include "control.h"

// Instance to control the paddle
Actuator actuator;

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

    // Initialize the actuator
    actuator.init();

    // Initialize the state machine
    machine.setup();

    /* Setup a timer and a function handler to run
     * the s
     */
    Timer0.attachInterrupt(control_handler);
    Timer0.start(CONTROL_HANDLER_PERIOD_US);

    /* Setup a timer and a function handler to run
     * the actuation.
     */
    Timer1.attachInterrupt(actuator_handler);
    Timer1.start(ACTUATOR_HANDLER_PERIOD_US);
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
    actuator.zero_position();
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