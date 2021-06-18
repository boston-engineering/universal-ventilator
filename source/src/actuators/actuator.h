#ifndef UVENT_ACTUATOR_H
#define UVENT_ACTUATOR_H

#include "stepper.h"

/* The type of ticks for the motor.
 * Although the actuator can be controlled in degrees,
 * having access to the steps allows for easier debuggin
 * This enum aids in transforming values from the higher
 * layers to steps, which the low level uses.
 */
enum class Tick_Type {
    TT_DEGREES,
    TT_STEPS
};

class Actuator
/* There can be other actuator types - steppers, DC motors,
 * Wiper DC motors. The actuator interface serves as a common interface
 * to any of them. To use other drives or motors, an associated driver
 * needs to be written. 
 * Here, the stepper driver is used.
 */
{
public:
    void init();
    void run();
    void disable();

    // Set the speed in steps.
    void set_speed(Tick_Type tt, float speed);

    float get_current_speed();
    void home();
    bool is_home();
    bool is_moving();
    double get_position();
    double get_position_raw();
    void zero_position();
    void set_position(Tick_Type tt, double value);
    bool is_running();
    void set_position_as_home();
    bool target_reached();

private:
    Stepper stepper;

    // Feedback
    AMS_AS5048B stepper_fb;

    // Prev. value of angle sensor.
    // Used to detect movement.
    double prev_position;

    // Zero position register val
    uint16_t fb_zero_reg_value = 5745;
};

#endif