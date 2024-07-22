#include "DS3231Wrapper.h"
#include <wiringPi.h>
#include <iostream>
#include <ctime>
#include <unistd.h>

void DS3231Wrapper::displayTimeAndDate() {
    std::tm time = getTime();
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &time);
    std::cout << "Current Date & time: " << buffer << std::endl;
}

void DS3231Wrapper::displayTemperature() {
    float temperature = getTemperature();
    std::cout << "Current temperature: " << temperature << " C" << std::endl;
}

void DS3231Wrapper::setCurrentTimeAndDate(const std::tm &time) {
    setTime(time);
    std::cout << "Time and date set successfully." << std::endl;
}

void DS3231Wrapper::setAlarm1(const std::tm &time, uint8_t mode) {
    DS3231::setAlarm1(time, mode);
    std::cout << "Alarm 1 set." << std::endl;
}

void DS3231Wrapper::setAlarm2(const std::tm &time, uint8_t mode) {
    DS3231::setAlarm2(time, mode);
    std::cout << "Alarm 2 set." << std::endl;
}

void DS3231Wrapper::enableAlarm1Interrupt(int ledPin) {
    pinMode(ledPin, OUTPUT);
    DS3231::enableAlarm1Interrupt();
    handleInterrupt(ledPin);
}

void DS3231Wrapper::enableAlarm2Interrupt(int ledPin) {
    pinMode(ledPin, OUTPUT);
    DS3231::enableAlarm2Interrupt();
    handleInterrupt(ledPin);
}

void DS3231Wrapper::setSquareWaveFrequency(uint8_t frequency) {
    DS3231::setSquareWave(frequency);
    std::cout << "Square wave frequency set." << std::endl;
}

void DS3231Wrapper::scheduleRecurringEvent(const std::tm &time, int interval, void (*callback)()) {
    lastAlarmTime = time;
    alarmInterval = interval;
    eventCallback = callback;
    eventScheduled = true;
}

void DS3231Wrapper::checkScheduledEvents() {
    if (eventScheduled) {
        std::tm currentTime = getTime();
        // Assuming that `lastAlarmTime` and `currentTime` can be compared
        if (difftime(mktime(&currentTime), mktime(&lastAlarmTime)) >= alarmInterval) {
            eventCallback();
            lastAlarmTime = currentTime; // Reset the last alarm time
        }
    }
}

void DS3231Wrapper::handleInterrupt(int ledPin) {
    if (digitalRead(ledPin) == HIGH) {
        digitalWrite(ledPin, LOW);
    } else {
        digitalWrite(ledPin, HIGH);
    }
}
