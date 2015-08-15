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

#define BMP085_ADDRESS      0x77

#include <Barometer.h>
#include <EepromBasedWiredDevice.h>

class BMP085Barometer: public Barometer, public EepromBasedWiredDevice {

    /*
     * OSS selects which mode the BMP085 operates in, and can be set to either 0, 1, 2, or 3.
     * OSS determines how many samples the BMP085 will take before it sends over its uncompensated
     * pressure reading. With OSS set to 0, the BMP085 will consume the least current.
     * Setting OSS to 3 increases resolution, as it samples pressure eight times before
     * producing a reading, this comes at a cost of more power usage. If you want to change OSS,
     * just set it accordingly at the top of the program. Try changing OSS to 3,
     * does the data become more stable?
     */
    const unsigned char OSS;

    // Pressure at sea level (Pa)
    const float PRESSURE_AT_SEA_LEVEL;

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
    long b5;

public:

    BMP085Barometer();

    /**
     * Temperature in °C, in steps of 0.1°C.
     */
    short getTemperature();

    /**
     * Pressure in hPa, in steps of 1Pa (= 0.01hPa = 0.01mbar).
     */
    long getPressure();

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
     * Reads the uncompensated temperature.
     *
     * UT = temperature data (16 bit)
     */
    unsigned int readUncompensatedTemperature();

    /**
     * Reads the uncompensated pressure.
     *
     * UP = pressure data (16 to 19 bit)
     */
    unsigned long readUncompensatedPressure();
};

#endif // __ARDUINO_BAROMETER_DRIVER_BMP085_BAROMETER_H__
