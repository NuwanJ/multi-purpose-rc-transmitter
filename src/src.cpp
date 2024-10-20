
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

uint8_t data1, data2;
uint8_t data1Old = 90, data2Old = 90;

#define ADC_MAX 4096

#define CORRECTION_X 14
#define CORRECTION_Y 35

void setup()
{
    delay(2000);
    Serial.begin(115200);
    Serial.println("ESP32 Transmitter");

    pinMode(PIN_LED_INBUILT, OUTPUT);

    digitalWrite(PIN_LED_INBUILT, LOW);
    delay(500);
    digitalWrite(PIN_LED_INBUILT, HIGH);

    BLEDevice::init("ESP32 Transmitter");
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

void loop()
{
    digitalWrite(PIN_LED_INBUILT, LOW);
    data1 = map(analogRead(PIN_POT_X), 0, ADC_MAX, 180, 0) + CORRECTION_X;
    // data1 = map(data1, CORRECTION_X/2, 180 + CORRECTION_X/2, 0, 180);

    data2 = map(analogRead(PIN_POT_Y), 0, ADC_MAX, 180, 0) + CORRECTION_Y;
    // data2 = map(data2, CORRECTION_Y, 180 + CORRECTION_Y, 0, 180);

    if (abs(data1 - data1Old) > 5)
    {
        Serial.printf("D:%03d S:%03d\n", data1, data2);
        pCharacteristic1->setValue(&data1, 1);
        pCharacteristic1->notify();
        data1Old = data1;
    }

    if (abs(data2 - data2Old) > 5)
    {
        Serial.printf("D:%03d S:%03d\n", data1, data2);
        pCharacteristic2->setValue(&data2, 1);
        pCharacteristic2->notify();
        data2Old = data2;
    }

    digitalWrite(PIN_LED_INBUILT, HIGH);
    delay(50);
}
