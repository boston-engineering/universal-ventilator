#include <DueTimer.h>
#include "display/test_display.h"
#include "display/TftDisplay.h"
#include "utilities/parser.h"
#include "utilities/command.h"
#include "controls/machine.h"
#include "../config/uvent_conf.h"

TftDisplay tft_display = {TFT_CS, TFT_RST, TOUCH_INT, TOUCH_RST};
Parser parser;
Machine machine;

void control_handler()
{
    static bool ledOn = false;

    // LED to visually show state machine is running.
    digitalWrite(DEBUG_LED, ledOn);

    // Toggle the LED.
    ledOn = !ledOn;
}

void setup()
{
    Serial.begin(SERIAL_BAUD_RATE);

    // Debug led setup
    pinMode(DEBUG_LED, OUTPUT);

    /* Setup a timer and a function handler to run
     * the timer is triggered.
     */
    Timer0.attachInterrupt(control_handler);
    Timer0.start(CONTROL_HANDLER_PERIOD_US);

    if (!tft_display.init()) {
        while (1);
    }

#if ENABLE_TEST_DISPLAY
    setup_test_display();
#endif

    // Initialize the state machine
    machine.setup();

    // Initialize the parser, with the command array and command array size
    parser.init(command_get_array(), command_get_array_size());
}

void loop()
{
    tft_display.update();
    delay(5);

#if ENABLE_TEST_DISPLAY
    update_test_display();
#endif

    // Service the command parser
    parser.service();
}
