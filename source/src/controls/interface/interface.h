#ifndef UVENT_INTERFACE_H
#define UVENT_INTERFACE_H

#include <lvgl.h>
#include <variant.h>
#include <display/main_display.h>

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

template<typename T>
class AdjustableValue {
public:
    AdjustableValue() = default;

    explicit AdjustableValue(T val)
            : target(val) { }

    AdjValueType value_type = UNKNOWN;

    void (* control_press_cb)(AdjustableValue<T>* this_ptr, lv_event_t* evt) = nullptr;
    void (* readout_update_cb)(AdjustableValue<T>* this_ptr, lv_event_t* evt) = nullptr;

    /**
     * Sets up the object for use. Must be called before calling get_settings
     * @param type The on-screen measured type
     */
    inline void init(AdjValueType type)
    {
        value_type = type;
    }

    void on_control_button_press(lv_event_t* evt)
    {
        // TODO generic on control press code
        if (control_press_cb) {
            control_press_cb(this, evt);
        }
    }

    void on_readout_update(lv_event_t* evt)
    {
        // TODO generic on control press code

        // TODO LV_EVENT_REFRESH
        if (readout_update_cb) {
            readout_update_cb(this, evt);
        }
    }

    AdjValueParams get_settings()
    {
        if (value_type >= ADJ_VALUE_COUNT) {
            return adj_value_settings[0];
        }
        return adj_value_settings[value_type];
    }

    /**
     * The holder for all the content that displays the sensor-measured value
     * @return The base object
     */
    lv_obj_t* get_obj_measured()
    {
        return lv_obj_measured;
    }

    /**
     * The holder for all the controls to set the target value for the mechanics
     * @return The base object
     */
    lv_obj_t* get_obj_target()
    {
        return lv_obj_target;
    }

    AdjustableValue<T> set_obj_measured(lv_obj_t* obj)
    {
        if (!obj) return *this;
        lv_obj_measured = obj;
        lv_obj_measured->user_data = this;
        return *this;
    }

    AdjustableValue<T> set_obj_target(lv_obj_t* obj)
    {
        if (!obj) return *this;
        lv_obj_target = obj;
        lv_obj_target->user_data = this;
        return *this;
    }

    /**
     * @return The last measured value
     */
    T* get_value_measured()
    {
        return &measured;
    }

    /**
     * @return The current set target value
     */
    T* get_value_target()
    {
        return &target;
    }

    AdjustableValue<T> set_value_measured(T& value)
    {
        measured = value;
        return *this;
    }

    AdjustableValue<T> set_value_target(T& value)
    {
        target = value;
        return *this;
    }

private:
    lv_obj_t* lv_obj_measured = nullptr;
    lv_obj_t* lv_obj_target = nullptr;
    T measured;
    T target;
};

extern AdjustableValue<double> adjustable_values[AdjValueType::ADJ_VALUE_COUNT];

#endif //UVENT_INTERFACE_H
