#include "actuator.h"
#include "controls/control.h"
#include "utilities/logging.h"

void Actuator::init()
{
    stepper_fb.begin();
    stepper.init();

    // Enable the drive
    stepper.set_enable(true);

    // Set the current position as home.
    stepper.set_position_as_home(0);
}

void Actuator::run()
{
    stepper.run();
}

void Actuator::set_enable(bool en)
{
    stepper.set_enable(en);
}

void Actuator::set_speed(Tick_Type tt, float speed)
{
    if (tt == Tick_Type::TT_DEGREES) {
        /* Degree to steps calculation.
        * In reference to the actuator pulley,
        * 360 degrees(1 rev) is TIMING_PULLEY_STEPS_PER_REV steps.
        * X degrees is X * TIMING_PULLEY_STEPS_PER_REV/360
        */
        double steps_per_sec = TIMING_PULLEY_DEGREES_TO_STEPS(speed);
        stepper.set_speed(float(steps_per_sec));
    }
    else {
        stepper.set_speed(speed);
    }

    // Store previous position value. Used to detect movement.
    prev_position = get_position();
}

void Actuator::set_position(Tick_Type tt, double value)
{
    if (tt == Tick_Type::TT_DEGREES) {
        /* Degree to steps calculation.
        * In reference to the actuator pulley,
        * 360 degrees is TIMING_PULLEY_STEPS_PER_REV steps.
        * X degrees is TIMING_PULLEY_STEPS_PER_REV/360
        */
        double angle_to_steps = TIMING_PULLEY_DEGREES_TO_STEPS(value);
        stepper.set_position(angle_to_steps);
    }
    else {
        stepper.set_position(value);
    }
}

void Actuator::set_position_relative(Tick_Type tt, double value)
{
    if (tt == Tick_Type::TT_DEGREES) {
        /* Degree to steps calculation.
        * In reference to the actuator pulley,
        * 360 degrees is TIMING_PULLEY_STEPS_PER_REV steps.
        * X degrees is TIMING_PULLEY_STEPS_PER_REV/360
        */
        double angle_to_steps = TIMING_PULLEY_DEGREES_TO_STEPS(value);
        stepper.set_position_relative(angle_to_steps);
    }
    else {
        stepper.set_position_relative(value);
    }
}

void Actuator::set_position_as_home()
{
    stepper.set_position_as_home(0);
}

void Actuator::home()
{
    stepper.home();
}

bool Actuator::is_home()
{
    // double current_position = stepper.get_position();
    double current_position = get_position();

    /* Allow a degree of dither to detect home.
     * The stepper motor is setup to perform full steps.
     * The teeth ratio is 60/14, which is 1:4.29
     * which means, 4.29 revs of the stepper is 1 rev of the main wheel.
     * So 1 degree, full step on the stepper would give 1.8 *(1/4.29)
     * = 0.419~0.42 degrees on the wobbler shaft.
     * Check within a degree while homing.
     */
    return (((current_position >= 0.2) && (current_position <= 1.0)));
}

bool Actuator::is_running()
{
    return stepper.is_moving();
}

bool Actuator::target_reached()
{
    return stepper.target_reached();
}

bool Actuator::is_moving()
{
    /* The stepper drive reports movement if steps are being
     * issued. It does not mean the stepper is actually moving.
     * Eg. When steps are incoming, but the power stage is off.
     * Instead use the angle sensor to detect movement.
     */
    double current_position = get_position();

    // Steps are being issued. Check the angle sensor for movement.
    double abs_position = fabs(current_position - prev_position);

    prev_position = current_position;

    /* Look for a change in half a degree.
     * Checks are done every 20ms. If the homing speed is 200 steps/sec,
     * there is a step every 5ms. A full step of the stepper, moves the timing
     * pulley 1.8 *(1/4.29)=0.42 degrees
     * At the 20ms rate, the pulley would have moved (20/5) * 0.42 = 1.68
     * Look for a change of at least 0.5 degrees, since that will 
     * indicate a movement.
     */
    return (abs_position > 0.5);
}

float Actuator::get_current_speed()
{
    // [TODO]
    return 0;
}

double Actuator::get_position()
{
    return stepper_fb.angleR(U_DEG, true);
}

double Actuator::get_position_raw()
{
    return stepper_fb.angleR(U_RAW, true);
}

double Actuator::degrees_to_volume(C_Stat compliance)
{
    double position = get_position();

    // The actuator can only move to MAX_ACT_POS_DEG, before going back. Limit it here.
    if (position > MAX_ACT_POS_DEG) {
        position = MAX_ACT_POS_DEG;
    }

    double volume = 0.0;

    switch (compliance) {
    case C_Stat::NONE:
        volume = D2V_COEF_A_NO_LUNG * pow(position, 4) + D2V_COEF_B_NO_LUNG * pow(position, 3) + D2V_COEF_C_NO_LUNG * pow(position, 2) + D2V_COEF_D_NO_LUNG * position;
        break;
    case C_Stat::TWENTY:
        volume = D2V_COEF_A_COMP_20 * pow(position, 4) + D2V_COEF_B_COMP_20 * pow(position, 3) + D2V_COEF_C_COMP_20 * pow(position, 2) + D2V_COEF_D_COMP_20 * position;
        break;
    case C_Stat::FIFTY:
        volume = D2V_COEF_A_COMP_50 * pow(position, 4) + D2V_COEF_B_COMP_50 * pow(position, 3) + D2V_COEF_C_COMP_50 * pow(position, 2) + D2V_COEF_D_COMP_50 * position;
        break;
    default:
        // Unknown complaince.
        return -1;
    }

    // Cage to max volume.
    if (volume > (MAX_BAG_VOL_L)) {
        volume = (MAX_BAG_VOL_L);
    }

    return volume;
}

