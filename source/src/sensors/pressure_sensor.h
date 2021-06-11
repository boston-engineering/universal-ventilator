#ifndef PRESSURE_SENSOR
#define PRESSURE_SENSOR

#include "../config/uvent_conf.h"
#include <Arduino.h>

typedef enum class units_pressure {
    psi,
    mbar,
    bar,
    mmHg,
    inHg,
    cmH20,
    inH20,
    atm,
    kPa
} Units_pressure;

class PressureSensor {
public:
    // Arguments ->
    // analog_pin: pin on the arduino that the pressure sensor is connected to
    // max_psi: the maximum pressure of the sensor in units psi
    // min_psi: the minimum pressure of the sensor in units psi
    // resistance_ohms_1: the resistance of the first resistor in the circuit in units ohms
    // resistance_ohms_2: the resistance of the second resistor in the circuit in units ohms
    PressureSensor(int analog_pin, int max_psi, int min_psi, int resistance_ohms_1, int resistance_ohms_2);

    // Defintion: Returns the pressure from the gauge sensor.
    // Arguments ->
    // units: units of measurement requested, default is in psi
    double get_pressure(Units_pressure units = Units_pressure::psi);

    void read_flow(void);

private:
    // Conversion Table
    double const PSI_TO_MBAR = 68.9476;
    double const PSI_TO_BAR = 0.0689476;
    double const PSI_TO_MMHG = 51.7149;
    double const PSI_TO_INHG = 2.03602;
    double const PSI_TO_CMH20 = 70.307;
    double const PSI_TO_INH20 = 27.7076;
    double const PSI_TO_ATM = 0.068046;
    double const PSI_TO_KPA = 6.89476;

    // Voltage Supply
    // Units: Volts
    double const voltage_supply = 5.0;
    double const voltage_adc_ref = 3.3;

    // Pin on the arduino connecting the sensor
    int analog_pin;

    // Calculations
    double constant_A;
    double constant_B;

    // Definition: Calculates the pressure from either the Gauge or Differential Sensor
    // Arguments ->
    // units: units of measurement requested
    double calculate_pressure(Units_pressure units);

    // Definition: Outputs the measured pressure in the users choosen units of measurement
    // Arguments ->
    // pressure: the pressure measured in units psi
    // units: units of measurement requested
    double determine_units(double pressure, Units_pressure units);
};

#endif// PRESSURE_SENSOR