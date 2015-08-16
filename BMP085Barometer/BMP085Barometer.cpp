#include "BMP085Barometer.h"
#include <Arduino.h>

BMP085Barometer::BMP085Barometer()
        : EepromBasedWiredDevice(BMP085_ADDRESS, 0x01, EepromBasedWiredDevice::BIG_ENDIAN) {

    // 10ms of start-up time after power-up, before first communication
    delay(10);
    readCallibration();
}

short BMP085Barometer::getTemperature() {
    unsigned int ut;
    long x1, x2;
    ut = readUncompensatedTemperature();

    // Got from datasheet... ugly calculations.
    x1 = (((long) ut - (long) ac6) * (long) ac5) >> 15;
    x2 = ((long) mc << 11) / (x1 + md);
    b5 = x1 + x2;
    return ((b5 + 8) >> 4);
}

long BMP085Barometer::getPressure() {
    unsigned long up;
    long x1, x2, x3, b3, b6, preasure;
    unsigned long b4, b7;

    // b6 depends on the getTemperature be called first
    // to get fresher b5
    getTemperature();
    b6 = b5 - 4000;

    up = readUncompensatedPressure();

    // Calculate b3
    x1 = (b2 * (b6 * b6) >> 12) >> 11;
    x2 = (ac2 * b6) >> 11;
    x3 = x1 + x2;
    b3 = (((((long) ac1) * 4 + x3) << OSS) + 2) >> 2;

    // Calculate b4
    x1 = (ac3 * b6) >> 13;
    x2 = (b1 * ((b6 * b6) >> 12)) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    b4 = (ac4 * (unsigned long) (x3 + 32768)) >> 15;

    b7 = ((unsigned long) (up - b3) * (50000 >> OSS));
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
    altitude = (float) 44330 * (1 - pow(((float) pressure / PRESSURE_AT_SEA_LEVEL), 0.190295));
    return altitude;
}

void BMP085Barometer::readCallibration() {
    readBlock(0xaa, (unsigned char*) &ac1, 2);
    readBlock(0xac, (unsigned char*) &ac2, 2);
    readBlock(0xae, (unsigned char*) &ac3, 2);
    readBlock(0xb0, (unsigned char*) &ac4, 2);
    readBlock(0xb2, (unsigned char*) &ac5, 2);
    readBlock(0xb4, (unsigned char*) &ac6, 2);
    readBlock(0xb6, (unsigned char*) &b1, 2);
    readBlock(0xb8, (unsigned char*) &b2, 2);
    readBlock(0xba, (unsigned char*) &mb, 2);
    readBlock(0xbc, (unsigned char*) &mc, 2);
    readBlock(0xbe, (unsigned char*) &md, 2);
}

unsigned int BMP085Barometer::readUncompensatedTemperature() {
    unsigned int temperature;

    // Write 0x2e into register 0xf4
    // This requests a temperature reading
    unsigned char temperatureRequest[] = { 0x2e };
    writeBlock(0xf4, temperatureRequest, 0x01);

    // Wait at least 4.5ms
    delay(5);

    // Read two bytes from registers 0xf6 and 0xf7
    readBlock(0xf6, (unsigned char *) &temperature, 0x02);
    return temperature;
}

unsigned long BMP085Barometer::readUncompensatedPressure() {

    unsigned long pressure = 0;

    // Write 0x34+(OSS<<6) into register 0xf4
    // Request a pressure reading w/ over-sampling setting
    unsigned char pressureRequest[] = { (unsigned char) (0x34 + (OSS << 6)) };
    writeBlock(0xf4, pressureRequest, 0x01);

    // Wait for conversion, delay time dependent on OSS
    delay(2 + (3 << OSS));

    // Read register 0xf6 (MSB), 0xf7 (LSB), and 0xf8 (XLSB)
    readBlock(0xf6, (unsigned char *) &pressure, 0x03);

    // Adjust XLSB based on the over-sampling
    pressure = (pressure & ~0xff) | (pressure & 0xff) >> (8 - OSS);

    return pressure;
}
