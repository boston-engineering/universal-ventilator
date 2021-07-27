#include <DueTimer.h>
#include <utilities/logging.h>
#include <function_timings.h>
#include <display/screens/screen.h>
#include <utilities/util.h>
#include "../config/uvent_conf.h"
#include "controls/control.h"
#include "display/TftDisplay.h"
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

// Timers
lv_timer_t* update_readout_timer = nullptr;

// TODO clean up setup & main loop

void setup()
{
    Serial.begin(SERIAL_BAUD_RATE);

    if (!tft_display.init()) {
        while (1);
    }

    // Enable stepper, actuator, etc
    // Load EEPROM storage
    control_init();

    /*******************************/
    /* Setup & arrange the display */
    /*******************************/

    // Set up classes with controlled values, load defaults.
    init_adjustable_values();
    // Calculate the waveform from loaded parameters
    control_calculate_waveform();
    // Init the screen object (usually just creates an empty screen)
    screen.init();
    // Tell LVGL this is the currently loaded screen
    screen.select_screen();
    // Init containers, styles, defaults...
    init_main_display();
    // Creates all the components that go on the main screen in order for it to function.
    screen.setup();
    // Arm the speaker so it talks to LVGL on mute/unmute
    control_setup_alarm_cb();

    // Initialize the parser, with the command array and command array size
    parser.init(command_get_array(), command_get_array_size());

    // LVGL Timer to poll sensors and update screen data
#if ENABLE_CONTROL
    update_readout_timer = lv_timer_create(loop_update_readouts, SENSOR_POLL_INTERVAL, &screen);
#else
    // Setup an LVGL timer to loop/update display. Polls sensors, updates graphs, etc.
    update_readout_timer = lv_timer_create(loop_test_readout, SENSOR_POLL_INTERVAL, &screen);
#endif
}

void loop()
{
    tft_display.update();
    delay(5);

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
