#include "memtest.h"
#include "logging.h"
#include "TftDisplay.h"
#include "../.pio/libdeps/due/lvgl/examples/lv_examples.h"
#include "waves.h"

#define TOUCH_INT 19
#define TOUCH_RST 2
#define TFT_CS 4
#define TFT_RST 3
#define PRINT_MEM_DELAY 150000
#define CHART_UPDATE_DELAY 1000
#define TIMER_COUNTS    3

void create_lv_chart(lv_obj_t** chart, lv_chart_series_t** series);
void lv_example_style();
uint8_t next_sin_val(int i);
bool hasTimeElapsed(uint32_t*, uint32_t);

uint32_t timeCounts[TIMER_COUNTS];

uint16_t chart_indexes[] = {(uint16_t) (WAVE_LEN * .25), (uint16_t) (WAVE_LEN * .75)};

lv_obj_t* chart1;

lv_obj_t* chart2;

lv_chart_series_t* ser1;

lv_chart_series_t* ser2;

TftDisplay tft_display = {TFT_CS, TFT_RST, TOUCH_INT, TOUCH_RST};

bool hasTimeElapsed(uint32_t* timer_ptr, uint32_t n)
{
    uint32_t ms = millis();
    bool result = ms - *timer_ptr >= n;

    if (ms < *timer_ptr || result) {
        *timer_ptr = ms;
        return true;
    }

    return false;
}

uint8_t next_sin_val(int i)
{
    if (chart_indexes[i] >= WAVE_LEN) {
        chart_indexes[i] = 0;
    }
    return sin_wave_static[chart_indexes[i]++];
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

    if (!tft_display.init()) {
        while (1);
    }

    memset(timeCounts, 0, sizeof(uint32_t) * 2);
    printMem();

    //lv_example_style();
    create_lv_chart(&chart1, &ser1);
    //create_lv_chart(&chart2, &ser2);
}

void create_lv_chart(lv_obj_t** chart, lv_chart_series_t** series)
{
    /*Create a chart*/
    *chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(*chart, 460, 100);
    lv_obj_center(*chart);
    lv_chart_set_type(*chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/
    lv_chart_set_range(*chart, LV_CHART_AXIS_PRIMARY_Y, 0, 256);

    /*Add data series*/
    *series = lv_chart_add_series(*chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_point_count(*chart, WAVE_LEN * .75);
    lv_chart_set_update_mode(*chart, LV_CHART_UPDATE_MODE_SHIFT);

    for (unsigned char i : sin_wave_static) {
        lv_chart_set_next_value(*chart, *series, i);
    }
    for (int i = 0; i < (WAVE_LEN * .25); i++) {
        lv_chart_set_next_value(*chart, *series, sin_wave_static[i]);
    }

    lv_chart_refresh(*chart); /*Required after direct set*/
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
    delay(5);
    if(hasTimeElapsed(timeCounts, PRINT_MEM_DELAY)) {
        printMem();
    }

    if (hasTimeElapsed(&timeCounts[1], CHART_UPDATE_DELAY)) {
        lv_chart_set_next_value(chart1, ser1, next_sin_val(0));
    }
}
