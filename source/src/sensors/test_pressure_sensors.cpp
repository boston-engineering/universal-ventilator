#include "test_pressure_sensors.h"

void output_gauge_pressure_test(void)
{
    PressureSensor sensors = {PRESSURE_GAUGE_PIN, MAX_GAUGE_PRESSURE, MIN_GAUGE_PRESSURE, RESISTANCE_1, RESISTANCE_2};

    Serial.print("Gauge Pressure: ");
    Serial.print(sensors.get_pressure(), 6);
    Serial.println("");

    delay(1000);
}

void output_diff_pressure_test(void)
{
    PressureSensor sensors = {PRESSURE_DIFF_PIN, MAX_DIFF_PRESSURE, MIN_DIFF_PRESSURE, RESISTANCE_1, RESISTANCE_2};

    Serial.print("Differential Pressure:");
    Serial.print(sensors.get_pressure(), 6);
    Serial.println("");

    delay(1000);
}

void output_pressure_test(void)
{
    PressureSensor gauge_sensor = {PRESSURE_GAUGE_PIN, MAX_GAUGE_PRESSURE, MIN_GAUGE_PRESSURE, RESISTANCE_1, RESISTANCE_2};
    PressureSensor diff_sensor = {PRESSURE_DIFF_PIN, MAX_DIFF_PRESSURE, MIN_DIFF_PRESSURE, RESISTANCE_1, RESISTANCE_2};

    Serial.print("Gauge Pressure:        ");
    Serial.println(gauge_sensor.get_pressure(), 6);

    Serial.print("Differential Pressure: ");
    Serial.println(diff_sensor.get_pressure(), 6);
    Serial.println("");
    Serial.println("------------------------");
    Serial.println("");

    delay(1000);
}