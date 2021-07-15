#include <utilities/waves.h>
#include "main_display.h"
#include "global_components.h"

lv_obj_t* containers[DisplayContainer::CONTAINER_COUNT];
lv_style_t container_styles[DisplayContainer::CONTAINER_COUNT];
lv_style_t component_styles[ComponentType::COMPONENT_COUNT];
lv_obj_t* chart;

lv_color_t palette_color_1 = LV_COLOR_MAKE(109, 68, 197);
lv_color_t palette_color_2 = LV_COLOR_MAKE(59, 125, 185);
lv_color_t color_start_button = LV_COLOR_MAKE(49, 230, 133);
lv_color_t color_button_alert = LV_COLOR_MAKE(255, 172, 53);
lv_color_t color_text_disabled = LV_COLOR_MAKE(130, 130, 130);
lv_color_t color_gray = LV_COLOR_MAKE(248, 248, 248);

lv_point_t divider_1_points[] = {
        {200, 15},
        {200, 465}
};
lv_point_t divider_2_points[] = {
        {600, 0},
        {600, 480}
};

ButtonData button_type_data[ButtonType::BUTTON_TYPE_COUNT];

void init_styles()
{
    lv_style_t* style;
    // Style for main containers
    for (auto& i : container_styles) {
        style = &i;
        lv_style_init(style);
        lv_style_remove_prop(style, LV_STYLE_BG_COLOR);
        lv_style_set_border_width(style, 0 px);
        lv_style_set_border_color(style, lv_color_black());
        lv_style_set_outline_width(style, 0 px);
        lv_style_set_pad_all(style, 0 px);
        lv_style_set_radius(style, 0 px);
    }

    // Style for components
    for (auto& i : component_styles) {
        style = &i;
        lv_style_init(style);
        lv_style_remove_prop(style, LV_STYLE_BG_COLOR);
        lv_style_set_border_width(style, 0 px);
        lv_style_set_border_color(style, lv_color_black());
        lv_style_set_outline_width(style, 0 px);
        lv_style_set_pad_all(style, 0 px);
        lv_style_set_radius(style, 0 px);
    }
}

void add_dividers()
{
    lv_obj_t* line = lv_line_create(lv_scr_act());
    lv_obj_add_flag(line, LV_OBJ_FLAG_FLOATING);
    lv_line_set_points(line, divider_1_points, 2);
    lv_obj_add_style(line, STYLE_PTR_CM(DIVIDER), LV_PART_MAIN);

    line = lv_line_create(lv_scr_act());
    lv_obj_add_flag(line, LV_OBJ_FLAG_FLOATING);
    lv_line_set_points(line, divider_2_points, 2);
    lv_obj_add_style(line, STYLE_PTR_CM(DIVIDER), LV_PART_MAIN);
}

void add_chart()
{
    /*Create a chart*/
    chart = lv_chart_create(SCR_C(VISUAL_AREA_2));
    lv_obj_set_width(chart, LV_PCT(100));

    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 256);

    /*Add data series*/
#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnusedValue"
    lv_chart_series_t* series1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
#pragma clang diagnostic pop
    lv_chart_set_point_count(chart, WAVE_LEN * .75);
    lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);

    lv_chart_refresh(chart); /*Required after direct set*/
}

void update_chart()
{
    static int pos = WAVE_LEN * .75;
    lv_chart_series_t* series = lv_chart_get_series_next(chart, nullptr);
    if (++pos >= WAVE_LEN) {
        pos = 0;
    }
    lv_chart_set_next_value(chart, series, sin_wave_static[pos]);
    lv_chart_refresh(chart);
}