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

    int ac1;
    int ac2;
    int ac3;
    unsigned int ac4;
    unsigned int ac5;
    unsigned int ac6;
    int b1;
    int b2;
    int mb;
    int mc;
    int md;
    long b5;

public:

    BMP085Barometer();

    virtual unsigned short getTemperature();

    virtual unsigned short getPressure();

    virtual unsigned short getAltitude();

private:

    void readCallibration();

    /**
     * Reads the uncompressed temperatur.
     */
    unsigned int readTemperature();

    unsigned long readPressure();
};

#endif // __ARDUINO_BAROMETER_DRIVER_BMP085_BAROMETER_H__
