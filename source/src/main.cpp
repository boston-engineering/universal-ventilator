#include "display/test_display.h"
#include "display/TftDisplay.h"
#include "../config/uvent_conf.h"

TftDisplay tft_display = {TFT_CS, TFT_RST, TOUCH_INT, TOUCH_RST};

void setup()
{
    Serial.begin(SERIAL_BAUD_RATE);

    if (!tft_display.init()) {
        while (1);
    }

#if ENABLE_TEST_DISPLAY
    setup_test_display();
#endif
}

void loop()
{
    tft_display.update();
    delay(5);

#if ENABLE_TEST_DISPLAY
    update_test_display();
#endif
}
