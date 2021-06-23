#include <utilities/waves.h>
#include "main_display.h"
#include "global_components.h"
#include "../../config/uvent_conf.h"

lv_obj_t* containers[DisplayContainer::CONTAINER_COUNT];
lv_style_t container_styles[DisplayContainer::CONTAINER_COUNT];
lv_style_t component_styles[ComponentType::COMPONENT_COUNT];
lv_obj_t* chart;

lv_point_t divider_1_points[] = {
        {200, 15},
        {200, 465}
};
lv_point_t divider_2_points[] = {
        {600, 0},
        {600, 480}
};

ButtonData button_type_data[ButtonType::BUTTON_TYPE_COUNT];

static void on_enable_disable_press(lv_event_t*);

void init_styles()
{
    lv_style_t* style;
    // Style for main containers
    for (auto& i : container_styles) {
        style = &i;
        lv_style_init(style);
        lv_style_remove_prop(style, LV_STYLE_BG_COLOR);
        lv_style_set_border_width(style, 0 px);
        lv_style_set_border_color(style, color_black);
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
        lv_style_set_border_color(style, color_black);
        lv_style_set_outline_width(style, 0 px);
        lv_style_set_pad_all(style, 0 px);
        lv_style_set_radius(style, 0 px);
    }
}

void populate_items()
{
    // FULL SCREEN
    add_dividers();

    // VISUAL_AREA_1
    //TODO remove me when I:E adjustable component is complete
    add_readout_item("I:E Ratio", "1:2", nullptr);

    // VISUAL_AREA_2
    add_chart();

    // VISUAL_AREA_3
    add_start_button();
    add_mute_button();

    // CONTROL_AREA_1
    //TODO remove me when I:E adjustable component is complete
    add_control_item("I:E Ratio", "1:2", nullptr);

    // CONTROL_AREA_2
    add_settings_button("Settings &\nConfig");

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

void add_dummy_items()
{
    uint32_t i;
    for (i = 0; i < 8; i++) {
        lv_obj_t* obj = lv_obj_create(SCR_C(VISUAL_AREA_1));
        lv_obj_set_size(obj, 70, LV_SIZE_CONTENT);

        lv_obj_t* label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "%d", i);
        lv_obj_center(label);
    }
}

