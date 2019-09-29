#include <Arduino.h>

#include <esp32-hal-log.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

BLEUUID UUID_REMOTE_SERVICE("d7cdcf85-c8bc-4ee9-b3f5-20d713ed7316");
BLEUUID UUID_REMOTE_CHARACTERISTIC("61c6017c-8c49-4e58-8cac-8b04ba8f7a75");

// Pointeurs disponibles dans le scope global
BLEScan * BLEScan_ptr;
BLERemoteCharacteristic * remote_charac_ptr;

// Flags globals

bool connected = false;
bool device_is_found = false;

BLEAdvertisedDevice * found_device;

class CustomBLEClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient * client_ptr) {
        connected = true;
        Serial.println("Connecté au service");
    }

    void onDisconnect(BLEClient * client_ptr) {
        connected = false;
        Serial.println("Déconnecté du service");
    }
};

bool connect_to_service() {
    Serial.print("Connexion au service ");
    Serial.println(found_device->getAddress().toString().c_str());

    BLEClient * client_ptr = BLEDevice::createClient();
    Serial.println("Client créé");

    // Association des callbacks clients custom
    client_ptr->setClientCallbacks(new CustomBLEClientCallback());

    Serial.println("Callbacks enregistrés");

    Serial.printf("Le service est %s",
        client_ptr->isConnected() ? "connecté" : "déconnecté"
    );

    // Connexion au service BLE
    client_ptr->connect(found_device);
    Serial.println("Connecté au service");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService * remote_service_ptr = client_ptr->getService(
        UUID_REMOTE_SERVICE
    );
    if (remote_service_ptr == nullptr) {
        Serial.print("Échec de la recherche du service #");
        Serial.println(UUID_REMOTE_SERVICE.toString().c_str());
        client_ptr->disconnect();

        delete client_ptr;
        return false;
    }
    Serial.println("Service trouvé");

    // Maintenant qu'on est sûrs d'avoir le bon service, on cherche la bonne charactéristique
    remote_charac_ptr = remote_service_ptr->getCharacteristic(
        UUID_REMOTE_CHARACTERISTIC
    );
    if (remote_charac_ptr == nullptr) {
        Serial.print("Échec de la recherche de la carac. #");
        Serial.println(UUID_REMOTE_CHARACTERISTIC.toString().c_str());
        client_ptr->disconnect();
        delete client_ptr;
        return false;
    }
    Serial.println("Caractéristique trouvée");

    // Lisons la valeur
    if(remote_charac_ptr->canRead()) {
      std::string value = remote_charac_ptr->readValue();
      Serial.print("La valeur est : ");
      Serial.println(value.c_str());
    }

    Serial.println("Fin de la séquence de connexion");

    return true;
}

// On implémente la méthode virtuelle `onResult` pour afficher les informations
// du service trouvé lors d'un scan
class FoundAdvertisedDeviceCallback: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertised_device) {
        Serial.printf(
            "Device disponible : %s \n",
            advertised_device.toString().c_str()
        );

        // Est-ce bien le service recherché ?
        if (advertised_device.haveServiceUUID() &&
            advertised_device.isAdvertisingService(UUID_REMOTE_SERVICE)
        ) {
            device_is_found = true;

            BLEDevice::getScan()->stop();
            found_device = new BLEAdvertisedDevice(advertised_device);
        }
    }
};

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);

    esp_log_level_set("*", ESP_LOG_VERBOSE);
    Serial.println("Initialisation du scan...");

    BLEDevice::init("Mon_Client_BLE");

    BLEScan_ptr = BLEDevice::getScan();
    // On enregistre le callback créé
    // Maintenant chaque service découvert sera affiché en console
    BLEScan_ptr->setAdvertisedDeviceCallbacks(
        new FoundAdvertisedDeviceCallback()
    );

    // Le scan en mode actif est plus rapide mais consomme plus d'énergie
    BLEScan_ptr->setActiveScan(true);

    BLEScan_ptr->setInterval(100);

    // La fenêtre de scan doit être moins longue ou exactement égale à l'intervale
    BLEScan_ptr->setWindow(100);

    while(!device_is_found) {
        // Durée du scan en secondes
        const uint32_t scan_duration = 5;
        BLEScanResults foundDevices = BLEScan_ptr->start(scan_duration, false);
        Serial.print("Nombre de devices trouvées: ");
        Serial.println(foundDevices.getCount());

        Serial.println("Scan fini !");
        // Ne pas oublier de nettoyer le buffer de résultats !
        BLEScan_ptr->clearResults();
    }

}

void loop() {

    if (device_is_found && !connected) {
        connect_to_service();

        if (!connected) {
            Serial.println("Connexion a échoué, il y a un problème...");
        }
    }

    if (connected) {
        String new_value = "Secondes depuis le boot: " + String(millis()/1000);
        Serial.println(
            "Rafraichissement de la valeur : \"" + new_value + "\""
        );

        remote_charac_ptr->writeValue(new_value.c_str(), new_value.length());
        Serial.println("Fin du rafraichissement");
    }
    delay(1000);
}