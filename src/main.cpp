#include "memtest.h"
#include "TftDisplay.h"

#define TFT_INT 18
#define TFT_CS 4
#define TFT_RST 3
#define PRINT_MEM_DELAY 150000

void lv_example_style(void);

bool hasTimeElapsed(uint32_t);
uint32_t timeCount = 0;

TftDisplay tft_display = {TFT_CS, TFT_RST};

bool hasTimeElapsed(uint32_t n) {
    uint32_t ms = millis();
    bool result = ms - timeCount >= n;

    if (ms < timeCount || result) {
        timeCount = ms;
        return true;
    }

    return false;
}



void setup() {
    Serial.begin(115200);

    if (!tft_display.init()) {
        while (1);
    }

    printMem();

    lv_example_style();
}

/**
 * Using the text style properties
 */
void lv_example_style(void) {
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
    lv_obj_t *obj = lv_label_create(lv_scr_act());
    lv_obj_add_style(obj, &style, 0);
    lv_label_set_text(obj, "Text of\n"
                           "a label");

    lv_obj_center(obj);
}

void loop() {
    tft_display.update();
    delay(5);
    if(hasTimeElapsed(PRINT_MEM_DELAY)) {
        printMem();
    }
}
