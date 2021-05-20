#include "display/test_display.h"
#include "TftDisplay.h"

#define ENABLE_TEST_DISPLAY 1

#define TOUCH_INT 19
#define TOUCH_RST 2
#define TFT_CS 4
#define TFT_RST 3

TftDisplay tft_display = {TFT_CS, TFT_RST, TOUCH_INT, TOUCH_RST};

void setup()
{
    Serial.begin(115200);

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
