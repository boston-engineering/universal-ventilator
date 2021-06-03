#ifndef UVENT_CONF_H
#define UVENT_CONF_H

// Screen Dimensions
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480

// Debug LED
#define DEBUG_LED   13

// Display and touch pins
#define TOUCH_INT   19
#define TOUCH_RST   2
#define TFT_CS      4
#define TFT_RST     3

// Display testing
#define ENABLE_TEST_DISPLAY 0

// Draw borders around flexbox components for debugging
#define DEBUG_BORDER_READOUTS 0

// Serial baud rate
#define SERIAL_BAUD_RATE 115200

#define SPI_CLK_SPEED 41000000L

// Control handler  period in microsec.
#define CONTROL_HANDLER_PERIOD_US 100000

#endif