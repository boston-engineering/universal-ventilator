#ifndef UVENT_TEST_DISPLAY_H
#define UVENT_TEST_DISPLAY_H

#include <lvgl.h>
#include "utilities/waves.h"

#define CHART_UPDATE_DELAY  1000
#define PRINT_MEM_DELAY     150000
#define TIMER_COUNTS        3

void setup_test_display();

void update_test_display();

void setPinState(uint32_t pin, uint32_t val);

void lv_flex_test();

void basic_layout_test();

void add_observed_expected_reading();

void create_lv_chart(lv_obj_t** chart, lv_chart_series_t** series);

void lv_example_style();

uint8_t next_sin_val(int i);

extern uint32_t time_counts[3];
extern uint16_t chart_indexes[];

extern lv_obj_t* chart1;
extern lv_obj_t* chart2;
extern lv_obj_t* cont_row;
extern lv_obj_t* cont_col;
extern lv_obj_t* touch_cont;

extern lv_chart_series_t* ser1;
extern lv_chart_series_t* ser2;

#endif //UVENT_TEST_DISPLAY_H
