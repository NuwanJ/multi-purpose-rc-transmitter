
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
    BLEDevice::init("ESP32-C3 Transmitter");

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

    // Get the MAC address of the ESP32-C3
    String macAddress = BLEDevice::getAddress().toString().c_str();
    Serial.print("ESP32 Transmitter MAC Address: ");
    Serial.println(macAddress);

    Serial.println("Transmitter is ready.");
}

void loop()
{
    // Simulate sending 8-bit unsigned integer data on both channels
    uint8_t data1 = random(0, 256); // Random value between 0 and 255
    uint8_t data2 = random(0, 256);

    pCharacteristic1->setValue(&data1, 1);
    pCharacteristic1->notify();

    pCharacteristic2->setValue(&data2, 1);
    pCharacteristic2->notify();

    Serial.printf("Data sent - Channel 1: %d, Channel 2: %d\n", data1, data2);
    delay(1000); // Adjust the delay as needed
}
