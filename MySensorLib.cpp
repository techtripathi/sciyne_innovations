#include "MySensorLib.h"

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

BLEServer *pServer = nullptr;
BLECharacteristic *pCharacteristic = nullptr;
bool deviceConnected = false;

#define SERVICE_UUID        "12345678-1234-5678-1234-56789abcdef0"
#define CHARACTERISTIC_UUID "abcd1234-ab12-cd34-ef56-abcdef123456"

// Handle client connection/disconnection
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

// Handle write from BLE central (like nRF Connect)
class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) override {
    std::string value = pCharacteristic->getValue();
    if (!value.empty()) {
      Serial.print("Received Value from App: ");
      Serial.println(value.c_str());
    }
  }
};

MySensorLib::MySensorLib(int pin) {
  _pin = pin;
}

void MySensorLib::sciyne_init() {
  Serial.begin(115200);
  delay(1000);

  // BLE init
  BLEDevice::init("ESP32_Soil");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_NOTIFY |
    BLECharacteristic::PROPERTY_WRITE
  );

  pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

  pCharacteristic->setValue("0");
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("BLE Soil Sensor ready...");
}

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
