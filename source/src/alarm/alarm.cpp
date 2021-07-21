/**
 * Code modified from MIT Emergency Ventilator Controller
 */

#include <Arduino.h>
#include "alarm.h"

Alarm::Alarm(const String& default_text, const int& min_bad_to_trigger,
             const int& min_good_to_clear, const AlarmLevel& alarm_level) : text_(default_text),
                                                                            min_bad_to_trigger_(min_bad_to_trigger),
                                                                            min_good_to_clear_(min_good_to_clear),
                                                                            alarm_level_(alarm_level)
{}

void Alarm::reset()
{
    // *this = Alarm::Alarm(text_, min_bad_to_trigger_, min_good_to_clear_, alarm_level_);
}

void Alarm::setCondition(const bool& bad, const unsigned long& seq)
{
    if (bad) {
        consecutive_bad_ += (seq != last_bad_seq_);
        last_bad_seq_ = seq;
        if (!on_) {
            on_ = consecutive_bad_ >= min_bad_to_trigger_;
        }
        consecutive_good_ = 0;
    }
    else {
        consecutive_good_ += (seq != last_good_seq_);
        last_good_seq_ = seq;
        if (on_) {
            on_ = consecutive_good_ < min_good_to_clear_;
        }
        consecutive_bad_ = 0;
    }
}

void Alarm::setText(const String& text)
{
}

void AlarmManager::begin()
{
    speaker_.begin();
}

void AlarmManager::update()
{
    // displ_->setAlarmText(getText());
    AlarmLevel highest_level = getHighestLevel();
    speaker_.update(highest_level);
}

void AlarmManager::allOff()
{
    for (int i = 0; i < NUM_ALARMS; i++) {
        alarms_[i].reset();
    }
}

int AlarmManager::numON() const
{
    int num = 0;
    for (int i = 0; i < NUM_ALARMS; i++) {
        num += (int) alarms_[i].isON();
    }
    return num;
}

AlarmLevel AlarmManager::getHighestLevel() const
{
    AlarmLevel alarm_level = NO_ALARM;
    for (int i = 0; i < NUM_ALARMS; i++) {
        if (alarms_[i].isON()) {
            alarm_level = max(alarm_level, alarms_[i].alarmLevel());
        }
    }
    return alarm_level;
}

void AlarmManager::snooze()
{
}