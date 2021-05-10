//
// Created by Andrew Hoener on 5/10/21.
//
#include "TFT_Display.h"

void wrappedFlushDisplay(struct _lv_disp_drv_t *lvDispDrv, const lv_area_t *area, lv_color_t *color_p) {
    static_cast<TFT_Display *>(lvDispDrv->user_data)->flushDisplay(lvDispDrv, area, color_p);
}

TFT_Display::TFT_Display(uint8_t csPin, uint8_t rstPin) {
    tft_display = Adafruit_RA8875(csPin, rstPin);
}

bool TFT_Display::init() {

    Serial.println("Initializing...");
    if (!tft_display.begin(RA8875_800x480)) {
        Serial.println("Failed to init display, RA8875 did not setup correctly");
        return false;
    }

    tft_display.displayOn(true);
    tft_display.GPIOX(true);                                // Enable TFT - display enable tied to GPIOX
    tft_display.PWM1config(true, RA8875_PWM_CLK_DIV1024);   // PWM output for backlight
    tft_display.PWM1out(255);

    tft_display.fillScreen(RA8875_GREEN);

    // TODO enable touch mode

    Serial.println("Display init finished, starting LVGL...");
    lv_init();

    lv_disp_draw_buf_init(&_screen_buffer, _pixel_buffer, NULL, BUFFER_SIZE);
    _screen_buffer.buf1 = _pixel_buffer;
    _screen_buffer.buf2 = nullptr;
    _screen_buffer.buf_act = _screen_buffer.buf1;
    _screen_buffer.size = BUFFER_SIZE;

    lv_disp_drv_init(&_display_driver);                 // Initialize the display
    _display_driver.user_data = this;                   // Save `this` for callback functions
    _display_driver.hor_res = SCREEN_WIDTH;             // Set Resolution
    _display_driver.ver_res = SCREEN_HEIGHT;
    _display_driver.flush_cb = wrappedFlushDisplay;     // Callback for display writing
    _display_driver.draw_buf = &_screen_buffer;
    lv_disp_drv_register(&_display_driver);             // register Display

    return true;
}

void TFT_Display::flushDisplay(struct _lv_disp_drv_t *lvDispDrv, const lv_area_t *area, lv_color_t *color_p) {

    lv_coord_t width = lv_area_get_width(area);
    lv_coord_t height = lv_area_get_height(area);

    /**
     * The way the RA8875 works, you can block write, but only in a straight line.
     * Attempting to block write the entire area will result in it being spread straight across the current line,
     * instead of in its intended position. Block writing line by line should fix this issue.
     */
    for (int16_t y = 0; y < height; y++) {
        lv_color_t *pixel_start = color_p + (width * y);
        tft_display.drawPixels((uint16_t *) pixel_start, width, area->x1, area->y1 + y);
    }

    lv_disp_flush_ready(lvDispDrv);
}

void TFT_Display::update() {
    lv_task_handler();
}

