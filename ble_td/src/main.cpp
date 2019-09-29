#include <Arduino.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define UUID_SERVICE "d7cdcf85-c8bc-4ee9-b3f5-20d713ed7316"
#define UUID_CHARACTERISTIC "61c6017c-8c49-4e58-8cac-8b04ba8f7a75"
BLECharacteristic * charact_ptr ;

void setup() {
  Serial.begin(115200);
  Serial.println("Initialisation du BLE");

  BLEDevice::init("Mon_Service_BLE");

  // Création du serveur
  BLEServer * server_ptr = BLEDevice::createServer();

  // Création du service
  BLEService * service_ptr = server_ptr->createService(UUID_SERVICE);

  // Création de la caractéristique (~= donnée associée au service)
  charact_ptr = service_ptr->createCharacteristic(
    UUID_CHARACTERISTIC,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
  );

  // On stocke la valeur de la charactéristique
  charact_ptr->setValue("Hello BLE !");
  service_ptr->start();

  // On diffuse les paquets d'advertising qui vont rendre visible notre service
  BLEAdvertising * advertising_ptr = BLEDevice::getAdvertising();
  advertising_ptr->addServiceUUID(UUID_SERVICE);
  advertising_ptr->setScanResponse(true);

  BLEDevice::startAdvertising();
  Serial.println("Définition de la caractérstique BLE terminée.");
  Serial.println("Démarrage de l'advertising");
  Serial.println("Le service devrait être visible sur votre téléphone");
}

void loop() {
  Serial.println(charact_ptr->getValue().c_str());
  delay(500);
}