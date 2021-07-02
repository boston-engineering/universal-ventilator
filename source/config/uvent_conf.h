#ifndef UVENT_CONF_H
#define UVENT_CONF_H

// Screen Dimensions
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480

// Debug LED
#define DEBUG_LED 13

// Display and touch pins
#define TOUCH_INT 23
#define TOUCH_RST 22
#define TFT_CS 10
#define TFT_RST 26

// Display testing
#define ENABLE_TEST_DISPLAY 0

// Draw borders around flexbox components for debugging
#define DEBUG_BORDER_READOUTS 0
#define DEBUG_BORDER_CONTROLS 0

// Pressure sensor testing
#define ENABLE_TEST_PRESSURE_SENSORS 0

// Settings address on external eeprom
#define EXT_EEPROM_SETTINGS_LOC 100

// CRC address on external eeprom
#define EXT_EEPROM_CRC_LOC 0

// Test EEPROM
#define ENABLE_TEST_EEPROM 0

// Test EEPROM Memory positions
#define EEPROM_TEST_1_MEM_1 63000
#define EEPROM_TEST_1_MEM_2 63010
#define EEPROM_TEST_1_MEM_3 63020
#define EEPROM_TEST_1_MEM_4 63040
#define EEPROM_TEST_1_MEM_5 63050
#define EEPROM_TEST_2_MEM_1 63100

// Serial baud rate
#define SERIAL_BAUD_RATE 115200

#define SPI_CLK_SPEED 22000000L

// Control handler  period in microsec.
#define CONTROL_HANDLER_PERIOD_US 20000

// Actuator handler period in microsec.
#define ACTUATOR_HANDLER_PERIOD_US 50

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

// Stepper: Direction pin
#define STEPPER_DIRECTION_PIN 8

// Stepper: Step pin
#define STEPPER_STEP_PIN 9

// Stepper: Maximum steps per second
#define STEPPER_MAX_STEPS_PER_SECOND 400

// Stepper: Homing speed steps/sec
#define STEPPER_HOMING_SPEED_IN_STEPS_SEC 100

// Stepper: Angle per step in degrees
#define STEPPER_ANGLE_DEG_PER_STEP 1.8

/* Stepper step configuration
 * 1 = FULL_STEP, 200 PPR, pulses per revolution
 * 2 = HALF_STEP, 400 PPR, pulses per revolution
 * 5 = 5_MICRO_STEP, 1000 PPR, pulses per revolution
 * 10 = 10_MICRO_STEP, 2000 PPR, pulses per revolution
 */
#define STEPPER_MULTIPLIER_MODE 1

// Stepper: No. of steps in one revolution
#define STEPPER_STEPS_PER_REV 360 / (STEPPER_ANGLE_DEG_PER_STEP / STEPPER_MULTIPLIER_MODE)

// Stepper
// Gear Ratio
#define TEETH_ON_PINION 14.0
#define TEETH_ON_TIMING_PULLEY 60.0
#define GEAR_RATIO TEETH_ON_TIMING_PULLEY / TEETH_ON_PINION

// One revolution of Timing pulley in steps
#define TIMING_PULLEY_STEPS_PER_REV GEAR_RATIO* STEPPER_STEPS_PER_REV

// Convert from degrees to steps
#define TIMING_PULLEY_DEGREES_TO_STEPS TIMING_PULLEY_STEPS_PER_REV / 360.0

// DEBUG FLAGS
#define DEBUG_STEPPER 0

// EEPROM address
#define EEPROM_ADDRESS 0x50

#endif