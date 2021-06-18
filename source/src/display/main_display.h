#ifndef UVENT_MAIN_DISPLAY_H
#define UVENT_MAIN_DISPLAY_H

#include <lvgl.h>

// LVGL Style Macros
#define SIZEANDALIGN(x, y, w, h, align) \
    lv_obj_set_size(container, w, h); \
    lv_obj_align(container, align, x, y)

// Style, Component, Container, etc. retrieval Macros
/**
 * Gets a container on the screen.
 */
#define SCR_C(idx) containers[DisplayContainer::idx]

/**
 * Gets a style pointer for a container
 */
#define STYLE_PTR_CT(idx) \
    &container_styles[DisplayContainer::idx]

/**
 * Gets a style pointer for a component.
 */
#define STYLE_PTR_CM(idx) \
    &component_styles[ComponentType::idx]

// Container Init Macros
#define CONTAINER_INIT_FUNC_NAME(name) name##_init()

#define CONTAINER_INIT_FUNC(name) void CONTAINER_INIT_FUNC_NAME(name)

#define CONTAINER_INIT(name) \
    extern CONTAINER_INIT_FUNC(name); \
    CONTAINER_INIT_FUNC_NAME(name)

/**
 * Macro to init a container that sits on the actual screen.
 */
#define CONTAINER_DECL_BASE(name) \
    SCR_C(name) = lv_obj_create(lv_scr_act()); \
    CONTAINER_INIT(name)

/**
 * Macro to init a container that exists within another container.
 */
#define CONTAINER_DECL(name, parent) \
    SCR_C(name) = lv_obj_create(SCR_C(parent)); \
    CONTAINER_INIT(name)

// Style Init Macros
#define STYLE_INIT_FUNC_NAME(name) style_##name##_init()

#define STYLE_INIT_FUNC(name) void STYLE_INIT_FUNC_NAME(name)

/**
 * Declares / Inits a style
 */
#define STYLE_DECL(name) \
    extern STYLE_INIT_FUNC(name); \
    STYLE_INIT_FUNC_NAME(name)

/**
 * Visual Marker for pixel values.
 * Blank, adds no code.
 */
#define px
#define FLEX_GROW 1

typedef enum DisplayContainer {
    VISUAL_MAIN = 0,    /**< Left 600x480. Contains VISUAL_AREA_1 and VISUAL_G_C*/
    VISUAL_AREA_1,      /**< Left 200x480. Responsible for data readout boxes.*/
    VISUAL_G_C,         /**< 400x480 @ (200, 0). Contains VISUAL_AREA_2-3.*/
    VISUAL_AREA_2,      /**< 400x400 @ relative (0, 0) [static (200, 0)]. Holds graphs and other data.*/
    VISUAL_AREA_3,      /**< 400x80 @ relative (0, 400) [static (200, 400)]. Holds Start/Stop buttons and Alarm Mute.*/
    CONTROL_MAIN,       /**< 200x480 @ (600, 0). Contains CONTROL_AREA_1-2.*/
    CONTROL_AREA_1,     /**< 200x340 @ relative (0, 0) [static (600, 0)]. Holds data control +/- buttons.*/
    CONTROL_AREA_2,     /**< 200x140 @ relative (0, 340) [static (600, 340)]. Holds Settings/Configuration option buttons.*/
    CONTAINER_COUNT,
} DisplayContainer;

typedef enum ComponentType {
    // Readouts and Displayed Values
    READOUT = 0,                /**< Readout container for for a ventilator value*/
    READOUT_NAME_CONTAINER,     /**< Container for the name of a readout*/
    READOUT_NAME_TEXT,          /**< Text for the name of a readout*/
    READOUT_VALUE_CONTAINER,    /**< Container for the value/unit of a readout*/
    READOUT_VALUE_UNIT_TEXT,    /**< Unit text of a readout*/
    READOUT_VALUE_AMOUNT_TEXT,  /**< Quantity text of a readout*/
    // Start/Stop Buttons, Mute Button, Settings, Configuration
    OPTION_BUTTON,              /**< Action Button*/
    OPTION_BUTTON_TEXT,         /**< Action Button Text*/
    // Value Adjustment Controls
    CONTROL,
    CONTROL_TEXT_CONTAINER,     /**< Holder for configuration readout*/
    CONTROL_TEXT_CONTAINER_TOP, /**< Holder for value & quantity texts*/
    CONTROL_TEXT_VALUE,         /**< Value text*/
    CONTROL_TEXT_NAME,          /**< Name text*/
    CONTROL_BUTTON,             /**< Value Adjuster Button*/
    CONTROL_BUTTON_TEXT,        /**< Value Adjuster Button Text*/
    // Purely Visual Elements
    DIVIDER,                    /**< Divider*/
    BLANK,
    COMPONENT_COUNT,
} ComponentType;

