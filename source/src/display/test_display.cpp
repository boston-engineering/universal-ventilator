#include <utilities/memtest.h>
#include "test_display.h"

uint16_t chart_indexes[] = {(uint16_t)(WAVE_LEN * .25), (uint16_t)(WAVE_LEN * .75)};
uint32_t time_counts[] = {0, 0};

lv_obj_t* chart1 = nullptr;
lv_obj_t* chart2 = nullptr;
lv_obj_t* cont_row = nullptr;
lv_obj_t* cont_col = nullptr;
lv_obj_t* touch_cont = nullptr;

lv_chart_series_t* ser1 = nullptr;
lv_chart_series_t* ser2 = nullptr;

void setup_test_display()
{

    memset(time_counts, 0, sizeof(uint32_t) * 2);

    printMem();

    //lv_example_style();
    //create_lv_chart(&chart1, &ser1);
    //create_lv_chart(&chart2, &ser2);
    lv_flex_test();
}

void update_test_display()
{
    if (has_time_elapsed(time_counts, PRINT_MEM_DELAY)) {
        printMem();
    }

    if (has_time_elapsed(&time_counts[1], CHART_UPDATE_DELAY)) {
        if (chart1 && ser1)
            lv_chart_set_next_value(chart1, ser1, next_sin_val(0));
    }
}

bool has_time_elapsed(uint32_t* timer_ptr, uint32_t n)
{
    uint32_t ms = millis();
    bool result = ms-*timer_ptr >= n;

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

void lv_flex_test()
{
    /*Create a container with ROW flex direction*/
    cont_row = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont_row, 600, 75);
    lv_obj_align(cont_row, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_flex_flow(cont_row, LV_FLEX_FLOW_ROW);

    /*Create a container with COLUMN flex direction*/
    cont_col = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont_col, 300, 250);
    lv_obj_align_to(cont_col, cont_row, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);

    uint32_t i;
    for (i = 0; i < 10; i++) {
        lv_obj_t* obj;
        lv_obj_t* label;

        /*Add items to the row*/
        obj = lv_btn_create(cont_row);
        lv_obj_set_size(obj, 100, LV_PCT(100));

        label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "Item: %d", i);
        lv_obj_center(label);

        /*Add items to the column*/
        obj = lv_btn_create(cont_col);
        lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);

        label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "Item: %d", i);
        lv_obj_center(label);
    }

    touch_cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(touch_cont, 300, 100);
    lv_obj_align_to(touch_cont, cont_col, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_set_flex_flow(touch_cont, LV_FLEX_FLOW_COLUMN);
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

void basic_layout_test()
{
    /*Create a container with COLUMN flex direction*/
    lv_obj_t* label_col;
    label_col = lv_obj_create(lv_scr_act());
    lv_obj_set_size(label_col, 150, 480);
    lv_obj_align(label_col, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_flex_flow(label_col, LV_FLEX_FLOW_COLUMN);
}

void add_observed_expected_reading()
{

}