#include "command.h"
#include "controls/control.h"
#include "controls/machine.h"
#include "utilities/logging.h"
#include <Arduino.h>
#include <limits.h>

enum class Error_Codes {
    ER_NONE,
    ER_NOT_ENOUGH_ARGS,
    ER_INVALID_ARG
};

static void command_help(int argc, char** argv);
static void command_actuator(int argc, char** argv);
static void command_state(int argc, char** argv);
static void command_eeprom(int argc, char** argv);

/* Command response, with error code. */
static void print_response(Error_Codes error)
{
    serial_printf("e %d\n", error);
}

/* Convert the incoming string to a long(using strtol).
 * Return false if the string cannot be parsed to a long.
 */
inline bool sanitize_input(const char* str, int32_t* result)
{
    char* pEnd;// Pointer for strtol use.

    *result = strtol(str, &pEnd, 10);// Base 10

    /* If the end pointer is the same as the start of str pointer,
     * no parsing has taken place. Return false, else parsing was a success.
     */
    return !(pEnd == str);
}

/* Convert the incoming string to a float(using strtof).
 * Return false if the string cannot be parsed to a long.
 */
inline bool sanitize_input(const char* str, float* result)
{
    char* pEnd;// Pointer for strtol use.

    *result = strtof(str, &pEnd);

    /* If the end pointer is the same as the start of str pointer,
     * no parsing has taken place. Return false, else parsing was a success.
     */
    return !(pEnd == str);
}

/* Command list with their associated help text */
command_type commands[] =
        {
                {"help", command_help, "\t\tDisplay a list of commands.\r\n"},
                {"actuator", command_actuator, "\tActuator related commands.\r\n"},
                {"state", command_state, "\t\tState related commands.\r\n"},
                {"ee", command_eeprom, "\t\tEEPROM related commands.\r\n"}};

uint16_t const command_array_size = sizeof(commands) / sizeof(command_type);

/* Help function. */
static void
command_help(int argc, char** argv)
{
    unsigned int i;

    for (i = 0; i < command_array_size; i++) {
        Serial.print(commands[i].name);
        Serial.print(commands[i].help);
    }
}

/* Actuator function. */
static void
command_actuator(int argc, char** argv)
{
    // Check is help is requested for this command or no arguments were included.
    if (!(strcmp(argv[1], "help")) || (argc == 1)) {
        Serial.println("Format: actuator command");
        Serial.println("home     - Homes the paddle.");
        Serial.println("zero     - Sets the current actuator position as home.");
        Serial.println("pos      - Gets the current actuator position.");
        Serial.println("pos_raw  - Gets the current raw data from the angle sensor .");
        Serial.println("mv_deg   - Moves the actuator to a position(degrees).");
        Serial.println("mv_steps - Moves the actuator by no. of steps(steps).");
        Serial.println("volume   - Get the tidal volume from the Ambu Bag (liters).");
        Serial.println("enable   - Enable/Disable the drive.");
    }
    else if (!(strcmp(argv[1], "home"))) {
        control_change_state(States::ST_ACTUATOR_HOME);
        print_response(Error_Codes::ER_NONE);
        return;
    }
    else if (!(strcmp(argv[1], "zero"))) {
        control_zero_actuator_position();
        print_response(Error_Codes::ER_NONE);
        return;
    }
    else if (!(strcmp(argv[1], "pos"))) {
        Serial.println(control_get_actuator_position(), DEC);
        return;
    }
    else if (!(strcmp(argv[1], "pos_raw"))) {
        Serial.println(control_get_actuator_position_raw(), DEC);
        return;
    }
    else if (!(strcmp(argv[1], "mv_deg"))) {
        if (argc == 2) {// Not enough arguments.
            print_response(Error_Codes::ER_NOT_ENOUGH_ARGS);
            return;
        }
        if (!(strcmp(argv[2], "help"))) {
            Serial.println("Format: mv_deg angle speed");
            Serial.println("angle - The angle(int) of the shaft in degrees");
            Serial.println("speed - The speed(int) in degrees per second.");
            return;
        }

        int32_t req_angle;
        int32_t req_speed;

        // Check if the strings can be parsed. If False, abort.
        if (!(sanitize_input(argv[2], &req_angle)) || !(sanitize_input(argv[3], &req_speed))) {
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }

        // Values should be between 0 and 360 degrees.
        if ((req_angle > 360) || (req_angle < -360)) {
            // Invalid request
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }
        else if ((req_speed > 180) || (req_speed < 0)) {
            // Invalid request
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }
        else {
            //change state to jog
            control_change_state(States::ST_ACTUATOR_JOG);
            control_actuator_manual_move(Tick_Type::TT_DEGREES, req_angle, req_speed);
        }
    }
    else if (!(strcmp(argv[1], "mv_steps"))) {
        if (argc == 2) {
            // Not enough arguments.
            print_response(Error_Codes::ER_NOT_ENOUGH_ARGS);
            return;
        }
        if (!(strcmp(argv[2], "help"))) {
            Serial.println("Format: mv_steps angle speed");
            Serial.println("angle - The angle(int) of the shaft in degrees");
            Serial.println("speed - The speed(int) in degrees per second.");
            return;
        }

        int32_t req_steps;
        int32_t req_speed;

        // Check if the strings can be parsed. If False, abort.
        if (!(sanitize_input(argv[2], &req_steps)) || !(sanitize_input(argv[3], &req_speed))) {
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }

        if ((req_steps > LONG_MAX) || (req_steps < LONG_MIN)) {
            // Invalid request
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }
        else if ((req_speed > STEPPER_MAX_STEPS_PER_SECOND) || (req_speed < -STEPPER_MAX_STEPS_PER_SECOND)) {
            // Invalid request
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }
        else {
            // Change state to jog
            control_change_state(States::ST_ACTUATOR_JOG);
            control_actuator_manual_move(Tick_Type::TT_STEPS, req_steps, req_speed);
        }
    }
    else if (!(strcmp(argv[1], "volume"))) {
        if (argc == 2) {
            // Not enough arguments.
            print_response(Error_Codes::ER_NOT_ENOUGH_ARGS);
            return;
        }
        if (!(strcmp(argv[2], "help"))) {
            Serial.println("Format: volume compliance");
            Serial.println("compliance - The compliance of the artificial lung.");
            Serial.println("Options are: none, 20, or 50");
            return;
        }

        if ((strcmp(argv[2], "none")) && (strcmp(argv[2], "None")) && (strcmp(argv[2], "NONE")) && (strcmp(argv[2], "20")) && (strcmp(argv[2], "50"))) {
            // Invalid request
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }

        int dec_place = 3;
        if (!(strcmp(argv[2], "none")) || !(strcmp(argv[2], "None")) || !(strcmp(argv[2], "NONE"))) {
            Serial.println(control_get_volume(C_Stat::NONE), dec_place);
            return;
        }
        else if (!(strcmp(argv[2], "20"))) {
            Serial.println(control_get_volume(C_Stat::TWENTY), dec_place);
            return;
        }
        else if (!(strcmp(argv[2], "50"))) {
            Serial.println(control_get_volume(C_Stat::FIFTY), dec_place);
            return;
        }
        else {
            // Invalid request
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }
    }
    else if (!(strcmp(argv[1], "enable"))) {
        if (!(strcmp(argv[2], "help"))) {
            Serial.println("Format: enable 0/1");
            return;
        }
        else if (!(strcmp(argv[2], "1"))) {
            control_actuator_set_enable(true);
            print_response(Error_Codes::ER_NONE);
            return;
        }
        else if (!(strcmp(argv[2], "0"))) {
            control_actuator_set_enable(false);
            print_response(Error_Codes::ER_NONE);
            return;
        }
        else {
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }
    }
}

