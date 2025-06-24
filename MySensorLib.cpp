/*
  MySensorLib.cpp - BLE Soil Sensor Library for ESP32
  ---------------------------------------------------
  This file implements a simple BLE server for soil moisture sensing.
  It allows an ESP32 to advertise as a BLE device, accept connections,
  and send/receive data to/from a BLE central (e.g., mobile app).

  Copyright (c) 2024 Vaibhav / Sciyne Innovations
  SPDX-License-Identifier: MIT

  This library is intended for Arduino/ESP32 developers.
  Use it to quickly prototype BLE-based sensor projects.

  Dependencies:
    - Arduino core for ESP32
    - BLEDevice, BLEUtils, BLEServer libraries

  File: /home/vaibhav/sciyne_innovations/MySensorLib.cpp
*/

#include "MySensorLib.h"

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// Global BLE objects and connection state
BLEServer *pServer = nullptr;
BLECharacteristic *pCharacteristic = nullptr;
bool deviceConnected = false;

// BLE UUIDs for service and characteristic
#define SERVICE_UUID        "12345678-1234-5678-1234-56789abcdef0"
#define CHARACTERISTIC_UUID "abcd1234-ab12-cd34-ef56-abcdef123456"

// Callback class to handle BLE client connection/disconnection events
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) override {
    deviceConnected = true;
    Serial.println("Client connected");
  }

  void onDisconnect(BLEServer *pServer) override {
    deviceConnected = false;
    Serial.println("Client disconnected");
  }
};

// Callback class to handle BLE characteristic write events from central
class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) override {
    std::string value = pCharacteristic->getValue();
    if (!value.empty()) {
      Serial.print("Received Value from App: ");
      Serial.println(value.c_str());
    }
  }
};

// Constructor: initialize sensor pin
MySensorLib::MySensorLib(int pin) {
  _pin = pin;
}

// Initialize BLE server, service, characteristic, and start advertising
void MySensorLib::sciyne_init() {
  Serial.begin(115200);
  delay(1000);

  // Initialize BLE device with a name
  BLEDevice::init("ESP32_Soil");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create BLE service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create BLE characteristic with read, notify, and write properties
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_NOTIFY |
    BLECharacteristic::PROPERTY_WRITE
  );

  // Set callback for characteristic writes
  pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

  // Set initial value and start service
  pCharacteristic->setValue("0");
  pService->start();

  // Start BLE advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("BLE Soil Sensor ready...");
}

// Main loop: send simulated moisture value to BLE client every 2 seconds
void MySensorLib::sciyne_run() {
  static int moisture = 0;
  Serial.print("Sending moisture: ");
  Serial.println(moisture);

  if (deviceConnected) {
    String value = String(moisture);
    pCharacteristic->setValue(value.c_str());
    pCharacteristic->notify();
  }

  moisture++;
  if (moisture >= 100) {
    moisture = 0;
  }

  delay(2000); // send every 2 seconds
}
