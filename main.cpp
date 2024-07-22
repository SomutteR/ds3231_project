#include "DS3231Wrapper.h"
#include <csignal>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <wiringPi.h>

bool keepRunning = true;

void signalHandler(int signum) {
    keepRunning = false;
}

// Define GPIO pin for square wave output
#define SQW_PIN 4 // GPIO pin 4 (WiringPi pin 4)

// Define frequency for the square wave (Hz)
#define FREQUENCY 2 // Set frequency to 2Hz

// Define the duration of each half cycle in microseconds
#define CYCLE_DURATION_US (1000000 / FREQUENCY)
#define HALF_CYCLE_DURATION_US (CYCLE_DURATION_US / 2)

void setupGPIO() {
    // Initialize WiringPi for GPIO setup
    if (wiringPiSetup() == -1) {
        std::cerr << "Failed to initialize WiringPi." << std::endl;
        exit(1);
    }
    pinMode(SQW_PIN, OUTPUT);
    digitalWrite(SQW_PIN, LOW); // Ensure the pin is initially LOW
}

void generateSquareWave() {
    std::cout << "Generating square wave on GPIO pin " << SQW_PIN << " with frequency " << FREQUENCY << "Hz." << std::endl;

    while (keepRunning) {
        digitalWrite(SQW_PIN, HIGH);
        usleep(HALF_CYCLE_DURATION_US); // Wait for half of the cycle duration
        digitalWrite(SQW_PIN, LOW);
        usleep(HALF_CYCLE_DURATION_US); // Wait for the other half of the cycle duration
    }
}

int main() {
    // Setup GPIO pin for square wave
    setupGPIO();

    DS3231Wrapper rtc;

    // Register signal and signal handler
    signal(SIGINT, signalHandler);

    // Display current time and temperature
    rtc.displayTimeAndDate();
    rtc.displayTemperature();

    // Set the time and date
    std::tm newTime = {};
    newTime.tm_year = 2024 - 1900; // Year since 1900
    newTime.tm_mon = 6 - 1;        // Month 0-11
    newTime.tm_mday = 22;
    newTime.tm_hour = 12;
    newTime.tm_min = 0;
    newTime.tm_sec = 0;
    rtc.setTime(newTime);

    // Example: Set Alarm1 to trigger every 14 seconds
    std::tm alarm1Time = rtc.getTime();
    alarm1Time.tm_sec += 14; // Set Alarm 1 to trigger in 14 seconds
    rtc.setAlarm1(alarm1Time, 0x0F); // 0x0F: Alarm 1 triggers every 14 seconds
    rtc.enableAlarm1Interrupt(0); // Assuming 0 is the pin for Alarm 1
    std::cout << "Alarm 1 set to trigger every 14 seconds." << std::endl;

    // Example: Set Alarm2 to trigger every minute
    std::tm alarm2Time = rtc.getTime();
    alarm2Time.tm_sec = 0; // Reset seconds
    alarm2Time.tm_min += 1; // Set Alarm 2 to trigger in the next minute
    rtc.setAlarm2(alarm2Time, 0x07); // 0x07: Alarm 2 triggers every minute
    rtc.enableAlarm2Interrupt(1); // Assuming 1 is the pin for Alarm 2
    std::cout << "Alarm 2 set to trigger every minute." << std::endl;

    // Example: Set square wave frequency to 1Hz using DS3231
    rtc.setSquareWave(0x00); // 0x00 for 1Hz
    std::cout << "Square wave frequency set to 1Hz using DS3231." << std::endl;

    // Start generating square wave using GPIO
    generateSquareWave();

    std::cout << "Program terminated." << std::endl;

    return 0;
}
