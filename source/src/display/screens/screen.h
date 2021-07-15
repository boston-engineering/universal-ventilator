#ifndef UVENT_SCREEN_H
#define UVENT_SCREEN_H

#include <lvgl.h>

class Screen {
public:
    Screen() = default;
    ~Screen();
    virtual void init();
    virtual void setup();
    void cleanup();
    void select_screen();
protected:
    lv_obj_t* screen = nullptr;
};

class MainScreen : public Screen {
public:
    MainScreen();
    void init() override;
    void setup() override;

    void open_config();
    void attach_settings_cb();

    void add_gauge_pressure_chart_point(double data);
    void refresh_gauge_pressure_chart();
private:
    void add_gauge_chart();

    lv_obj_t* gauge_pressure_chart = nullptr;
};

#endif //UVENT_SCREEN_H
