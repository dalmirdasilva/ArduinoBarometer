/**
 * Arduino - Barometer Driver
 *
 * BMP085Barometer.h
 *
 * BMP085Barometer class.
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#ifndef __ARDUINO_BAROMETER_DRIVER_BMP085_BAROMETER_H__
#define __ARDUINO_BAROMETER_DRIVER_BMP085_BAROMETER_H__ 1

#include <Barometer.h>
#include <EepromBasedWiredDevice.h>

#define BMP085_ADDRESS          0x77

// https://www.raspberrypi.org/forums/viewtopic.php?t=64618&p=476195
#define PRESSURE_ADJUSTMENT     0

// Pressure at sea level (Pa)
#define PRESSURE_AT_SEA_LEVEL	101325 + PRESSURE_ADJUSTMENT

class BMP085Barometer: public Barometer, public EepromBasedWiredDevice {

    enum Register {
        CALIBRATION_AC1 = 0xaa,
        CALIBRATION_AC2 = 0xac,
        CALIBRATION_AC3 = 0xae,
        CALIBRATION_AC4 = 0xb0,
        CALIBRATION_AC5 = 0xb2,
        CALIBRATION_AC6 = 0xb4,
        CALIBRATION_B1 = 0xb6,
        CALIBRATION_B2 = 0xb8,
        CALIBRATION_MB = 0xba,
        CALIBRATION_MC = 0xbc,
        CALIBRATION_MD = 0xbe,
        CHIP_ID = 0xd0,
        VERSION = 0xd1,
        SOFTWARE_RESET = 0xe0,
        CONTROL = 0xf4,
        TEMPERATURE_DATA = 0xf6,
        PRESSURE_DATA = 0xf6,
        READ_TEMPERATURE_COMMAND = 0x2e,
        READ_PRESSURE_COMMAND = 0x34
    };

    struct Calibration {
        short ac1;
        short ac2;
        short ac3;
        unsigned short ac4;
        unsigned short ac5;
        unsigned short ac6;
        short b1;
        short b2;
        short mb;
        short mc;
        short md;
    };

    const static unsigned char MAX_RETRIES_ON_READING = 10;

    /*
     * OSS selects which mode the BMP085 operates in, and can be set to either 0, 1, 2, or 3.
     * OSS determines how many samples the BMP085 will take before it sends over its uncompensated
     * pressure reading. With OSS set to 0, the BMP085 will consume the least current.
     * Setting OSS to 3 increases resolution, as it samples pressure eight times before
     * producing a reading, this comes at a cost of more power usage. If you want to change OSS,
     * just set it accordingly at the top of the program. Try changing OSS to 3,
     * does the data become more stable?
     */
    unsigned char oversampling;

    Calibration calibration;

public:

    enum {
        ULTRA_LOW_POWER = 0,
        STANDARD = 1,
        HIGH_RESOLUTION = 2,
        ULTRA_HIGH_RESOLUTION = 3
    } OversamplingMode;

    /**
     * Public constructor.
     */
    BMP085Barometer();

    /**
     * Public constructor.
     *
     * @param oversampling
     */
    BMP085Barometer(unsigned char oversampling);

    /**
     * Temperature in °C, in steps of 0.1°C.
     */
    short getTemperature();

    /**
     * Pressure in hPa, in steps of 1Pa (= 0.01hPa = 0.01mbar).
     */
    long getPressure();

    /**
     * Altitude.
     */
    float getAltitude();

private:

    /**
     * The 176 bit EEPROM is partitioned in 11 words of 16 bit each. These contain 11 calibration
     * coefficients. Every sensor module has individual coefficients. Before the first calculation of
     * temperature and pressure, the master reads out the EEPROM data.
     * The data communication can be checked by checking that none of the words has the value 0 or 0xFFFF.
     */
    void readCallibration();

    /**
     * Reads uncompensated temperature value.
     *
     * UT = temperature data (16 bit)
     *
     * write 0x2E into reg 0xF4, wait 4.5ms
     * read reg 0xF6 (MSB), 0xF7 (LSB)
     * UT = MSB << 8 + LSB
     */
    unsigned int readUncompensatedTemperature();

    /**
     * Reads uncompensated pressure value.
     *
     * UP = pressure data (16 to 19 bit)
     *
     * Steps:
     *
     * write 0x34+(oss<<6) into reg 0xF4, wait
     * read reg 0xF6 (MSB), 0xF7 (LSB), 0xF8 (XLSB)
     * UP = (MSB<<16 + LSB<<8 + XLSB) >> (8-oss)
     */
    unsigned long readUncompensatedPressure();

    /**
     * b5 is needed to measure pressure and temperature
     */
    long computeB5(unsigned int ut);
};

#endif // __ARDUINO_BAROMETER_DRIVER_BAROMETER_H__
