
#include <Arduino.h>
#include "machine.h"

// Set the current state in the state machine
void Machine::set_state(States new_state)
{
    state_first_entry = true;
    state = new_state;

    // Reset the timer before each state starts.
    machine_timer = 0;
}

// State functions
void Machine::state_startup()
{
    // For testing. Each tick is 100ms(CONTROL_HANDLER_PERIOD_US)
    if (machine_timer > 10)
    {
        set_state(States::ST_INSPR);
    }
}

void Machine::state_inspiration()
{

}

void Machine::state_inspiration_hold()
{

}

void Machine::state_expiration()
{

}

void Machine::state_expiration_hold()
{

}
void Machine::state_peep_pause()
{

}

void Machine::state_paddle_home()
{

}

void Machine::state_off()
{

}

void Machine::run()
{
    // Increment the soft timer.
    machine_timer++;

    // State Machine
    switch (state)
    {
        case States::ST_STARTUP:
            state_startup();
            break;
        case States::ST_INSPR:
            state_inspiration();
            break;
        case States::ST_INSPR_HOLD:
            state_inspiration_hold();
            break;
        case States::ST_EXPR:
            state_expiration();
            break;
        case States::ST_PEEP_PAUSE:
            state_peep_pause();
            break;
        case States::ST_EXPR_HOLD:
            state_expiration_hold();
            break;
        case States::ST_PADLE_HOME:
            state_paddle_home();
            break;
        case States::ST_OFF:
            state_off();
            break;
        default:
            break;
    }
}

void Machine::setup()
{
    // Initial state
    state = States::ST_STARTUP;
}

const char* Machine::get_current_state_string()
{
    // State Machine
    switch (state)
    {
        case States::ST_STARTUP:
            return("Startup");
        case States::ST_INSPR:
            return("Inspiration");
            break;
        case States::ST_INSPR_HOLD:
            return("Inspiration Hold");
            break;
        case States::ST_EXPR:
            return("Expiration");
            break;
        case States::ST_PEEP_PAUSE:
            return("Peep Pause");
            break;
        case States::ST_EXPR_HOLD:
            return("Expiration Hold");
            break;
        case States::ST_PADLE_HOME:
            return("Paddle Home");
            break;
        case States::ST_OFF:
            return("Off");
            break;
        default:
            break;
    }
    return("Invalid State");
}

States Machine::get_current_state()
{
    return state;
}