#include "Chatouilleur.h"

Chatouilleur::Chatouilleur(const char* serviceUUID, const char* intensityUUID, const char* positionUUID, const char* widthUUID, 
                           int motorPins[], int numberOfMotors, int ledPin, int numLeds) 
  : serviceUUID(serviceUUID), 
    intensityUUID(intensityUUID), 
    positionUUID(positionUUID), 
    widthUUID(widthUUID), 
    myService(serviceUUID),
    intensityCharacteristic(intensityUUID, BLERead | BLEWrite),
    positionCharacteristic(positionUUID, BLERead | BLEWrite),
    widthCharacteristic(widthUUID, BLERead | BLEWrite),
    motorPins(motorPins), 
    numberOfMotors(numberOfMotors), 
    ledPin(ledPin), 
    numLeds(numLeds) 
{
    leds = new CRGB[numLeds];
}

void Chatouilleur::init() {
    for (int i = 0; i < numberOfMotors; i++) {
        ledcSetup(i, freq, resolution);
        ledcAttachPin(motorPins[i], i);
        ledcWrite(i, 0);
    }
    FastLED.addLeds<WS2811, ledPin, GRB>(leds, numLeds).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(maxLedsBrightness);
}

void Chatouilleur::startMotors() {
    for (int i = 0; i < numberOfMotors; i++) {
        ledcWrite(i, startIntensity);
        delay(250);
        ledcWrite(i, idleIntensity);
    }
}

void Chatouilleur::stopMotors() {
    for (int i = 0; i < numberOfMotors; i++) {
        ledcWrite(i, 0);
    }
}

void Chatouilleur::updateChatouille(uint8_t intensity, uint8_t position, uint8_t width) {
    for (int i = 0; i < numberOfMotors; i++) {
        int motorPosition = map(i, 0, numberOfMotors - 1, 0, 255);
        int distance = abs(position - motorPosition);
        double scaledDistance = (double)distance / width;
        double exponentialFalloff = exp(-scaledDistance * scaledDistance);
        int motorIntensity = idleIntensity + (intensity - idleIntensity) * exponentialFalloff;
        motorIntensity = constrain(motorIntensity, idleIntensity, 255);
        motorIntensity = motorIntensity * voltageCoeff;
        ledcWrite(i, motorIntensity);
    }
}

void Chatouilleur::updateLEDColors(uint8_t intensity, uint8_t position, uint8_t width) {
    uint8_t startHue = 192;
    uint8_t endHue = 0;
    uint8_t saturation = 255;
    uint8_t value = 255;

    for (int i = 0; i < numLeds; i++) {
        int ledPosition = map(i, 0, numLeds - 1, 0, 255);
        int distance = abs(position - ledPosition);
        double scaledDistance = (double)distance / width;
        double exponentialFalloff = exp(-scaledDistance * scaledDistance);
        int hueRange = startHue - endHue;
        uint8_t hueOffset = (uint8_t)(exponentialFalloff * intensity * hueRange / 255);
        uint8_t currentHue = startHue - hueOffset;
        leds[i] = CHSV(currentHue, saturation, value);
    }
    FastLED.show();
}

void Chatouilleur::flashLed(int delayTime) {
    // Fait clignoter la LED bleue quand non connecté
    // digitalWrite(blueLedsPin, HIGH);
    // delay(delayTime);
    // digitalWrite(blueLedsPin, LOW);
    // delay(delayTime);
}

BLEDevice Chatouilleur::connectBLE() {
    if (!BLE.begin()) {
        Serial.println("Starting BLE failed!");
        while (true);
    }

    BLE.setLocalName("Chatouilleur");
    BLE.setAdvertisedService(myService);
    myService.addCharacteristic(intensityCharacteristic);
    myService.addCharacteristic(positionCharacteristic);
    myService.addCharacteristic(widthCharacteristic);
    BLE.addService(myService);
    BLE.advertise();
    return BLE.central();
}