void
add_readout_item(const char* title, const char* qty, const char* unit, lv_color_t bg_color, lv_obj_t* parent_container)
{
    lv_obj_t* obj = lv_obj_create(parent_container);
    lv_obj_add_style(obj, STYLE_PTR_CM(READOUT), LV_PART_MAIN);
    lv_obj_set_style_bg_color(obj, bg_color, LV_PART_MAIN);
    lv_obj_set_width(obj, LV_PCT(100));

    lv_obj_t* name_container = lv_obj_create(obj);
    lv_obj_add_style(name_container, STYLE_PTR_CM(READOUT_NAME_CONTAINER), LV_PART_MAIN);
    lv_obj_set_height(name_container, LV_PCT(100));

    lv_obj_t* value_container = lv_obj_create(obj);
    lv_obj_add_style(value_container, STYLE_PTR_CM(READOUT_VALUE_CONTAINER), LV_PART_MAIN);
    lv_obj_set_height(value_container, LV_PCT(100));
    lv_obj_set_width(value_container, LV_SIZE_CONTENT);

    lv_obj_t* name_label = lv_label_create(name_container);
    lv_obj_set_width(name_label, LV_PCT(100));
    lv_obj_add_style(name_label, STYLE_PTR_CM(READOUT_NAME_TEXT), LV_PART_MAIN);
    lv_label_set_text_fmt(name_label, title);
    //

    lv_obj_t* value_amount_label = lv_label_create(value_container);
    lv_obj_t* value_unit_label;
    lv_obj_set_width(value_amount_label, LV_SIZE_CONTENT);
    lv_obj_add_style(value_amount_label, STYLE_PTR_CM(READOUT_VALUE_AMOUNT_TEXT), LV_PART_MAIN);
    lv_label_set_text_fmt(value_amount_label, qty);
    lv_obj_center(value_amount_label);

    if (unit) {
        value_unit_label = lv_label_create(value_container);
        lv_obj_add_style(value_unit_label, STYLE_PTR_CM(READOUT_VALUE_UNIT_TEXT), LV_PART_MAIN);
        lv_label_set_text_fmt(value_unit_label, unit);
    }

#if DEBUG_BORDER_READOUTS
    lv_obj_set_style_border_width(name_container, 1 px, LV_PART_MAIN);
    lv_obj_set_style_border_width(value_container, 1 px, LV_PART_MAIN);
    lv_obj_set_style_border_width(name_label, 1 px, LV_PART_MAIN);
    lv_obj_set_style_border_width(value_amount_label, 1 px, LV_PART_MAIN);
    if (unit && value_unit_label) {
        lv_obj_set_style_border_width(value_unit_label, 1 px, LV_PART_MAIN);
    }
#endif
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

void add_control_item(const char* title, const char* qty, const char* unit, lv_color_t button_color,
        lv_obj_t* parent_container)
{
    lv_obj_t* obj = lv_obj_create(parent_container);
    lv_obj_add_style(obj, STYLE_PTR_CM(CONTROL), LV_PART_MAIN);
    lv_obj_set_width(obj, LV_PCT(100));

    lv_obj_t* reduce_button = lv_btn_create(obj);
    lv_obj_add_style(reduce_button, STYLE_PTR_CM(CONTROL_BUTTON), LV_PART_MAIN);
    lv_obj_set_size(reduce_button, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    lv_obj_t* reduce_button_label = lv_label_create(reduce_button);

    lv_obj_add_style(reduce_button_label, STYLE_PTR_CM(CONTROL_BUTTON_TEXT), LV_PART_MAIN);
    lv_label_set_text_fmt(reduce_button_label, "-");
    lv_obj_align(reduce_button_label, LV_ALIGN_CENTER, -1, -1);

    lv_obj_t* text_container = lv_obj_create(obj);
    lv_obj_add_style(text_container, STYLE_PTR_CM(CONTROL_TEXT_CONTAINER), LV_PART_MAIN);
    lv_obj_set_size(text_container, LV_PCT(100), LV_PCT(100));

    lv_obj_t* top_container = lv_obj_create(text_container);
    lv_obj_add_style(top_container, STYLE_PTR_CM(CONTROL_TEXT_CONTAINER_TOP), LV_PART_MAIN);

    lv_obj_t* value_label = lv_label_create(top_container);
    lv_obj_t* unit_label;
    lv_obj_set_width(value_label, LV_SIZE_CONTENT);
    lv_obj_add_style(value_label, STYLE_PTR_CM(CONTROL_TEXT_VALUE), LV_PART_MAIN);
    lv_label_set_text_fmt(value_label, "%s", qty);

    if (unit) {
        unit_label = lv_label_create(top_container);
        lv_obj_set_width(unit_label, LV_SIZE_CONTENT);
        lv_obj_add_style(unit_label, STYLE_PTR_CM(CONTROL_TEXT_VALUE), LV_PART_MAIN);
        lv_obj_set_style_text_font(unit_label, &lv_font_montserrat_14, LV_PART_MAIN);
        lv_label_set_text_fmt(unit_label, "%s", unit);
    }

    lv_obj_t* name_label = lv_label_create(text_container);
    lv_obj_set_width(name_label, LV_PCT(100));
    lv_obj_add_style(name_label, STYLE_PTR_CM(CONTROL_TEXT_NAME), LV_PART_MAIN);
    lv_label_set_text_fmt(name_label, title);

    lv_obj_t* increase_button = lv_btn_create(obj);
    lv_obj_add_style(increase_button, STYLE_PTR_CM(CONTROL_BUTTON), LV_PART_MAIN);
    lv_obj_set_size(increase_button, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    lv_obj_t* increase_button_label = lv_label_create(increase_button);

    lv_obj_add_style(increase_button_label, STYLE_PTR_CM(CONTROL_BUTTON_TEXT), LV_PART_MAIN);
    lv_label_set_text_fmt(increase_button_label, "+");
    lv_obj_center(increase_button_label);

#if DEBUG_BORDER_CONTROLS
    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(reduce_button, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(reduce_button_label, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(increase_button, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(increase_button_label, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(text_container, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(top_container, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(value_label, 1, LV_PART_MAIN);
    if (unit && unit_label) {
        lv_obj_set_style_border_width(unit_label, 1, LV_PART_MAIN);
    }
    lv_obj_set_style_border_width(name_label, 1, LV_PART_MAIN);
#endif
}

void add_start_button()
{
    lv_obj_t* button = lv_btn_create(SCR_C(VISUAL_AREA_3));
    lv_obj_set_height(button, LV_PCT(100));
    lv_obj_add_style(button, STYLE_PTR_CM(OPTION_BUTTON), LV_PART_MAIN);
    lv_obj_add_flag(button, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_style_bg_color(button, LV_COLOR_MAKE(49, 230, 133), LV_PART_MAIN);
    lv_obj_set_style_bg_color(button, LV_COLOR_MAKE(255, 172, 53), LV_STATE_CHECKED);
    lv_obj_set_style_flex_grow(button, 2, LV_PART_MAIN);

    lv_obj_t* label = lv_label_create(button);
    lv_obj_set_width(label, LV_SIZE_CONTENT);
    lv_obj_add_style(label, STYLE_PTR_CM(OPTION_BUTTON_TEXT), LV_PART_MAIN);
    lv_label_set_text_fmt(label, "Start");
    lv_obj_align_to(label, button, LV_ALIGN_CENTER, 0, 0);
    lv_obj_center(label);

    lv_obj_add_event_cb(button, on_enable_disable_press, LV_EVENT_ALL, nullptr);
}

void add_mute_button()
{
    lv_obj_t* button = lv_btn_create(SCR_C(VISUAL_AREA_3));
    lv_obj_set_height(button, LV_PCT(100));
    lv_obj_set_width(button, LV_SIZE_CONTENT);
    lv_obj_add_style(button, STYLE_PTR_CM(OPTION_BUTTON), LV_PART_MAIN);
    lv_obj_add_flag(button, LV_OBJ_FLAG_CHECKABLE);

    lv_obj_set_style_bg_color(button, LV_COLOR_MAKE(255, 172, 53), LV_STATE_CHECKED);
    lv_obj_set_style_flex_grow(button, FLEX_GROW, LV_PART_MAIN);

    lv_obj_t* label = lv_label_create(button);
    lv_obj_set_width(label, LV_SIZE_CONTENT);
    lv_obj_add_style(label, STYLE_PTR_CM(OPTION_BUTTON_TEXT), LV_PART_MAIN);
    lv_label_set_text_fmt(label, "Mute\nAlarms");
    lv_obj_align_to(label, button, LV_ALIGN_CENTER, 0, 0);
    lv_obj_center(label);

}

static void on_enable_disable_press(lv_event_t* e)
{
    static uint32_t cnt = 1;
    lv_obj_t* btn = lv_event_get_target(e);
    lv_obj_t* label = lv_obj_get_child(btn, 0);
    if (e->code == LV_EVENT_VALUE_CHANGED) {
        bool enabled = (strcmp(lv_label_get_text(label), "Standby") == 0);
        LV_LOG_USER("State Value: %d", enabled);
        if (enabled) {
            lv_label_set_text_fmt(label, "Start");
        }
        else {
            lv_label_set_text_fmt(label, "Standby");
        }
    }
}

void add_settings_button(const char* title)
{
    lv_obj_t* button = lv_btn_create(SCR_C(CONTROL_AREA_2));
    lv_obj_set_width(button, LV_PCT(100));
    lv_obj_add_style(button, STYLE_PTR_CM(OPTION_BUTTON), LV_PART_MAIN);
    lv_obj_set_style_pad_top(button, 4 px, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(button, 4 px, LV_PART_MAIN);

    lv_obj_t* label = lv_label_create(button);
    lv_obj_set_width(label, LV_SIZE_CONTENT);
    lv_obj_add_style(label, STYLE_PTR_CM(OPTION_BUTTON_TEXT), LV_PART_MAIN);
    lv_label_set_text_fmt(label, title);
    lv_obj_align_to(label, button, LV_ALIGN_CENTER, 0, 0);
    lv_obj_center(label);
}