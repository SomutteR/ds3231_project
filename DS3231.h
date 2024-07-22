#ifndef DS3231_H
#define DS3231_H

#include <ctime>
#include <cstdint>

class DS3231 {
public:
    void begin();
    std::tm getTime();
    void setTime(const std::tm &time);
    float getTemperature();

    bool checkAlarm1();
    void resetAlarm1Flag();
    bool checkAlarm2();
    void resetAlarm2Flag();

    void setAlarm1(const std::tm &time, uint8_t mode);
    void setAlarm2(const std::tm &time, uint8_t mode);
    void enableAlarm1Interrupt();
    void enableAlarm2Interrupt();
    void setSquareWave(uint8_t frequency);

private:
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
};

#endif
