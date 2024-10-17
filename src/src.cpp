
#include <Arduino.h>
#include "define.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// BLE settings
#define SERVICE_UUID "12345678-1234-5678-1234-56789abcdef0"
#define CHAR_UUID1 "12345678-1234-5678-1234-56789abcdef1"
#define CHAR_UUID2 "12345678-1234-5678-1234-56789abcdef2"

// BLE characteristics
BLECharacteristic *pCharacteristic1;
BLECharacteristic *pCharacteristic2;

void setup()
{
    Serial.begin(115200);
    BLEDevice::init("ESP32 Transmitter");

    pinMode(2, OUTPUT);

    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);

    pCharacteristic1 = pService->createCharacteristic(
        CHAR_UUID1,
        BLECharacteristic::PROPERTY_NOTIFY);
    pCharacteristic1->addDescriptor(new BLE2902());

    pCharacteristic2 = pService->createCharacteristic(
        CHAR_UUID2,
        BLECharacteristic::PROPERTY_NOTIFY);
    pCharacteristic2->addDescriptor(new BLE2902());

    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->start();

    // Get the MAC address of the ESP32
    String macAddress = BLEDevice::getAddress().toString().c_str();
    Serial.print("ESP32 Transmitter MAC Address: ");
    Serial.println(macAddress);

    Serial.println("Transmitter is ready.");
}

int potPinX = 36;
int potPinY = 39;

uint8_t data1, data2;
uint8_t data1Old = 90, data2Old = 90;

int ADC_Max = 4096;

void loop()
{
    digitalWrite(2, HIGH);
    data1 = map(analogRead(potPinX), 0, ADC_Max, 0, 180);
    data2 = map(analogRead(potPinY), 0, ADC_Max, 0, 180);

    if (data1 != data1Old)
    {
        pCharacteristic1->setValue(&data1, 1);
        pCharacteristic1->notify();
        data1Old = data1;
    }

    if (data2 != data2Old)
    {
        pCharacteristic2->setValue(&data2, 1);
        pCharacteristic2->notify();
        data2Old = data2;
    }

    delay(100);
    digitalWrite(2, LOW);
    delay(400);
}
