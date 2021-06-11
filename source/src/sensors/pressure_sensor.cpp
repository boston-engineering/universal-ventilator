#include "pressure_sensor.h"

PressureSensor::PressureSensor(int analog_pin, int max_psi, int min_psi, int resistance_ohms_1, int resistance_ohms_2): analog_pin(analog_pin)
{
    // Set resolution to 12 bits, the default is 10
    analogReadResolution(ADC_RESOLUTION);
    int max_resolution_units = pow(2, ADC_RESOLUTION);

    // Calculate voltage step for voltage through circuit and across the resistors
    double stage_one_voltage_calc = voltage_adc_ref / max_resolution_units;
    double stage_two_voltage_calc = (double) (resistance_ohms_1 + resistance_ohms_2) / resistance_ohms_2;
    double voltage_step = stage_one_voltage_calc * stage_two_voltage_calc;

    // Equation taken from data sheet https://sensing.honeywell.com/honeywell-sensing-trustability-ssc-series-standard-accuracy-board-mount-pressure-sensors-50099533-a-en.pdf
    double constant_C = (max_psi - min_psi) / (0.8 * voltage_supply);
    constant_A = constant_C * voltage_step;
    constant_B = 0.1 * voltage_supply * constant_C - min_psi;
}

double PressureSensor::get_pressure(Units_pressure units)
{
    return calculate_pressure(units);
}

double PressureSensor::calculate_pressure(Units_pressure units)
{
    // analog value
    int analog_val = analogRead(analog_pin);

    // Calculate pressure from constants in the constructor
    double pressure_applied = (analog_val * constant_A) - constant_B;
    return determine_units(pressure_applied, units);
}

double PressureSensor::determine_units(double pressure, Units_pressure units)
{
    switch (units) {
    case Units_pressure::psi:
        return pressure;
    case Units_pressure::atm:
        return pressure * PSI_TO_ATM;
    case Units_pressure::bar:
        return pressure * PSI_TO_BAR;
    case Units_pressure::cmH20:
        return pressure * PSI_TO_CMH20;
    case Units_pressure::inH20:
        return pressure * PSI_TO_INH20;
    case Units_pressure::inHg:
        return pressure * PSI_TO_INHG;
    case Units_pressure::kPa:
        return pressure * PSI_TO_KPA;
    case Units_pressure::mbar:
        return pressure * PSI_TO_MBAR;
    case Units_pressure::mmHg:
        return pressure * PSI_TO_MMHG;
    default:// return psi
        return pressure;
    }
}
