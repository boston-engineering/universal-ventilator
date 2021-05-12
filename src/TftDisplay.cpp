//
// Created by Andrew Hoener on 5/10/21.
//
#include "TftDisplay.h"

void wrapped_flush_display(struct _lv_disp_drv_t *lvDispDrv, const lv_area_t *area, lv_color_t *color_p) {
    static_cast<TftDisplay *>(lvDispDrv->user_data)->flush_display(lvDispDrv, area, color_p);
}

bool wrapped_read_inputs(struct _lv_indev_drv_t* lv_indev_drv, lv_indev_data_t* data)
{
    static_cast<TftDisplay*>(lv_indev_drv->user_data)->read_inputs(lv_indev_drv, data);
    return false;
}

TftDisplay::TftDisplay(uint8_t cs_pin, uint8_t rst_pin, uint8_t touch_int_pin, uint8_t touch_rst_pin)
{

    tft_display = Adafruit_RA8875(cs_pin, rst_pin);
    touch_driver = TftTouch(touch_int_pin, touch_rst_pin);
}

bool TftDisplay::init()
{

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

    Serial.println("Display connected, starting touchscreen capture...");

    //pinMode(interrupt_pin, INPUT);
    //digitalWrite(interrupt_pin, HIGH);
    touch_driver.init();

    Serial.println("Touchscreen init finished, starting LVGL...");
    lv_init();

    lv_disp_draw_buf_init(&lv_screen_buffer, pixel_buffer, NULL, BUFFER_SIZE);
    lv_screen_buffer.buf1 = pixel_buffer;
    lv_screen_buffer.buf2 = nullptr;
    lv_screen_buffer.buf_act = lv_screen_buffer.buf1;
    lv_screen_buffer.size = BUFFER_SIZE;

    lv_disp_drv_init(&lv_display_driver);                 // Initialize the display
    lv_display_driver.user_data = this;                   // Save `this` for callback functions
    lv_display_driver.hor_res = SCREEN_WIDTH;             // Set Resolution
    lv_display_driver.ver_res = SCREEN_HEIGHT;
    lv_display_driver.flush_cb = wrapped_flush_display;     // Callback for display writing
    lv_display_driver.draw_buf = &lv_screen_buffer;
    lv_disp_drv_register(&lv_display_driver);             // register Display

    Serial.println("Finished registering lvgl display and drivers");
    Serial.println("Registering touchscreen...");

    lv_indev_drv_init(&lv_input_driver);
    lv_input_driver.type = LV_INDEV_TYPE_POINTER;
    lv_input_driver.user_data = this;
    lv_input_driver.read_cb = wrapped_read_inputs;
    lv_indev_drv_register(&lv_input_driver);

    return true;
}

void TftDisplay::flush_display(struct _lv_disp_drv_t* lv_disp_drv, const lv_area_t* area, lv_color_t* color_p)
{

    lv_coord_t width = lv_area_get_width(area);
    lv_coord_t height = lv_area_get_height(area);

    /**
     * The way the RA8875 works, you can block write, but only in a straight line.
     * Attempting to block write the entire area will result in it being spread straight across the current line,
     * instead of in its intended position. Block writing line by line should fix this issue.
     */
    for (int16_t y = 0; y<height; y++) {
        lv_color_t* pixel_start = color_p+(width*y);
        tft_display.drawPixels((uint16_t*) pixel_start, width, area->x1, area->y1+y);
    }

    lv_disp_flush_ready(lv_disp_drv);
}

void TftDisplay::read_inputs(struct _lv_indev_drv_t* lvIndevDrv, lv_indev_data_t* data)
{
    // Serial.println("Reading Input");
    // TODO find out if checking to see if touch1 == old_touch1 is viable without slowing down the entire process.
    bool isPressed = touch_driver.touched();
    if (isPressed) {
        touch_driver.read_touch_registers(1);

        TsData touch_data = *(touch_driver.new_touch_data);
        TsData last_data = *(touch_driver.last_touch_data);

        if (touch_data == last_data) {
            return;
        }

        data->point.x = touch_data.x;
        data->point.y = touch_data.y;
        switch (touch_data.state) {
            case PRESSED:
                tft_display.fillCircle(touch_data.x, touch_data.y, 4, RA8875_RED);
                data->state = LV_INDEV_STATE_PR;
                break;
            case RELEASED:
                data->state = LV_INDEV_STATE_REL;
                break;
            case HELD:      //
                tft_display.fillCircle(touch_data.x, touch_data.y, 4, RA8875_RED);
                break;
            case RESERVED:  // Fall-through cases
            case IDLE:      // Fall-through cases
            default:        //
                data->state = LV_INDEV_STATE_REL;
                break;
        }

        memcpy(touch_driver.last_touch_data, touch_driver.new_touch_data, sizeof(TsData) * NUM_TOUCH_REGISTERS);
    }
}

void TftDisplay::update()
{
    lv_task_handler();
}