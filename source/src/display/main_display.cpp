#include "main_display.h"
#include "../../config/uvent_conf.h"

lv_obj_t* containers[DisplayContainer::CONTAINER_COUNT];
lv_style_t container_styles[DisplayContainer::CONTAINER_COUNT];
lv_style_t component_styles[ComponentType::COMPONENT_COUNT];

lv_point_t divider_1_points[] = {
        {200, 15},
        {200, 465}
};
lv_point_t divider_2_points[] = {
        {600, 0},
        {600, 480}
};

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
    add_readout_item("Tidal Volume", "350", "mL");
    add_readout_item("Respiration Rate", "30", "/min");
    add_readout_item("I:E Ratio", "1:2", nullptr);
    add_readout_item("PEEP Limit (Floor)", "5", "cmH2O");
    add_readout_item("PIP Limit (Ceiling)", "33", "cmH2O");
    add_readout_item("Current Pressure", "28", "cmH2O", palette_color_2);

    add_dividers();

//  add_dummy_items();
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