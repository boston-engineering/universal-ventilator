//
// Created by Andrew Hoener on 5/10/21.
//

#ifndef UVENT_TFT_DISPLAY_H
#define UVENT_TFT_DISPLAY_H

#include "SPI.h"
#include "Wire.h"
#include "Adafruit_I2CDevice.h"
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"
#include "lvgl.h"

// This is calibration data for the raw touch data to the screen coordinates
// In the future, we should calibrate the display rather than hardcode and store the values in eeprom
/*
*                 TS_MAXX
*           --------------------
*           |                  |
*           |                  |
*           |                  |
*           |                  |
*           |                  |
*  TS_MINY  |                  |  TS_MAXY
*           |                  |
*           |                  |
*           |                  |
*           |                  |
*           |                  |
*           --------------------
*                 TS_MINX
*/

#define TS_MINX 300
#define TS_MINY 150
#define TS_MAXX 3700
#define TS_MAXY 3900

#define MINPRESSURE 40
#define MAXPRESSURE 1000

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480
#define BUFFER_SIZE SCREEN_WIDTH * 10

void wrappedFlushDisplay(struct _lv_disp_drv_t *lvDispDrv, const lv_area_t *area, lv_color_t *color_p);

/**
* LVGL Callback to handle logging on different platforms.
* @param buf The char sequence to be printed
*/
void lvLogFunction(const char *buf);

class TFT_Display {
public:
    TFT_Display(uint8_t csPin, uint8_t rstPin);

    ~TFT_Display() = default;

    /**
     * Starts the RA8875 display, then LVGL, then registers handlers.
     *
     * @return True if successful, False otherwise.
     */
    bool init();

    /**
     * <p>
     * Runs update functions and other interval-based code <br>
     * Should be run on a timer or Arduino <code>loop()</code> function. Does not set its own delay.
     * </p>
     */
    void update();

/**
 * <p>
 * Registered with LVGL on init.
 * Will be periodically called with block updates to the display, leaving implementation up to the user.
 * Uses Adafruit RA8875 to write out lines
 * </p>
 * @param lvDispDrv The LVGL display driver, written to when done to signal finished.
 * @param area      The area that needs updating (x, y, x1, y1)
 * @param color_p   An array of colors, either in RGB or uin16_t. Requires <code>COLOR_DEPTH=16</code> in LVGL config.
 */
    void flushDisplay(struct _lv_disp_drv_t *lvDispDrv, const lv_area_t *area, lv_color_t *color_p);

protected:
    Adafruit_RA8875 tft_display = Adafruit_RA8875(0, 0);

private:
    lv_disp_drv_t _display_driver{};
    lv_disp_draw_buf_t _screen_buffer{};
    lv_color_t _pixel_buffer[BUFFER_SIZE]{};
};

#endif //UVENT_TFT_DISPLAY_H
