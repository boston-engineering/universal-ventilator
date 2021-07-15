#include <display/layouts/layouts.h>
#include "screen.h"

MainScreen::MainScreen()
        : Screen()
{

}

void MainScreen::init()
{
    Screen::init();
}

void MainScreen::setup()
{
    Screen::setup();

    setup_readouts();
    setup_controls();

    // FULL SCREEN
    add_dividers();

    // VISUAL_AREA_2
    setup_visual_2();
    add_gauge_chart();
    add_vt_chart();

    setup_buttons();
    attach_settings_cb();
}

void MainScreen::attach_settings_cb()
{
    // Add callback to settings button to setup the screen
    lv_obj_t* settings_button_container = SCR_C(CONTROL_AREA_2);
    lv_obj_t* settings_button = lv_obj_get_child(settings_button_container, 0);

    if (settings_button) {
        auto on_settings_check = [](lv_event_t* evt) {
            lv_obj_t* target = lv_event_get_target(evt);
            lv_state_t state = lv_obj_get_state(target);
            if (!lv_obj_has_flag(target, LV_OBJ_FLAG_CHECKABLE)) {
                return;
            }

            auto* screen_ptr = (MainScreen*) evt->user_data;

            if ((state & LV_STATE_CHECKED) != 0) {
                screen_ptr->open_config();
            }
            else {
                setup_controls();
            }
        };

        lv_obj_add_event_cb(settings_button, on_settings_check, LV_EVENT_VALUE_CHANGED, this);
    }
}

void MainScreen::open_config()
{
    setup_config_window();
}

void MainScreen::add_gauge_chart()
{

    if (gauge_pressure_chart != nullptr) {
        LV_LOG_ERROR("User attempted to create chart that already exists, aborting...");
        return;
    }

    /*Create a chart*/
    lv_obj_t* screen_area = SCR_C(VISUAL_AREA_2);
    lv_obj_t* parent = lv_obj_get_child(screen_area, 0);

    lv_obj_t* label = lv_label_create(parent);
    lv_label_set_text(label, "Gauge Pressure");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_height(label, LV_SIZE_CONTENT);

    gauge_pressure_chart = lv_chart_create(parent);
    lv_obj_set_flex_grow(gauge_pressure_chart, FLEX_GROW);
    lv_obj_set_height(gauge_pressure_chart, LV_PCT(100));
    lv_obj_set_style_border_width(gauge_pressure_chart, 2 px, LV_PART_MAIN);
    lv_obj_set_style_border_color(gauge_pressure_chart, lv_color_black(), LV_PART_MAIN);

    lv_chart_set_type(gauge_pressure_chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/
    lv_chart_set_range(gauge_pressure_chart, LV_CHART_AXIS_PRIMARY_Y, GAUGE_PRESSURE_CHART_MIN_VALUE, GAUGE_PRESSURE_CHART_MAX_VALUE);

    /*Add data series*/
    lv_chart_add_series(gauge_pressure_chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_point_count(gauge_pressure_chart, GAUGE_PRESSURE_CHART_MAX_POINTS);
    lv_chart_set_update_mode(gauge_pressure_chart, LV_CHART_UPDATE_MODE_SHIFT);

}

void MainScreen::add_vt_chart()
{

    if (vt_chart != nullptr) {
        LV_LOG_ERROR("User attempted to create chart that already exists, aborting...");
        return;
    }

    /*Create a chart*/
    lv_obj_t* screen_area = SCR_C(VISUAL_AREA_2);
    lv_obj_t* parent = lv_obj_get_child(screen_area, 0);

    lv_obj_t* label = lv_label_create(parent);
    lv_label_set_text(label, "Tidal Volume");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_height(label, LV_SIZE_CONTENT);

    vt_chart = lv_chart_create(parent);
    lv_obj_set_flex_grow(vt_chart, FLEX_GROW);
    lv_obj_set_height(vt_chart, LV_PCT(100));
    lv_obj_set_style_border_width(vt_chart, 2 px, LV_PART_MAIN);
    lv_obj_set_style_border_color(vt_chart, lv_color_black(), LV_PART_MAIN);

    lv_chart_set_type(vt_chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/
    lv_chart_set_range(vt_chart, LV_CHART_AXIS_PRIMARY_Y, VT_CHART_MIN_VALUE, VT_CHART_MAX_VALUE);

    /*Add data series*/
    lv_chart_add_series(vt_chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_point_count(vt_chart, VT_CHART_MAX_POINTS);
    lv_chart_set_update_mode(vt_chart, LV_CHART_UPDATE_MODE_SHIFT);

}

void MainScreen::add_gauge_pressure_chart_point(double data)
{
    if(!gauge_pressure_chart) {
        return;
    }
    lv_chart_series_t* series = lv_chart_get_series_next(gauge_pressure_chart, nullptr);

    lv_chart_set_next_value(gauge_pressure_chart, series, data);
}

void MainScreen::refresh_gauge_pressure_chart() {
    if(!gauge_pressure_chart) {
        return;
    }
    lv_chart_refresh(gauge_pressure_chart);
}

void MainScreen::add_vt_chart_point(double data)
{
    if(!vt_chart) {
        return;
    }
    lv_chart_series_t* series = lv_chart_get_series_next(vt_chart, nullptr);

    lv_chart_set_next_value(vt_chart, series, data);
}

void MainScreen::refresh_vt_chart() {
    if(!vt_chart) {
        return;
    }
    lv_chart_refresh(vt_chart);
}


