#ifndef UVENT_MACHINE_H
#define UVENT_MACHINE_H

void machine_run();
void machine_setup();

enum class States
{
    ST_STARTUP,
    ST_INSPR,
    ST_INSPR_HOLD,
    ST_EXPR,
    ST_PEEP_PAUSE,
    ST_EXPR_HOLD,
    ST_PADLE_HOME,
    ST_OFF
};

class Machine
{
    public:
        void setup();
        void run();
        const char* get_current_state_string();
        States get_current_state();
    private:
        // Default startup state
        States state;

        // Condition to evaluate code on first entry into a state
        bool state_first_entry = false;

        /* An internal timer to take care of any internal state timings.
         * Increments everytime the run function is run to keep time.
         * and resets before change of state.
         */
        uint32_t machine_timer;

        // Set the current state in the state machine
        void set_state(States);

        // State functions
        void state_startup();
        void state_inspiration();
        void state_inspiration_hold();
        void state_expiration();
        void state_peep_pause();
        void state_expiration_hold();
        void state_paddle_home();
        void state_off();
};
#endif