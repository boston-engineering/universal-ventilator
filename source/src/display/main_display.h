#ifndef UVENT_MAIN_DISPLAY_H
#define UVENT_MAIN_DISPLAY_H

#include <lvgl.h>

// LVGL Style Macros
#define SIZEANDALIGN(x, y, w, h, align) \
    lv_obj_set_pos(container, x, y); \
    lv_obj_set_size(container, w, h); \
    lv_obj_set_align(container, align);

// Style, Component, Container, etc. retrieval Macros
/**
 * Gets a container on the screen.
 */
#define SCR_C(idx) containers[DisplayContainer::idx]

// Container Init Macros
#define CONTAINER_INIT_FUNC_NAME(name) name##_init()

#define CONTAINER_INIT_FUNC(name) void CONTAINER_INIT_FUNC_NAME(name)

#define CONTAINER_INIT(name) \
    extern CONTAINER_INIT_FUNC(name); \
    CONTAINER_INIT_FUNC_NAME(name);

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

extern lv_obj_t* containers[];

/**
 * Sets up all containers for the main display
 * The CONTAINER_DECL_BASE and CONTAINER_DECL macros also include init prototypes to scope them within this file.
 * The definitions of these prototypes are in `control_containers.cpp` and `visual_containers.cpp`
 *
 * This function should only be called once at startup
 * */
inline void init_main_display()
{
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
}

#endif //UVENT_MAIN_DISPLAY_H
