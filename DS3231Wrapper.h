#ifndef DS3231WRAPPER_H
#define DS3231WRAPPER_H

#include "DS3231.h"
#include <ctime>

class DS3231Wrapper : public DS3231 {
public:
    void displayTimeAndDate();
    void displayTemperature();
    void setCurrentTimeAndDate(const std::tm &time);
    void setAlarm1(const std::tm &time, uint8_t mode);
    void setAlarm2(const std::tm &time, uint8_t mode);
    void enableAlarm1Interrupt(int ledPin);
    void enableAlarm2Interrupt(int ledPin);
    void setSquareWaveFrequency(uint8_t frequency);
    void scheduleRecurringEvent(const std::tm &time, int interval, void (*callback)());
    void checkScheduledEvents();

private:
    void handleInterrupt(int ledPin);
    std::tm lastAlarmTime;
    int alarmInterval;
    void (*eventCallback)();
    bool eventScheduled;
};

#endif
