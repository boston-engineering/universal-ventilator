#include <DueTimer.h>
#include <utilities/logging.h>
#include <function_timings.h>
#include <display/screens/screen.h>
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
MainScreen screen;

// Parser instance to parse commands on the serial line
Parser parser;
test_eeprom eeprom_test;

// TODO clean up setup & main loop

void setup()
{
    Serial.begin(SERIAL_BAUD_RATE);


    if (!tft_display.init()) {
        while (1);
    }

#if ENABLE_CONTROL
    control_init();
#endif

#if ENABLE_TEST_DISPLAY
    setup_test_display();
#else
    init_adjustable_values();
    screen.init();
    screen.select_screen();
    init_main_display();
    screen.setup();
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
    //test_sensors_read_pressure(250, true, Units_pressure::cmH20, Units_pressure::mbar);
    //test_sensors_read_gauge(250, true, Units_pressure::cmH20);
    //test_sensors_read_differential(250, true, Units_pressure::mbar);
    //test_sensors_read_flow(250, true, Order_type::first, units_flow::lpm);
    //test_sensors_read_flow(250, true, Order_type::second, units_flow::lpm);
    test_sensors_read_flow(100, true, Order_type::third, units_flow::lpm);
#endif

#if ENABLE_TEST_EEPROM
    eeprom_test.select_test();
    delay(1000);
#endif

    control_service();

    loop_test_readout();
}

#if USE_DMA_INTERRUPT

ISR(DMAC_Handler)
{
    RA_DEBUG_STOP(SPI_TIMING);
    RA_DEBUG_START(INTERRUPT);
    tft_display.onDMAInterrupt();
    RA_DEBUG_STOP(INTERRUPT);
}

#endif