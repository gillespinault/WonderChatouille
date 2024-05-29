#ifndef CHATOUILLEUR_H
#define CHATOUILLEUR_H

#include <ArduinoBLE.h>
#include <FastLED.h>
#include <driver/ledc.h>

class Chatouilleur {
public:
    Chatouilleur(const char* serviceUUID, const char* intensityUUID, const char* positionUUID, const char* widthUUID, 
                 int motorPins[], int numberOfMotors, int ledPin, int numLeds);

    void init();
    void startMotors();
    void stopMotors();
    void updateChatouille(uint8_t intensity, uint8_t position, uint8_t width);
    void updateLEDColors(uint8_t intensity, uint8_t position, uint8_t width);
    void flashLed(int delayTime);
    BLEDevice connectBLE();

    BLEUnsignedCharCharacteristic intensityCharacteristic;
    BLEUnsignedCharCharacteristic positionCharacteristic;
    BLEUnsignedCharCharacteristic widthCharacteristic;

private:
    const char* serviceUUID;
    const char* intensityUUID;
    const char* positionUUID;
    const char* widthUUID;
    
    BLEService myService;

    int* motorPins;
    int numberOfMotors;
    int ledPin;
    int numLeds;
    CRGB* leds;
    uint8_t maxLedsBrightness = 16;

    int freq = 10000;
    int resolution = 8;
    int idleIntensity = 10;
    int startIntensity = 128;
    float motorVoltage = 3.3;
    float supplyVoltage = 5;
    float voltageCoeff = motorVoltage / supplyVoltage;
};

#endif
