#ifndef TEST_PRESSURE_SENSORS
#define TEST_PRESSURE_SENSORS

#include "../config/uvent_conf.h"
#include "pressure_sensor.h"

// Output gauge pressure. Pins are set in uvent_conf.h
void output_gauge_pressure_test(void);

// Output differential pressure. Pins are set in uvent_conf.h
void output_diff_pressure_test(void);

// Output gauge and differential pressure. Pins are set in uvent_conf.h
void output_pressure_test(void);

#endif//TEST_PRESSURE_SENSORS
