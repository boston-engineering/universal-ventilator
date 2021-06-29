#include "pressure_sensor.h"

PressureSensor::PressureSensor(int analog_pin, double max_psi, double min_psi, int resistance_ohms_1, int resistance_ohms_2) : analog_pin(analog_pin)
{
    init(max_psi, min_psi, resistance_ohms_1, resistance_ohms_2, 0);
}

void PressureSensor::init(double max_psi, double min_psi, int resistance_ohms_1, int resistance_ohms_2, int pressure_offset_adc_counts)
{
    // Set resolution to 12 bits, the default is 10
    analogReadResolution(ADC_RESOLUTION);
    int max_resolution_units = pow(2, ADC_RESOLUTION);
    diff_zero_resolution = max_resolution_units / 2;

    // Calculate voltage step for voltage through circuit and across the resistors
    // More information about these calculations in the README.md
    double stage_one_voltage_calc = VOLTAGE_ADC_REF / max_resolution_units;
    double stage_two_voltage_calc = (double) (resistance_ohms_1 + resistance_ohms_2) / resistance_ohms_2;
    double voltage_step = stage_one_voltage_calc * stage_two_voltage_calc;

    // Equation taken from data sheet https://sensing.honeywell.com/honeywell-sensing-trustability-ssc-series-standard-accuracy-board-mount-pressure-sensors-50099533-a-en.pdf
    // More information about these calculations in the README.md
    double constant_C = (max_psi - min_psi) / (0.8 * VOLTAGE_SUPPLY);
    constant_A = constant_C * voltage_step;
    constant_B = 0.1 * VOLTAGE_SUPPLY * constant_C - min_psi;

    // Set zeroed value to 0
    zero_value = pressure_offset_adc_counts;
}

double PressureSensor::get_pressure(Units_pressure units, bool zero)
{
    int analog_val;
    double pressure_applied;

    // Get analog value, if after zeroing the value is less than 0 then set to zero.
    if (zero) {
        analog_val = analogRead(analog_pin) - zero_value;
        if (analog_val < 0) {
            analog_val = 0;
        }
    }
    else {
        analog_val = analogRead(analog_pin);
    }

    // Calculate pressure from constants in the constructor
    // More information about these calculations in the README.md
    pressure_applied = (analog_val * constant_A) - constant_B;

    if (units != Units_pressure::psi) {
        determine_units_pressure(pressure_applied, units);
    }

    return pressure_applied;
}

double PressureSensor::get_flow(Units_flow units, bool zero, Order_type order)
{
    double flow = 0;
    double x = get_pressure(Units_pressure::mbar, zero);

    if (order == Order_type::first) {
        flow = COEF_A_1ST_ORDER * x;
    }
    else if (order == Order_type::second) {
        flow = COEF_A_2ND_ORDER * pow(x, 2) + COEF_B_2ND_ORDER * x;
    }
    else if (order == Order_type::third) {
        flow = COEF_A_3RD_ORDER * pow(x, 3) + COEF_B_3RD_ORDER * pow(x, 2) + COEF_C_3RD_ORDER * x;
    }

    if (units != Units_flow::lpm) {
        determine_units_flow(flow, units);
    }

    return flow;
}

void PressureSensor::reset_zero(int* zero_val)
{
    zero_value = *zero_val = 0;
}

void PressureSensor::calculate_zero(int* zero_val, Zero_type zero)
{
    int const average_samples = 100;

    if (zero != Zero_type::DONT_ZERO) {
        for (int i = 0; i < average_samples; i++) {
            *zero_val += analogRead(analog_pin);
        }

        *zero_val /= average_samples;

        if (zero == Zero_type::diff) {
            *zero_val -= diff_zero_resolution;
        }
        zero_value = *zero_val;
    }
}

void PressureSensor::determine_units_pressure(double& pressure, Units_pressure units)
{
    switch (units) {
    case Units_pressure::atm:
        pressure *= PSI_TO_ATM;
        break;
    case Units_pressure::bar:
        pressure *= PSI_TO_BAR;
        break;
    case Units_pressure::cmH20:
        pressure *= PSI_TO_CMH20;
        break;
    case Units_pressure::inH20:
        pressure *= PSI_TO_INH20;
        break;
    case Units_pressure::inHg:
        pressure *= PSI_TO_INHG;
        break;
    case Units_pressure::kPa:
        pressure *= PSI_TO_KPA;
        break;
    case Units_pressure::mbar:
        pressure *= PSI_TO_MBAR;
        break;
    case Units_pressure::mmHg:
        pressure *= PSI_TO_MMHG;
        break;
    default:// return psi
        pressure = pressure;
        break;
    }
}

void PressureSensor::determine_units_flow(double& flow, Units_flow units)
{
    switch (units) {
    case Units_flow::lps:
        flow *= LPM_TO_LPS;
        break;
    case Units_flow::mlpm:
        flow *= LPM_TO_MLPM;
        break;
    case Units_flow::mlps:
        flow *= LPM_TO_MLPS;
        break;
    case Units_flow::cfm:
        flow *= LPM_TO_CFM;
        break;
    default:// return lpm
        flow = flow;
        break;
    }
}

#if ENABLE_TEST_PRESSURE_SENSORS
void PressureSensor::test_calculate_zero(Zero_type zero)
{
    int const average_samples = 100;
    zero_value = 0;

    if (zero != Zero_type::DONT_ZERO) {
        for (int i = 0; i < average_samples; i++) {
            zero_value += analogRead(analog_pin);
        }

        zero_value = zero_value / average_samples;

        if (zero == Zero_type::diff) {
            zero_value -= diff_zero_resolution;
        }
    }
}
#endif
