#include "interface.h"

/**
 * Adjustable values
 */
AdjustableValue<double> adjustable_values[AdjValueType::ADJ_VALUE_COUNT];

extern const AdjValueParams adj_value_settings[] = {
        {"Tidal Volume",        "vT",         "%d",   "%d",   "mL",    100, 900, 50,  true,  true,  palette_color_1},
        {"Respiration Rate",    "Resp. Rate", "%d",   "%d",   "/min",  8,   30,  2,   true,  true,  palette_color_1},
        {"PEEP Limit (Floor)",  "PEEP",       "%d",   "%d",   "cmH2O", 5,   20,  1,   true,  true,  palette_color_1},
        {"PIP Limit (Ceiling)", "PIP",        "%d",   "%d",   "cmH2O", 15,  40,  1,   true,  true,  palette_color_1},
        // TODO ask Dr. Fischer about exact values for this
        {"Plateau Time",        "Plateau",    "%d",   "%d",   "ms",    200, 800, 50,  false, true,  palette_color_1},
        {"Current Pressure",    nullptr,      "%d",   "%d",   "cmH2O", 5,   40,  0,   true,  false, palette_color_2},
        {"",                    "",           "%.1f", "%.1f", nullptr, 1,   4,   0.1, false, false, palette_color_1},
        {"",                    "",           "%.1f", "%.1f", nullptr, 1,   4,   0.1, false, false, palette_color_1},
};