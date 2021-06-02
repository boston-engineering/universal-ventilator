#include "../main_display.h"

CONTAINER_INIT_FUNC(CONTROL_MAIN)
{
    lv_obj_t* container = SCR_C(CONTROL_MAIN);
    SIZEANDALIGN(600, 0, 200, 480, LV_ALIGN_TOP_LEFT);

    // Style init & bind
    lv_style_t* style_ptr = STYLE_PTR_CT(CONTROL_MAIN);
    lv_style_set_border_color(style_ptr, color_black);
    lv_obj_add_style(container, style_ptr, LV_PART_MAIN);
}

CONTAINER_INIT_FUNC(CONTROL_AREA_1)
{
    lv_obj_t* container = SCR_C(CONTROL_AREA_1);
    SIZEANDALIGN(0, 0, 200, 340, LV_ALIGN_TOP_LEFT);

    // Style init & bind
    lv_style_t* style_ptr = STYLE_PTR_CT(CONTROL_AREA_1);
    lv_style_set_border_color(style_ptr, color_black);
    lv_obj_add_style(container, style_ptr, LV_PART_MAIN);
}

CONTAINER_INIT_FUNC(CONTROL_AREA_2)
{
    lv_obj_t* container = SCR_C(CONTROL_AREA_2);
    SIZEANDALIGN(0, 340, 200, 140, LV_ALIGN_TOP_LEFT);

    // Style init & bind
    lv_style_t* style_ptr = STYLE_PTR_CT(CONTROL_AREA_2);
    lv_style_set_border_color(style_ptr, color_black);
    lv_obj_add_style(container, style_ptr, LV_PART_MAIN);
}