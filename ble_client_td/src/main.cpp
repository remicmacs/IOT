#include <Arduino.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// Pointeur disponible dans le scope global
BLEScan * BLEScan_ptr;


// On implémente la méthode virtuelle `onResult` pour afficher les informations
// du service trouvé lors d'un scan
class FoundAdvertisedDeviceCallback: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertised_device) {
        Serial.printf(
            "Advertised Device: %s \n",
            advertised_device.toString().c_str()
        );
    }
};

void setup() {
    Serial.begin(115200);
    Serial.println("Scanning...");

    BLEDevice::init("Mon_Client_BLE");

    BLEScan_ptr = BLEDevice::getScan();
    // On enregistre le callback créé
    // Maintenant chaque service découvert sera affiché en console
    BLEScan_ptr->setAdvertisedDeviceCallbacks(
        new FoundAdvertisedDeviceCallback()
    );

    // // Le scan en mode actif est plus rapide mais consomme plus d'énergie
    // BLEScan_ptr->setActiveScan(true);

    // BLEScan_ptr->setInterval(100);

    // // La fenêtre de scan doit être moins longue ou exactement égale à l'intervale
    // BLEScan_ptr->setWindow(100);

}

void loop() {

    // Durée du scan en secondes
    const uint32_t scan_duration = 5;
    BLEScanResults foundDevices = BLEScan_ptr->start(scan_duration, false);
    Serial.print("Number of devices found: ");
    Serial.println(foundDevices.getCount());

    Serial.println("Scan done !");
    // Ne pas oublier de nettoyer le buffer de résultats !
    BLEScan_ptr->clearResults();
    delay(20);
}