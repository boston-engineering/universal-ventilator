#include <Arduino.h>
#include "command.h"

static void command_help(int argc, char** argv);

/* Command list with their associated help text */
command_type commands[] =
        {
                {"help", command_help, "\tDisplay a list of commands.\r\n"}};

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

command_type* command_get_array(void)
{
    return commands;
}

uint16_t command_get_array_size()
{
    return command_array_size;
}