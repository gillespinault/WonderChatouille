#include <Arduino.h>
#include "Chatouilleur.h"

// Configuration des pins et variables
const char* serviceUUID = "12345678-1234-5678-1234-56789abcdef0";
const char* intensityUUID = "12345678-1234-5678-1234-56789abcdef1";
const char* positionUUID = "12345678-1234-5678-1234-56789abcdef2";
const char* widthUUID = "12345678-1234-5678-1234-56789abcdef3";

const int motorPins[] = {5, 6, 7, 8, 9, 10, 20, 21}; 
const int numberOfMotors = 8;
const int ledPin = 3;
const int numLeds = 26;

// Instanciation de la classe Chatouilleur
Chatouilleur chatouille(serviceUUID, intensityUUID, positionUUID, widthUUID, motorPins, numberOfMotors, ledPin, numLeds);

void setup() {
    Serial.begin(115200);
    chatouille.init(); // Initialisation du chatouilleur
}

void loop() {
    BLEDevice central = chatouille.connectBLE();
    if (central) {
        Serial.println("Connected to central device");
        chatouille.startMotors(); // Démarrage des moteurs

        while (central.connected()) {
            if (chatouille.intensityCharacteristic.written() || chatouille.positionCharacteristic.written() || chatouille.widthCharacteristic.written()) {
                uint8_t intensity = chatouille.intensityCharacteristic.value();
                uint8_t position = chatouille.positionCharacteristic.value();
                uint8_t width = chatouille.widthCharacteristic.value();
                chatouille.updateChatouille(intensity, position, width);
                chatouille.updateLEDColors(intensity, position, width);
            }
        }

        Serial.println("Disconnected from central device");
        chatouille.stopMotors(); // Arrêt des moteurs
    } else {
        chatouille.flashLed(500);
    }
}
