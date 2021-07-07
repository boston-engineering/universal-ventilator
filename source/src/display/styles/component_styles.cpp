#include "../main_display.h"

/**
* ComponentType::READOUT
*/
STYLE_INIT_FUNC(READOUT)
{
    lv_style_t* style = STYLE_PTR_CM(READOUT);
    // Colors & Borders
    lv_style_set_radius(style, 8 px);
    // lv_style_set_bg_color(style, palette_color_1);
    lv_style_set_bg_opa(style, LV_OPA_COVER);
    lv_style_set_pad_all(style, 0 px);
    // Shadow
    lv_style_set_shadow_width(style, 8 px);
    lv_style_set_shadow_color(style, lv_color_black());
    lv_style_set_shadow_ofs_x(style, 4 px);
    lv_style_set_shadow_ofs_y(style, 4 px);
    // Flexbox
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_SPACE_EVENLY);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_START);
    lv_style_set_pad_column(style, 2 px);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    // Inside Flex
    lv_style_set_flex_grow(style, FLEX_GROW);
}

/**
* ComponentType::READOUT_NAME_CONTAINER
*/
STYLE_INIT_FUNC(READOUT_NAME_CONTAINER)
{
    lv_style_t* style = STYLE_PTR_CM(READOUT_NAME_CONTAINER);
    // Colors & Borders
    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    lv_style_set_pad_left(style, 4 px);
    lv_style_set_pad_right(style, 0 px);
    lv_style_set_pad_top(style, 0 px);
    lv_style_set_pad_bottom(style, 4 px);
    // Flexbox
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_END);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_START);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    // Inside Flex
    lv_style_set_flex_grow(style, FLEX_GROW);
}

/**
* ComponentType::READOUT_NAME_TEXT
*/
STYLE_INIT_FUNC(READOUT_NAME_TEXT)
{
    lv_style_t* style = STYLE_PTR_CM(READOUT_NAME_TEXT);
    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    // Font Properties
    lv_style_set_text_color(style, lv_color_white());
    lv_style_set_text_align(style, LV_TEXT_ALIGN_LEFT);
    lv_style_set_text_font(style, &lv_font_montserrat_18);
    // Flexbox
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
}

/**
* ComponentType::READOUT_VALUE_CONTAINER
*/
STYLE_INIT_FUNC(READOUT_VALUE_CONTAINER)
{
    lv_style_t* style = STYLE_PTR_CM(READOUT_VALUE_CONTAINER);
    // Colors & Borders
    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    lv_style_set_pad_left(style, 0 px);
    lv_style_set_pad_right(style, 6 px);
    lv_style_set_pad_top(style, 2 px);
    lv_style_set_pad_bottom(style, 2 px);
    // Flexbox
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_END);
    lv_style_set_pad_row(style, 0 px);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
}

/**
* ComponentType::READOUT_VALUE_UNIT_TEXT
*/
STYLE_INIT_FUNC(READOUT_VALUE_UNIT_TEXT)
{
    lv_style_t* style = STYLE_PTR_CM(READOUT_VALUE_UNIT_TEXT);
    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    // Font Properties
    lv_style_set_text_color(style, lv_color_white());
    lv_style_set_text_align(style, LV_TEXT_ALIGN_RIGHT);
    lv_style_set_text_font(style, &lv_font_montserrat_16);
    // Flexbox
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
}

/**
* ComponentType::READOUT_VALUE_AMOUNT_TEXT
*/
STYLE_INIT_FUNC(READOUT_VALUE_AMOUNT_TEXT)
{
    lv_style_t* style = STYLE_PTR_CM(READOUT_VALUE_AMOUNT_TEXT);
    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    // Font Properties
    lv_style_set_text_color(style, lv_color_white());
    lv_style_set_text_align(style, LV_TEXT_ALIGN_RIGHT);
    lv_style_set_text_font(style, &lv_font_montserrat_40);
    // Flexbox
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
}

/**
* ComponentType::OPTION_BUTTON
*/
STYLE_INIT_FUNC(OPTION_BUTTON)
{
    lv_style_t* style = STYLE_PTR_CM(OPTION_BUTTON);
    // Colors & Borders
    lv_style_set_bg_color(style, LV_COLOR_MAKE(215, 215, 215));
    lv_style_set_radius(style, 8 px);
    // Inside Flex
    lv_style_set_flex_grow(style, FLEX_GROW);
}

