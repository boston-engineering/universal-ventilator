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

/* The type of equation used is based on whether values from 
 * the Fluke Biomedical VT900A Gas Flow Analyzer were taken
 * with or without a artificial lung. Values taken
 * with a artificial lung were taken at
 * Static compliance levels of C = 50, C = 20, and C = none (no lung)
*/
enum class C_Stat {
    NONE,
    FIFTY,
    TWENTY
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
    void set_enable(bool);

    // Set the speed in steps.
    void set_speed(Tick_Type tt, float speed);

    float get_current_speed();
    void home();
    bool is_home();
    bool is_moving();
    double get_position();
    double get_position_raw();
    double get_tidal_volume(C_Stat compliance);
    uint16_t set_current_position_as_zero();
    void set_zero_position(uint16_t new_zero);
    void set_position(Tick_Type tt, double value);
    void set_position_relative(Tick_Type tt, double value);
    bool is_running();
    void set_position_as_home();
    bool target_reached();
    bool add_correction();

private:
    Stepper stepper;

    // Feedback
    AMS_AS5048B stepper_fb;

    // Prev. value of angle sensor.
    // Used to detect movement.
    double prev_position;

    // No Lung
    // y = -5E-09*x^4 + 1E-06x^3 - 4E-05x^2 + 0.0003x
    // C_Stat = none
    // Equation was derived by mapping volume displaced per degree the paddle moved. A Fluke Biomedical VT900A Gas Flow Analyzer was used to measure the volume displaced.
    double const COEF_A_NO_LUNG = -0.00000000539259422946378;
    double const COEF_B_NO_LUNG = 0.00000137006464979886;
    double const COEF_C_NO_LUNG = -0.0000370548541166471;
    double const COEF_D_NO_LUNG = 0.000260254960634754;

    // With Lung
    // y = -3E-09*x^4 + 9E-07*x^3 - 3E-05*x^2 - 7E-05*x
    // C_Stat = 20 (mL/cmH2O)
    // Equation was derived by mapping volume displaced per degree the paddle moved. A Fluke Biomedical VT900A Gas Flow Analyzer was used to measure the volume displaced.
    double const COEF_A_COMP_20 = -0.00000000318509207676739;
    double const COEF_B_COMP_20 = 0.000000868855116173189;
    double const COEF_C_COMP_20 = -0.0000278632433332315;
    double const COEF_D_COMP_20 = -0.0000703187442674391;

    // With Lung
    // y = -3E-09*x^4 + 8E-07*x^3 - 3E-06*x^2 - 0.0006*x
    // C_Stat = 50 (mL/cmH2O)
    // Equation was derived by mapping volume displaced per degree the paddle moved. A Fluke Biomedical VT900A Gas Flow Analyzer was used to measure the volume displaced.
    double const COEF_A_COMP_50 = -0.00000000342520216878951;
    double const COEF_B_COMP_50 = 0.000000834768084837607;
    double const COEF_C_COMP_50 = -0.00000305968137895296;
    double const COEF_D_COMP_50 = -0.000643516160449664;
};

#endif