static lv_color_t palette_color_1 = LV_COLOR_MAKE(109, 68, 197);
static lv_color_t palette_color_2 = LV_COLOR_MAKE(59, 125, 185);
static lv_color_t color_black = LV_COLOR_MAKE(0, 0, 0);
static lv_color_t color_gray = LV_COLOR_MAKE(248, 248, 248);

extern lv_obj_t* containers[];
extern lv_style_t container_styles[];
extern lv_style_t component_styles[];

extern lv_point_t divider_1_points[];
extern lv_point_t divider_2_points[];

void init_styles();

static void setup_styles();

/**
 * Main function to add items to the containers.
 */
void populate_items();

/**
 * Adds the two main dividers to the screen.
 * Positioned floating so they don't get in the way of the main layout.
 */
void add_dividers();

void add_dummy_items();

/**
 * Adds an item that displays a value on the left side of the screen. (vT, RR, etc)
 *
 * @param title The name of the readout
 * @param qty The amount measured
 * @param unit The unit being measured (Nullable)
 * @param bg_color The background color of the box
 * @param parent_cont The parent screen of the container
 */
void add_readout_item(const char*, const char*, const char*, lv_color_t bg_color = palette_color_1,
        lv_obj_t* parent_cont = SCR_C(VISUAL_AREA_1));

/**
 * Adds a simple chart to the screen.
 * Currently a dummy function
 */
void add_chart();

/**
 * Adds a control unit to the right side of the screen.
 * Includes +/- buttons, nametags, unit, etc.
 * @param unit The unit of the readout
 * @param button_color The color the button's background should be
 * @param parent_cont The parent of the control unit
 */
void add_control_item(const char*, const char*, const char* unit, lv_color_t button_color = palette_color_1,
        lv_obj_t* parent_cont = SCR_C(CONTROL_AREA_1));

/**
 * Adds the start button
 */
void add_start_button();

/**
 * Adds the mute button
 */
void add_mute_button();

/**
 * Adds the settings/config button
 * @param title Name of the container for easy changing
 */
void add_settings_button(const char* title);

/**
 * Sets up all containers for the main display
 * The CONTAINER_DECL_BASE and CONTAINER_DECL macros also include init prototypes to scope them within this file.
 * The definitions of these prototypes are in `control_containers.cpp` and `visual_containers.cpp`
 *
 * This function should only be called once at startup
 */
inline void init_main_display()
{
    // Setup styles before we define the containers
    init_styles();

    // Run style setup functions once base settings are done
    setup_styles();

    // Setup master containers
    CONTAINER_DECL_BASE(VISUAL_MAIN);
    CONTAINER_DECL_BASE(CONTROL_MAIN);
    // Visual Main Subcontainers
    CONTAINER_DECL(VISUAL_AREA_1, VISUAL_MAIN);
    CONTAINER_DECL(VISUAL_G_C, VISUAL_MAIN);
    // Visual Graphs & Controls Subcontainers
    CONTAINER_DECL(VISUAL_AREA_2, VISUAL_G_C);
    CONTAINER_DECL(VISUAL_AREA_3, VISUAL_G_C);
    // Control Main Subcontainers
    CONTAINER_DECL(CONTROL_AREA_1, CONTROL_MAIN);
    CONTAINER_DECL(CONTROL_AREA_2, CONTROL_MAIN);

    populate_items();
}

/**
 * Declares and runs macro functions for each defined style to setup individual properties.<br>
 * The STYLE_DECL macro writes both the prototype of the function, and then executes it.<br>
 * The definitions of these prototypes are in `component_styles.cpp`.<br>
 * <br>
 * This function should only be called by the `init_main_display` function
 */
static inline void setup_styles()
{

    STYLE_DECL(READOUT);
    STYLE_DECL(READOUT_NAME_CONTAINER);
    STYLE_DECL(READOUT_NAME_TEXT);
    STYLE_DECL(READOUT_VALUE_CONTAINER);
    STYLE_DECL(READOUT_VALUE_UNIT_TEXT);
    STYLE_DECL(READOUT_VALUE_AMOUNT_TEXT);
    STYLE_DECL(OPTION_BUTTON);
    STYLE_DECL(OPTION_BUTTON_TEXT);
    STYLE_DECL(CONTROL);
    STYLE_DECL(CONTROL_TEXT_CONTAINER);
    STYLE_DECL(CONTROL_TEXT_CONTAINER_TOP);
    STYLE_DECL(CONTROL_TEXT_VALUE);
    STYLE_DECL(CONTROL_TEXT_NAME);
    STYLE_DECL(CONTROL_BUTTON);
    STYLE_DECL(CONTROL_BUTTON_TEXT);
    STYLE_DECL(DIVIDER);
}

#endif //UVENT_MAIN_DISPLAY_H
