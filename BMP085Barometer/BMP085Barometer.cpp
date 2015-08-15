#include "BMP085Barometer.h"
#include <Arduino.h>

BMP085Barometer::BMP085Barometer()
        : EepromBasedWiredDevice(BMP085_ADDRESS, 0x01, EepromBasedWiredDevice::BIG_ENDIAN), OSS(0), PRESSURE_AT_SEA_LEVEL(101325) {
    readCallibration();
}

unsigned short BMP085Barometer::getTemperature() {

}

unsigned short BMP085Barometer::getPressure() {

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

unsigned int BMP085Barometer::readTemperature() {
    unsigned int temperature;

    // Write 0x2E into Register 0xF4
    // This requests a temperature reading
    unsigned char temperatureRequest[] = { 0x2e };
    writeBlock(0xf4, temperatureRequest, 0x01);

    // Wait at least 4.5ms
    delay(5);

    // Read two bytes from registers 0xF6 and 0xF7
    readBlock(0xf6, (unsigned char *) &temperature, 0x02);
    return temperature;
}

unsigned long BMP085Barometer::readPressure() {

    unsigned long pressure = 0;

    // Write 0x34+(OSS<<6) into register 0xF4
    // Request a pressure reading w/ over-sampling setting
    unsigned char pressureRequest[] = { (unsigned char)(0x34 + (OSS << 6)) };
    writeBlock(0xf4, pressureRequest, 0x01);

    // Wait for conversion, delay time dependent on OSS
    delay(2 + (3 << OSS));

    // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
    readBlock(0xf6, (unsigned char *) &pressure, 0x03);

    // Adjust over-sampling
    pressure = (pressure & ~0xff) | (pressure & 0xff) >> (8 - OSS);

    return pressure;
}
