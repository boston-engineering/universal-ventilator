#include "memtest.h"
#include "logging.h"
#include "TftDisplay.h"

#define TOUCH_INT 19
#define TOUCH_RST 2
#define TFT_CS 4
#define TFT_RST 3
#define PRINT_MEM_DELAY 150000

void lv_example_style();
bool hasTimeElapsed(uint32_t);

uint32_t timeCount = 0;

TftDisplay tft_display = {TFT_CS, TFT_RST, TOUCH_INT, TOUCH_RST};

bool hasTimeElapsed(uint32_t n)
{
    uint32_t ms = millis();
    bool result = ms - timeCount >= n;

    if (ms < timeCount || result) {
        timeCount = ms;
        return true;
    }

    return false;
}

void serial_printf(const char* str, ...)
{
    char buf[256];
    va_list args;
    va_start(args, str);
    vsnprintf(buf, 127, str, args);
    va_end(args);
    Serial.print(buf);
}

void setup()
{
    Serial.begin(115200);

#ifdef SAM3X8
    Serial.println("Arduino Due 3x8e");
#endif

    if (!tft_display.init()) {
        while (1);
    }

    printMem();

    lv_example_style();
}

/**
 * Using the text style properties
 */
void lv_example_style(void)
{
    static lv_style_t style;
    lv_style_init(&style);

    lv_style_set_radius(&style, 5);
    lv_style_set_bg_opa(&style, LV_OPA_COVER);
    lv_style_set_bg_color(&style, lv_palette_lighten(LV_PALETTE_GREY, 3));
    lv_style_set_border_width(&style, 2);
    lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_pad_all(&style, 10);

    lv_style_set_text_color(&style, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_text_letter_space(&style, 5);
    lv_style_set_text_line_space(&style, 20);
    lv_style_set_text_decor(&style, LV_TEXT_DECOR_UNDERLINE);

    /*Create an object with the new style*/
    lv_obj_t* obj = lv_label_create(lv_scr_act());
    lv_obj_add_style(obj, &style, 0);
    lv_label_set_text(obj, "Text of\n"
                           "a label");

    lv_obj_center(obj);
}

void loop()
{
    tft_display.update();
    delay(2);
    if(hasTimeElapsed(PRINT_MEM_DELAY)) {
        printMem();
    }
}
