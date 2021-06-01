#include "main_display.h"

lv_obj_t* containers[DisplayContainer::CONTAINER_COUNT];
lv_style_t container_styles[DisplayContainer::CONTAINER_COUNT];
lv_style_t component_styles[ComponentType::COMPONENT_COUNT];

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

    /**
     * ComponentType::READOUT_STATIC
     */
    style = STYLE_PTR_CM(READOUT_STATIC);
    // Colors & Borders
    lv_style_set_radius(style, 8 px);
    lv_style_set_bg_color(style, palette_color_1);
    lv_style_set_bg_opa(style, LV_OPA_COVER);
    lv_style_set_pad_all(style, 0 px);
    // Shadow
    // TODO setup shadow, may require transparency?
    // Flexbox
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_SPACE_EVENLY);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_START);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    // Inside Flex
    lv_style_set_flex_grow(style, FLEX_GROW);

    /**
     * ComponentType::READOUT_NAME_CONTAINER
     */
    style = STYLE_PTR_CM(READOUT_NAME_CONTAINER);
    // Colors & Borders
    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    lv_style_set_pad_all(style, 4 px);
    // Flexbox
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_END);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_START);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    // Inside Flex
    lv_style_set_flex_grow(style, FLEX_GROW);

    /**
     * ComponentType::READOUT_VALUE_CONTAINER
     */
    style = STYLE_PTR_CM(READOUT_VALUE_CONTAINER);
    // Colors & Borders
    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    lv_style_set_pad_left(style, 0 px);
    lv_style_set_pad_right(style, 8 px);
    lv_style_set_pad_top(style, 0 px);
    lv_style_set_pad_bottom(style, 0 px);
    // Flexbox
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_END);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    // Inside Flex
    lv_style_set_flex_grow(style, FLEX_GROW);

    /**
     * ComponentType::READOUT_NAME_TEXT
     */
    style = STYLE_PTR_CM(READOUT_NAME_TEXT);
    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    // Font Properties
    lv_style_set_text_color(style, lv_color_white());
    lv_style_set_text_align(style, LV_TEXT_ALIGN_LEFT);
    lv_style_set_text_font(style, &lv_font_montserrat_14);
    // Flexbox (Maybe not needed)
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);

    /**
     * ComponentType::READOUT_VALUE_AMOUNT_TEXT
     */
    style = STYLE_PTR_CM(READOUT_VALUE_AMOUNT_TEXT);
    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    // Font Properties
    lv_style_set_text_color(style, lv_color_white());
    lv_style_set_text_align(style, LV_TEXT_ALIGN_RIGHT);
    lv_style_set_text_font(style, &lv_font_montserrat_36);
    // Flexbox (Maybe not needed)
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);

    /**
     * ComponentType::READOUT_VALUE_UNIT_TEXT
     */
    style = STYLE_PTR_CM(READOUT_VALUE_UNIT_TEXT);
    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    // Font Properties
    lv_style_set_text_color(style, lv_color_white());
    lv_style_set_text_align(style, LV_TEXT_ALIGN_RIGHT);
    lv_style_set_text_font(style, &lv_font_montserrat_12);
    // Flexbox (Maybe not needed)
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);

}

void populate_items()
{
    add_readout_item();
    add_readout_item();
    add_readout_item();
    add_readout_item();
    add_readout_item();
}

void add_readout_item()
{
    lv_obj_t* parent = SCR_C(VISUAL_AREA_1);
    lv_obj_t* obj = lv_obj_create(parent);
    lv_obj_add_style(obj, STYLE_PTR_CM(READOUT_STATIC), LV_PART_MAIN);
    lv_obj_set_width(obj, LV_PCT(100));

    lv_obj_t* name_container = lv_obj_create(obj);
    lv_obj_add_style(name_container, STYLE_PTR_CM(READOUT_NAME_CONTAINER), LV_PART_MAIN);
    lv_obj_set_height(name_container, LV_PCT(100));

    lv_obj_t* value_container = lv_obj_create(obj);
    lv_obj_add_style(value_container, STYLE_PTR_CM(READOUT_VALUE_CONTAINER), LV_PART_MAIN);
    lv_obj_set_style_border_width(value_container, 1 px, LV_PART_MAIN);
    lv_obj_set_height(value_container, LV_PCT(100));

    lv_obj_t* name_label = lv_label_create(name_container);
    lv_obj_set_width(name_label, LV_PCT(100));
    lv_obj_add_style(name_label, STYLE_PTR_CM(READOUT_NAME_TEXT), LV_PART_MAIN);
    lv_label_set_text_fmt(name_label, "Tidal Pressure");

    lv_obj_t* value_amount_label = lv_label_create(value_container);
    lv_obj_set_width(value_amount_label, LV_PCT(100));
    lv_obj_add_style(value_amount_label, STYLE_PTR_CM(READOUT_VALUE_AMOUNT_TEXT), LV_PART_MAIN);
    lv_obj_set_style_border_width(value_amount_label, 1 px, LV_PART_MAIN);
    lv_label_set_text_fmt(value_amount_label, "350");

    lv_obj_t* value_unit_label = lv_label_create(value_container);
    lv_obj_add_style(value_unit_label, STYLE_PTR_CM(READOUT_VALUE_UNIT_TEXT), LV_PART_MAIN);
    lv_obj_set_style_border_width(value_unit_label, 1 px, LV_PART_MAIN);
    lv_label_set_text_fmt(value_unit_label, "mL");
}