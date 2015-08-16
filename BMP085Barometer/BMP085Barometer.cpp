#include "BMP085Barometer.h"
#include <Arduino.h>

BMP085Barometer::BMP085Barometer(unsigned char oversampling)
        : EepromBasedWiredDevice(BMP085_ADDRESS, 0x01, EepromBasedWiredDevice::BIG_ENDIAN), oversampling(oversampling) {

    // 10ms of start-up time after power-up, before first communication
    delay(10);
    readCallibration();
}

BMP085Barometer::BMP085Barometer()
        : BMP085Barometer(ULTRA_LOW_POWER) {
}

short BMP085Barometer::getTemperature() {
    unsigned int ut;
    long b5;
    ut = readUncompensatedTemperature();
    b5 = computeB5(ut);
    return ((b5 + 8) >> 4);
}

long BMP085Barometer::getPressure() {
    unsigned int ut;
    long x1, x2, x3, b3, b5, b6, preasure;
    unsigned long up, b4, b7;

    // b6 depends on the getTemperature be called first
    // to get fresher b5
    ut = readUncompensatedTemperature();
    b5 = computeB5(ut);

    b6 = b5 - 4000;
    up = readUncompensatedPressure();

    // Calculate b3
    x1 = (calibration.b2 * (b6 * b6) >> 12) >> 11;
    x2 = (calibration.ac2 * b6) >> 11;
    x3 = x1 + x2;
    b3 = (((((long) calibration.ac1) * 4 + x3) << oversampling) + 2) >> 2;

    // Calculate b4
    x1 = (calibration.ac3 * b6) >> 13;
    x2 = (calibration.b1 * ((b6 * b6) >> 12)) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    b4 = (calibration.ac4 * (unsigned long) (x3 + 32768)) >> 15;
    b7 = ((unsigned long) (up - b3) * (50000 >> oversampling));
    if (b7 < 0x80000000) {
        preasure = (b7 << 1) / b4;
    } else {
        preasure = (b7 / b4) << 1;
    }
    x1 = (preasure >> 8) * (preasure >> 8);
    x1 = (x1 * 3038) >> 16;
    x2 = (-7357 * preasure) >> 16;
    preasure += (x1 + x2 + 3791) >> 4;
    return preasure;
}

float BMP085Barometer::getAltitude() {
    float altitude;
    long pressure;
    pressure = getPressure();
    altitude = (float) 44330 * (1.0 - pow(((float) pressure / PRESSURE_AT_SEA_LEVEL), 0.190295));
    return altitude;
}

void BMP085Barometer::readCallibration() {
    readBlock(CALIBRATION_AC1, (unsigned char*) &(calibration.ac1), 2);
    readBlock(CALIBRATION_AC2, (unsigned char*) &(calibration.ac2), 2);
    readBlock(CALIBRATION_AC3, (unsigned char*) &(calibration.ac3), 2);
    readBlock(CALIBRATION_AC4, (unsigned char*) &(calibration.ac4), 2);
    readBlock(CALIBRATION_AC5, (unsigned char*) &(calibration.ac5), 2);
    readBlock(CALIBRATION_AC6, (unsigned char*) &(calibration.ac6), 2);
    readBlock(CALIBRATION_B1, (unsigned char*) &(calibration.b1), 2);
    readBlock(CALIBRATION_B2, (unsigned char*) &(calibration.b2), 2);
    readBlock(CALIBRATION_MB, (unsigned char*) &(calibration.mb), 2);
    readBlock(CALIBRATION_MC, (unsigned char*) &(calibration.mc), 2);
    readBlock(CALIBRATION_MD, (unsigned char*) &(calibration.md), 2);
}

unsigned int BMP085Barometer::readUncompensatedTemperature() {

    unsigned int temperature;

    // Write 0x2e into register 0xf4
    // This requests a temperature reading
    unsigned char temperatureRequest[] = { READ_TEMPERATURE_COMMAND };
    writeBlock(CONTROL, temperatureRequest, 0x01);

    // Wait at least 4.5ms
    delay(5);

    // Read two bytes from registers 0xf6 and 0xf7
    readBlock(TEMPERATURE_DATA, (unsigned char *) &temperature, 0x02);
    return temperature;
}

unsigned long BMP085Barometer::readUncompensatedPressure() {

    unsigned long pressure = 0;

    // Write 0x34+(OSS<<6) into register 0xf4
    // Request a pressure reading w/ over-sampling setting
    unsigned char pressureRequest[] = { (unsigned char) (READ_PRESSURE_COMMAND + (oversampling << 0x06)) };
    writeBlock(CONTROL, pressureRequest, 0x01);

    // Wait for conversion, delay time dependent on OSS
    delay(2 + (3 << oversampling));

    // Read register 0xf6 (MSB), 0xf7 (LSB), and 0xf8 (XLSB)
    readBlock(PRESSURE_DATA, (unsigned char *) &pressure, 0x03);

    // Adjust XLSB based on the over-sampling
    pressure >>= (8 - oversampling);
    return pressure;
}

long BMP085Barometer::computeB5(unsigned int ut) {
    long x1, x2;
    x1 = (((long) ut - (long) calibration.ac6) * (long) calibration.ac5) >> 15;
    x2 = ((long) calibration.mc << 11) / (x1 + calibration.md);
    return x1 + x2;
}
