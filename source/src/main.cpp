#include "display/test_display.h"
#include "display/TftDisplay.h"
#include "utilities/parser.h"
#include "utilities/command.h"
#include "../config/uvent_conf.h"
#include "display/main_display.h"

TftDisplay tft_display = {TFT_CS, TFT_RST, TOUCH_INT, TOUCH_RST};
Parser parser;

void setup()
{
    Serial.begin(SERIAL_BAUD_RATE);

    if (!tft_display.init()) {
        while (1);
    }

#if ENABLE_TEST_DISPLAY
    setup_test_display();
#else
    init_main_display();
#endif

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
