#ifndef UVENT_CHARTS_H
#define UVENT_CHARTS_H

#include <lvgl.h>

/**
 * Chart that gets its data from a polled sensor
 */
struct SensorChart {
    const char* name{};
    lv_obj_t* chart = nullptr;
    int32_t range_min{};
    int32_t range_max{};
    uint32_t chart_points{};
    uint32_t refresh_time{};
    uint32_t last_refreshed = 0;

    SensorChart() = default;
    SensorChart(const char* name, int32_t min_val, int32_t max_val, uint32_t chart_points, uint32_t refresh_time);

    void generate_chart(lv_obj_t* parent);
    void add_data_point(double data) const;
    bool should_refresh();
    void refresh_chart() const;
};

#endif //UVENT_CHARTS_H
