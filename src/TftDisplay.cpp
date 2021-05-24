//
// Created by Andrew Hoener on 5/10/21.
//
#include "TftDisplay.h"

void wrappedFlushDisplay(struct _lv_disp_drv_t *lvDispDrv, const lv_area_t *area, lv_color_t *color_p) {
    static_cast<TftDisplay *>(lvDispDrv->user_data)->flushDisplay(lvDispDrv, area, color_p);
}

TftDisplay::TftDisplay(uint8_t csPin, uint8_t rstPin) {
    tft_display = Adafruit_RA8875(csPin, rstPin);
}

bool TftDisplay::init() {

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

    lv_disp_draw_buf_init(&_screenBuffer, _pixelBuffer, NULL, BUFFER_SIZE);
    _screenBuffer.buf1 = _pixelBuffer;
    _screenBuffer.buf2 = nullptr;
    _screenBuffer.buf_act = _screenBuffer.buf1;
    _screenBuffer.size = BUFFER_SIZE;

    lv_disp_drv_init(&_displayDriver);                 // Initialize the display
    _displayDriver.user_data = this;                   // Save `this` for callback functions
    _displayDriver.hor_res = SCREEN_WIDTH;             // Set Resolution
    _displayDriver.ver_res = SCREEN_HEIGHT;
    _displayDriver.flush_cb = wrappedFlushDisplay;     // Callback for display writing
    _displayDriver.draw_buf = &_screenBuffer;
    lv_disp_drv_register(&_displayDriver);             // register Display

    Serial.println("Finished registering lvgl display and drivers");
    Serial.println("Registering touchscreen...");


    return true;
}

void TftDisplay::flushDisplay(struct _lv_disp_drv_t *lvDispDrv, const lv_area_t *area, lv_color_t *color_p) {

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

void TftDisplay::update() {
    lv_task_handler();
}

