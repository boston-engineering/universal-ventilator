#ifndef UVENT_CONF_H
#define UVENT_CONF_H

// Screen Dimensions
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480

// Debug LED
#define DEBUG_LED 13

// Display and touch pins
#define TOUCH_INT 19
#define TOUCH_RST 2
#define TFT_CS 4
#define TFT_RST 3

// Display testing
#define ENABLE_TEST_DISPLAY 0

// Draw borders around flexbox components for debugging
#define DEBUG_BORDER_READOUTS 0
#define DEBUG_BORDER_CONTROLS 0

// Eeprom testing
#define ENABLE_TEST_PRESSURE_SENSORS 0

// Serial baud rate
#define SERIAL_BAUD_RATE 115200

#define SPI_CLK_SPEED 41000000L

// Control handler  period in microsec.
#define CONTROL_HANDLER_PERIOD_US 100000

// Pressure Sensor pins
#define PRESSURE_GAUGE_PIN 0
#define PRESSURE_DIFF_PIN 1

// Pressure Sensor resistor resistance in ohms
#define RESISTANCE_1 100000
#define RESISTANCE_2 196000

// Gauge Pressure sensor max and min pressure values, units psi
// Gauge unit number: 001PG
// data sheet link: https://sensing.honeywell.com/honeywell-sensing-trustability-ssc-series-standard-accuracy-board-mount-pressure-sensors-50099533-a-en.pdf
#define MAX_GAUGE_PRESSURE 1
#define MIN_GAUGE_PRESSURE 0

// Differential Pressure sensor max and min pressure values, units psi
// Differential unit number: 001PD
// data sheet link: https://sensing.honeywell.com/honeywell-sensing-trustability-ssc-series-standard-accuracy-board-mount-pressure-sensors-50099533-a-en.pdf
#define MAX_DIFF_PRESSURE 1
#define MIN_DIFF_PRESSURE -1

#endif