/**
* ComponentType::OPTION_BUTTON_TEXT
*/
STYLE_INIT_FUNC(OPTION_BUTTON_TEXT)
{
    lv_style_t* style = STYLE_PTR_CM(OPTION_BUTTON_TEXT);
    // Font Properties
    lv_style_set_text_color(style, lv_color_black());
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_font(style, &lv_font_montserrat_24);
}

/**
* ComponentType::CONTROL
*/
STYLE_INIT_FUNC(CONTROL)
{
    lv_style_t* style = STYLE_PTR_CM(CONTROL);
    // Colors & Borders
    lv_style_set_bg_opa(style, LV_OPA_COVER);
    lv_style_set_pad_all(style, 5 px);
    // Flexbox
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_SPACE_BETWEEN);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_pad_column(style, 2 px);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    // Inside Flex
    lv_style_set_flex_grow(style, FLEX_GROW);
}

/**
* ComponentType::CONTROL_TEXT_CONTAINER
*/
STYLE_INIT_FUNC(CONTROL_TEXT_CONTAINER)
{
    lv_style_t* style = STYLE_PTR_CM(CONTROL_TEXT_CONTAINER);
    // Colors & Borders
    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    // Flexbox
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_pad_row(style, 0 px);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    // Inside Flex
    lv_style_set_flex_grow(style, FLEX_GROW);
}

/**
* ComponentType::CONTROL_TEXT_CONTAINER_TOP
*/
STYLE_INIT_FUNC(CONTROL_TEXT_CONTAINER_TOP)
{
    lv_style_t* style = STYLE_PTR_CM(CONTROL_TEXT_CONTAINER_TOP);
    // Colors & Borders
    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    // Flexbox
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_END);
    lv_style_set_pad_column(style, 4 px);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_width(style, LV_SIZE_CONTENT);
    lv_style_set_height(style, LV_SIZE_CONTENT);
}

/**
* ComponentType::CONTROL_TEXT_CONTAINER_IE_TOP
*/
STYLE_INIT_FUNC(CONTROL_TEXT_CONTAINER_IE_TOP)
{
    lv_style_t* style = STYLE_PTR_CM(CONTROL_TEXT_CONTAINER_IE_TOP);
    // Colors & Borders
    lv_style_set_bg_opa(style, LV_OPA_TRANSP);
    // Flexbox
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_pad_column(style, 6 px);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_width(style, LV_SIZE_CONTENT);
    lv_style_set_height(style, LV_SIZE_CONTENT);
//    lv_style_set_height(style,LV_PCT(100));
}

/**
* ComponentType::CONTROL_TEXT_VALUE
*/
STYLE_INIT_FUNC(CONTROL_TEXT_VALUE)
{
    lv_style_t* style = STYLE_PTR_CM(CONTROL_TEXT_VALUE);
    // Font Properties
    lv_style_set_text_color(style, lv_color_black());
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_font(style, &lv_font_montserrat_28);
}

/**
* ComponentType::CONTROL_TEXT_NAME
*/
STYLE_INIT_FUNC(CONTROL_TEXT_NAME)
{
    lv_style_t* style = STYLE_PTR_CM(CONTROL_TEXT_NAME);
    // Font Properties
    lv_style_set_text_color(style, lv_color_black());
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_font(style, &lv_font_montserrat_18);
}

/**
* ComponentType::CONTROL_BUTTON
*/
STYLE_INIT_FUNC(CONTROL_BUTTON)
{
    lv_style_t* style = STYLE_PTR_CM(CONTROL_BUTTON);
    // Colors & Borders
    lv_style_set_bg_color(style, palette_color_1);
    lv_style_set_radius(style, 50 px);
    lv_style_set_min_width(style, 45 px);
    lv_style_set_min_height(style, 45 px);
}

/**
* ComponentType::CONTROL_BUTTON_TEXT
*/
STYLE_INIT_FUNC(CONTROL_BUTTON_TEXT)
{
    lv_style_t* style = STYLE_PTR_CM(CONTROL_BUTTON_TEXT);
    // Font Properties
    lv_style_set_text_color(style, lv_color_white());
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_font(style, &lv_font_montserrat_24);
}

/**
* ComponentType::DIVIDER
*/
STYLE_INIT_FUNC(DIVIDER)
{
    lv_style_t* style = STYLE_PTR_CM(DIVIDER);
    // Line properties
    lv_style_set_line_width(style, 2 px);
    lv_style_set_line_color(style, lv_color_black());
    lv_style_set_line_rounded(style, true);
}