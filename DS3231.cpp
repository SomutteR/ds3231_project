#include "DS3231.h"
#include <wiringPiI2C.h>
#include <stdexcept>
#include <iostream>

// Define I2C address and register addresses
#define DS3231_ADDRESS 0x68
#define REG_TIME 0x00
#define REG_ALARM1 0x07
#define REG_ALARM2 0x0B
#define REG_CONTROL 0x0E
#define REG_STATUS 0x0F
#define REG_TEMP 0x11

// Define register bits
#define EOSC 0x80
#define BBSQW 0x40
#define CONV 0x20
#define RS2 0x10
#define RS1 0x08
#define INTCN 0x04
#define A2IE 0x02
#define A1IE 0x01
#define OSF 0x80
#define EN32kHz 0x08
#define BSY 0x04
#define A2F 0x02
#define A1F 0x01

int fd;

void DS3231::begin() {
    fd = wiringPiI2CSetup(DS3231_ADDRESS);
    if (fd == -1) {
        throw std::runtime_error("Failed to initialize I2C communication.");
    }
}

std::tm DS3231::getTime() {
    std::tm time;
    uint8_t buffer[7];

    for (int i = 0; i < 7; i++) {
        buffer[i] = readRegister(REG_TIME + i);
    }

    time.tm_sec = (buffer[0] & 0x0F) + ((buffer[0] >> 4) * 10);
    time.tm_min = (buffer[1] & 0x0F) + ((buffer[1] >> 4) * 10);
    time.tm_hour = (buffer[2] & 0x0F) + (((buffer[2] & 0x30) >> 4) * 10);
    time.tm_mday = (buffer[4] & 0x0F) + ((buffer[4] >> 4) * 10);
    time.tm_mon = ((buffer[5] & 0x0F) + ((buffer[5] >> 4) * 10)) - 1;
    time.tm_year = ((buffer[6] & 0x0F) + ((buffer[6] >> 4) * 10)) + 100;
    time.tm_wday = buffer[3] & 0x07;
    time.tm_yday = 0;
    time.tm_isdst = 0;

    return time;
}

void DS3231::setTime(const std::tm &time) {
    writeRegister(REG_TIME + 0, ((time.tm_sec / 10) << 4) + (time.tm_sec % 10));
    writeRegister(REG_TIME + 1, ((time.tm_min / 10) << 4) + (time.tm_min % 10));
    writeRegister(REG_TIME + 2, ((time.tm_hour / 10) << 4) + (time.tm_hour % 10));
    writeRegister(REG_TIME + 3, time.tm_wday & 0x07);
    writeRegister(REG_TIME + 4, ((time.tm_mday / 10) << 4) + (time.tm_mday % 10));
    writeRegister(REG_TIME + 5, (((time.tm_mon + 1) / 10) << 4) + ((time.tm_mon + 1) % 10));
    writeRegister(REG_TIME + 6, (((time.tm_year - 100) / 10) << 4) + ((time.tm_year - 100) % 10));
}

float DS3231::getTemperature() {
    int8_t temp_msb = wiringPiI2CReadReg8(fd, REG_TEMP);
    uint8_t temp_lsb = wiringPiI2CReadReg8(fd, REG_TEMP + 1);
    return temp_msb + ((temp_lsb >> 6) * 0.25);
}

bool DS3231::checkAlarm1() {
    return (readRegister(REG_STATUS) & A1F);
}

void DS3231::resetAlarm1Flag() {
    writeRegister(REG_STATUS, readRegister(REG_STATUS) & ~A1F);
}

bool DS3231::checkAlarm2() {
    return (readRegister(REG_STATUS) & A2F);
}

void DS3231::resetAlarm2Flag() {
    writeRegister(REG_STATUS, readRegister(REG_STATUS) & ~A2F);
}

void DS3231::setAlarm1(const std::tm &time, uint8_t mode) {
    writeRegister(REG_ALARM1 + 0, ((time.tm_sec / 10) << 4) + (time.tm_sec % 10) | (mode << 7));
    writeRegister(REG_ALARM1 + 1, ((time.tm_min / 10) << 4) + (time.tm_min % 10) | (mode << 7));
    writeRegister(REG_ALARM1 + 2, ((time.tm_hour / 10) << 4) + (time.tm_hour % 10) | (mode << 7));
    writeRegister(REG_ALARM1 + 3, ((time.tm_mday / 10) << 4) + (time.tm_mday % 10) | (mode << 7));
}

void DS3231::setAlarm2(const std::tm &time, uint8_t mode) {
    writeRegister(REG_ALARM2 + 0, ((time.tm_min / 10) << 4) + (time.tm_min % 10) | (mode << 7));
    writeRegister(REG_ALARM2 + 1, ((time.tm_hour / 10) << 4) + (time.tm_hour % 10) | (mode << 7));
    writeRegister(REG_ALARM2 + 2, ((time.tm_mday / 10) << 4) + (time.tm_mday % 10) | (mode << 7));
}

void DS3231::enableAlarm1Interrupt() {
    writeRegister(REG_CONTROL, readRegister(REG_CONTROL) | A1IE);
}

void DS3231::enableAlarm2Interrupt() {
    writeRegister(REG_CONTROL, readRegister(REG_CONTROL) | A2IE);
}

void DS3231::setSquareWave(uint8_t frequency) {
    uint8_t control = readRegister(REG_CONTROL);
    control &= ~(RS2 | RS1); // Clear existing rate bits
    control |= (frequency & (RS2 | RS1)); // Set new rate
    writeRegister(REG_CONTROL, control);
}
void DS3231::writeRegister(uint8_t reg, uint8_t value) {
    wiringPiI2CWriteReg8(fd, reg, value);
}

uint8_t DS3231::readRegister(uint8_t reg) {
    return wiringPiI2CReadReg8(fd, reg);
}
