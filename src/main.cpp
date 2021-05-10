
#include "SPI.h"
#include "Wire.h"
#include "Adafruit_I2CDevice.h"
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"
#include "lvgl.h"
#include "memtest.h"

#define TFT_INT 18
#define TFT_CS 4
#define TFT_RST 3
#define PRINT_MEM_DELAY 60000

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480
#define BUFFER_SIZE SCREEN_WIDTH * 10

lv_disp_draw_buf_t screen_buffer;
lv_disp_drv_t display_driver;
lv_color_t pixel_buffer[BUFFER_SIZE];

void lv_example_style(void);
bool hasTimeElapsed(uint32_t);

uint32_t timeCount = 0;

Adafruit_RA8875 tft = Adafruit_RA8875(TFT_CS, TFT_RST);

bool hasTimeElapsed(uint32_t n) {
    uint32_t ms = millis();
    bool result = ms - timeCount >= n;

    if(ms < timeCount || result) {
        timeCount = ms;
        return true;
    }

    return false;
}


void flushDisplay(struct _lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    auto width = lv_area_get_width(area);
    auto height = lv_area_get_height(area);

    for(int16_t y = 0; y < height; y++) {
        lv_color_t* pixel_start = color_p + (width * y);
        tft.drawPixels((uint16_t*) pixel_start, width, area->x1, area->y1 + y);
    }

    lv_disp_flush_ready(disp_drv);
}


void setup() {
    Serial.begin(115200);
    Serial.println("Initializing...");

    // Initialize LCD
    tft.begin(RA8875_800x480);

    Serial.println("Finished Display Init");
    tft.displayOn(true);
    tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
    tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
    tft.PWM1out(255);

    Serial.println("Finished PWM Init");

    tft.fillScreen(RA8875_RED);

    // Start LVGL
    printMem();
    Serial.println("Starting LVGL With stuff");
    lv_init();
    Serial.println("Finished LV INIT");

    printMem();

    // Fill the display buffer
    lv_disp_draw_buf_init(&screen_buffer, pixel_buffer, NULL, BUFFER_SIZE);
    screen_buffer.buf1    = pixel_buffer;
    screen_buffer.buf2    = nullptr;
    screen_buffer.buf_act = screen_buffer.buf1;
    screen_buffer.size    = BUFFER_SIZE;

    /*Initialize the display*/
    lv_disp_drv_init(&display_driver);
    /* Set Resolution */
    display_driver.hor_res = SCREEN_WIDTH;
    display_driver.ver_res = SCREEN_HEIGHT;
    display_driver.flush_cb = flushDisplay;
    display_driver.draw_buf = &screen_buffer;
    lv_disp_drv_register(&display_driver);

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
    lv_obj_t * obj = lv_label_create(lv_scr_act());
    lv_obj_add_style(obj, &style, 0);
    lv_label_set_text(obj, "Text of\n"
                           "a label");

    lv_obj_center(obj);
}

void loop() {
    lv_task_handler();
    delay(5);
    if(hasTimeElapsed(PRINT_MEM_DELAY)) {
        printMem();
    }
}
