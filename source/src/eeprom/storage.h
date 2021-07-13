#ifndef UVENT_STORAGE_H
#define UVENT_STORAGE_H

#include "../config/uvent_conf.h"
#include "SparkFun_External_EEPROM.h"

// UVent settings structure
struct __attribute__((packed)) uvent_settings {
    /* Julian Date: YYMMDD
     * Lot:         L: A,B,C...
     * Serial:      XXXX
     */
    char serial[12];            // YYMMDDLXXXX
    uint16_t diff_pressure_type;// 0: ±0.07, 1: ±0.09
    uint16_t actuator_home_offset_adc_counts;
    int32_t gpressure_offset_adc_counts;
    int32_t dpressure_offset_adc_counts;
};

class Storage {
public:
    bool init();
    bool is_crc_ok();
    void load_defaults();
    void get_settings(uvent_settings&);
    void set_settings(uvent_settings&);
    void display_storage();

private:
    ExternalEEPROM external_eeprom;

    // Defaults cal. settings
    uvent_settings def_settings = {
            .serial = "210000A0000",
            .diff_pressure_type = PRESSURE_SENSOR_TYPE_0,
            .actuator_home_offset_adc_counts = 0,
            .gpressure_offset_adc_counts = 0,
            .dpressure_offset_adc_counts = 0,
    };

    uint32_t crc_calculate();
};

#endif