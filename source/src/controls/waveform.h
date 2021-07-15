#ifndef UVENT_WAVEFORM_H
#define UVENT_WAVEFORM_H

struct waveform_params {
    float tCycleTimer;// Absolute time (s) at start of each breathing cycle
    float tIn;        // Calculated time (s) since tCycleTimer for end of IN_STATE
    float tHoldIn;    // Calculated time (s) since tCycleTimer for end of HOLD_IN_STATE
    float tEx;        // Calculated time (s) since tCycleTimer for end of EX_STATE
    float tPeriod;    // Calculated time (s) since tCycleTimer for end of cycle

    uint16_t bpm;   // Breaths per minute
    float volume_ml;// Tidal volume
    float ie_i;     // Inspiration of IE ratio
    float ie_e;     // Expiration of IE ratio
};

class Waveform {
public:
    waveform_params* get_params();
    int8_t calculate_waveform();
    bool is_inspiration_done();
    bool is_inspiration_hold_done();
    bool is_expiration_done();
    bool is_peep_pause_done();
    void display_details() const;

private:
    const float HOLD_IN_DURATION = 0.1;// Duration (s) to pause after inhalation
    const float MIN_PEEP_PAUSE = 0.05; // Time (s) to pause after exhalation / before watching for an assisted inhalation
    const float MAX_EX_DURATION = 1.00;// Maximum exhale duration (s)

    waveform_params params = {
            .tCycleTimer = 0,
            .tIn = 0.0,
            .tHoldIn = 0.0,
            .tEx = 0.0,
            .tPeriod = 0.0,
            .bpm = 15,
            .volume_ml = 1060,
            .ie_i = 1.0,
            .ie_e = 1.0};
};
#endif//UVENT_WAVEFORM_H