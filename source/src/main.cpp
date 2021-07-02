#include <DueTimer.h>
#include <utilities/logging.h>
#include "display/test_display.h"
#include "../config/uvent_conf.h"
#include "controls/control.h"
#include "display/TftDisplay.h"
#include "display/test_display.h"
#include "sensors/pressure_sensor.h"
#include "sensors/test_pressure_sensors.h"
#include "display/main_display.h"
#include "utilities/parser.h"
#include "eeprom/test_eeprom.h"

TftDisplay tft_display = {TFT_CS, TFT_RST, TOUCH_INT, TOUCH_RST};

// Parser instance to parse commands on the serial line
Parser parser;
test_eeprom eeprom_test;

void setup()
{
    Serial.begin(SERIAL_BAUD_RATE);

    NVIC_EnableIRQ(DMAC_IRQn);

    if (!tft_display.init()) {
        while (1)
            ;
    }

    control_init();

#if ENABLE_TEST_DISPLAY
    setup_test_display();
#else
    init_main_display();
#endif

    // Initialize the parser, with the command array and command array size
    parser.init(command_get_array(), command_get_array_size());
}

void loop()
{
    tft_display.update();
    delay(5);

#if ENABLE_TEST_DISPLAY
    update_test_display();
#endif

    parser.service();

#if ENABLE_TEST_PRESSURE_SENSORS
    output_pressure_test();
#endif

#if ENABLE_TEST_EEPROM
    eeprom_test.select_test();
    delay(1000);
#endif

    control_service();
}

ISR(DMAC_Handler)
{
    tft_display.onDMAInterrupt();
}