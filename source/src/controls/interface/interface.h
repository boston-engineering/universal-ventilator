#ifndef UVENT_INTERFACE_H
#define UVENT_INTERFACE_H

#include <lvgl.h>
#include <variant.h>
#include <display/main_display.h>

#define clamp(n, low, high) max(low, min(n, high))

typedef enum AdjValueType {
    TIDAL_VOLUME = 0,
    RESPIRATION_RATE,
    PEEP,
    PIP,
    PLATEAU_TIME,
    CUR_PRESSURE,
    IE_RATIO_LEFT,
    IE_RATIO_RIGHT,
    ADJ_VALUE_COUNT,
    UNKNOWN,
} AdjValueType;

typedef struct AdjValueParams {
    AdjValueParams(const char* title, const char* control_title, const char* measured_formatter,
            const char* target_formatter, const char* unit,
            float min_val, float max_val, float step, bool readout,
            bool control, lv_color_t main_color)
            : title(title), control_title(control_title), measured_formatter(measured_formatter),
              target_formatter(target_formatter), unit(unit),
              min_value(min_val), max_value(max_val), step(step), create_readout(readout),
              create_control(control), main_color(main_color) { }

    const char* title;
    const char* control_title;
    const char* measured_formatter;
    const char* target_formatter;
    const char* unit;
    double min_value;
    double max_value;
    double step;
    bool create_readout = true;
    bool create_control = false;
    lv_color_t main_color;
} AdjValueParams;

extern const AdjValueParams adj_value_settings[ADJ_VALUE_COUNT];

class AdjustableValue {
public:
    AdjustableValue() = default;

    explicit AdjustableValue(double val)
            : target(val) { }

    AdjValueType value_type = UNKNOWN;

    void (* control_press_cb)(AdjustableValue* this_ptr, lv_event_t* evt) = nullptr;
    void (* readout_update_cb)(AdjustableValue* this_ptr, lv_event_t* evt) = nullptr;

    /**
     * Sets up the object for use. Must be called before calling get_settings
     * @param type The on-screen measured type
     */
    inline void init(AdjValueType type)
    {
        value_type = type;
    }

    void refresh_readout();
    void on_control_button_press(lv_event_t* evt);
    void on_readout_update(lv_event_t* evt);
    AdjValueParams get_settings() const;
    double get_step() const;

    /**
     * The holder for all the content that displays the sensor-measured value
     * @return The base object
     */
    lv_obj_t* get_obj_measured();

    /**
     * The holder for all the controls to set the target value for the mechanics
     * @return The base object
     */
    lv_obj_t* get_obj_target();
    AdjustableValue set_obj_measured(lv_obj_t* obj);
    AdjustableValue set_obj_target(lv_obj_t* obj);

    /**
     * @return The last measured value
     */
    double* get_value_measured();

    /**
     * @return The current set target value
     */
    double* get_value_target();
    AdjustableValue set_value_measured(double value);
    AdjustableValue set_value_target(double value);

private:
    lv_obj_t* lv_obj_measured = nullptr;
    lv_obj_t* lv_obj_target = nullptr;
    double measured = -1;
    double target = 0;
};

extern AdjustableValue adjustable_values[AdjValueType::ADJ_VALUE_COUNT];

#endif //UVENT_INTERFACE_H
