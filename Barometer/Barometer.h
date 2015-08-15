/**
 * Arduino - Barometer Driver
 *
 * Barometer.h
 *
 * Barometer class.
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#ifndef __ARDUINO_BAROMETER_DRIVER_BAROMETER_H__
#define __ARDUINO_BAROMETER_DRIVER_BAROMETER_H__ 1

class Barometer {

public:

    /**
     * Returns the current temperature.
     */
    virtual unsigned short getTemperature() = 0;

    /**
     * Returns the current pressure.
     */
    virtual unsigned short getPressure() = 0;

    /**
     * Returns the current altitude.
     */
    virtual unsigned short getAltitude() = 0;
};

#endif // __ARDUINO_BAROMETER_DRIVER_BAROMETER_H__