double Actuator::volume_to_degrees(C_Stat compliance, double volume)
{
    double degrees = 0.0;

    // Check if volume requested is within limit
    if (volume < MIN_BAG_VOL_L && volume > MAX_BAG_VOL_L) {
        return -1;
    }

    switch (compliance) {
    case C_Stat::NONE:
        degrees = (V2D_COEF_A_NO_LUNG * pow(volume, 3)) + (V2D_COEF_B_NO_LUNG * pow(volume, 2)) + (V2D_COEF_C_NO_LUNG * volume) + V2D_COEF_D_NO_LUNG;
        break;
    case C_Stat::TWENTY:
        degrees = (V2D_COEF_A_COMP_20 * pow(volume, 3)) + (V2D_COEF_B_COMP_20 * pow(volume, 2)) + (V2D_COEF_C_COMP_20 * volume) + V2D_COEF_D_COMP_20;
        break;
    case C_Stat::FIFTY:
        degrees = (V2D_COEF_A_COMP_50 * pow(volume, 3)) + (V2D_COEF_B_COMP_50 * pow(volume, 2)) + (V2D_COEF_C_COMP_50 * volume) + V2D_COEF_D_COMP_50;
        break;
    default:
        // Unknown compliance
        return -1;
    }

    // The actuator can only move to MAX_ACT_POS_DEG, before going back. Limit it here.
    if (degrees > MAX_ACT_POS_DEG) {
        degrees = MAX_ACT_POS_DEG;
    }

    return degrees;
}

/* Set the current reading of the angle sensor as zero.
 * and return the zero value.
 */
uint16_t Actuator::set_current_position_as_zero()
{
    // Zero the zero register first, then write the actual value.
    stepper_fb.zeroRegW(0);
    uint16_t new_zero = stepper_fb.angleRegR();
    stepper_fb.zeroRegW(new_zero);

    return new_zero;
}

/* Sets the zero offset to the angle sensor.
 * This is done at startup.
 */
void Actuator::set_zero_position(uint16_t new_zero)
{
    // Zero the zero register first, then write the actual value.
    stepper_fb.zeroRegW(0);
    stepper_fb.zeroRegW(new_zero);
}

bool Actuator::add_correction()
{
    double current_position = get_position();

    /* Only correct for 5 degrees of offset on either side of 0.
     * If the offset is more than that, then something is wrong.
     * Move in deltas of x degrees at a time.
     */
    if (current_position > 0.0 && current_position <= 5.0) {
        set_position_relative(Tick_Type::TT_STEPS, TIMING_PULLEY_DEGREES_TO_STEPS(-1.0));
        set_speed(Tick_Type::TT_STEPS, 100);
#if DEBUG_CORRECTION
        serial_printf("Correcting! %0.2f\n", current_position);
#endif
    }
    else if ((current_position > 355 && current_position < 360.0)) {
        set_position_relative(Tick_Type::TT_STEPS, TIMING_PULLEY_DEGREES_TO_STEPS(1.0));
        set_speed(Tick_Type::TT_STEPS, 100);
#if DEBUG_CORRECTION
        serial_printf("Correcting! %0.2f\n", current_position);
#endif
    }
    else if (current_position > 5.0 && current_position < 355.0) {
        // Unable to correct
        return false;
    }

    return true;
}

Fault Actuator::calculate_trajectory(const float& duration_s, const float& goal_pos_deg, float& vel_deg)
{
    if (duration_s <= 0) return Fault::FT_ACTUATOR_INVALID_TIME;// Invalid time.

    // Get the current position of the actuator
    const float cur_pos_deg = (float) get_position();

    // Calculate the distance to move.
    const float distance_deg = abs(goal_pos_deg - cur_pos_deg);

    // Calculate velocity of travel in degrees/second.
    vel_deg = distance_deg / duration_s;

    if (TIMING_PULLEY_DEGREES_TO_STEPS(vel_deg) > STEPPER_MAX_STEPS_PER_SECOND) {
        serial_printf("Max velocity requested! %.2f, clipping to %0.2f!\n", vel_deg, TIMING_PULLEY_STEPS_TO_DEGREES(STEPPER_MAX_STEPS_PER_SECOND));

        // Cap to max velocity
        vel_deg = TIMING_PULLEY_STEPS_TO_DEGREES(STEPPER_MAX_STEPS_PER_SECOND);
    }

#if DEBUG_WAVEFORM
    serial_printf("Goal:%f, Speed: %f\n", goal_pos_deg, vel_deg);
#endif

    return Fault::FT_NONE;
}