/* State function. */
static void
command_state(int argc, char** argv)
{
    // Check is help is requested for this command or no arguments were included.
    if (!(strcmp(argv[1], "help")) || (argc == 1)) {
        Serial.println("Format: state command");
        Serial.println("list      - Lists all the states.");
        Serial.println("which     - Returns the current state ID.");
        Serial.println("which_str - Returns the current state string.");
        Serial.println("switch    - Force switch to a state.");
    }
    else if (!(strcmp(argv[1], "which"))) {
        Serial.println((uint16_t) control_get_state());
        return;
    }
    else if (!(strcmp(argv[1], "which_str"))) {
        Serial.println(control_get_state_string());
        return;
    }
    else if (!(strcmp(argv[1], "list"))) {

        uint8_t state_list_size = 0;
        const char** p = control_get_state_list(&state_list_size);

        // Spool through the list.
        for (uint8_t i = 0; i < state_list_size; i++) {
            // Print the index and state string
            serial_printf("%d: %s\r\n", i, *p);
            p++;
        }

        return;
    }
    else if (!(strcmp(argv[1], "switch"))) {
        if (argc == 2) {
            // Not enough arguments.
            print_response(Error_Codes::ER_NOT_ENOUGH_ARGS);
            return;
        }
        if (!(strcmp(argv[2], "help"))) {
            Serial.println("Format: switch state_id");
            Serial.println("state_id - The index of the state");
            return;
        }

        int32_t req_state;

        // Check if the strings can be parsed. If False, abort.
        if (!(sanitize_input(argv[2], &req_state))) {
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }

        // Values should be between 0 max state count.
        if ((req_state > (int16_t) States::ST_COUNT) || (req_state < 0)) {
            // Invalid request
            print_response(Error_Codes::ER_INVALID_ARG);
            return;
        }
        else {
            //change state to jog
            control_change_state((States) req_state);
        }
    }
}

/* EEPROM function. */
static void
command_eeprom(int argc, char** argv)
{
    // Check is help is requested for this command or no arguments were included.
    if (!(strcmp(argv[1], "help")) || (argc == 1)) {
        Serial.println("Format: state command");
        Serial.println("dump      - Dumps eeprom contents.");
    }
    else if (!(strcmp(argv[1], "dump"))) {
        control_display_storage();
        return;
    }
}

command_type* command_get_array(void)
{
    return commands;
}

uint16_t command_get_array_size()
{
    return command_array_size;
}