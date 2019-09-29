# TD BLE

**ATTENTION** : Ce TD se fait avec l'**ESP32** car le BLE n'est pas disponible sur l'**ESP8266** sans ajouter un module externe.

## Service BLE Basique

Pour créer un serveur BLE, il faut inclure les headers suivants :

```cpp
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
```

Le principe du BLE est qu'il est structuré autour de la logique GATT (_Generic ATTributes_).

![GATT](GATT-BLE-ESP32.png)

Il va donc falloir créer un **serveur**, puis déclarer un **service**. On attachera ensuite une **caractéristique** à ce service, elle-même sera doté d'un **descripteur**.

### Créer un server

On déclare tous les objets nécessaires dans la méthode `setup()`.

```cpp
// Nom du serveur Bluetooth qui sera diffusé
BLEDevice::init("Mon_Service_BLE");

// Création du serveur
BLEServer * server_ptr = BLEDevice::createServer();
```

### Créer un service

Un service est un regroupement d'informations, comme par exemple la donnée d'un capteur. La liste officielle des services définie par le SIG est disponible [ici](https://www.bluetooth.com/specifications/gatt/services/).

Un ou plusieurs services compose un **profil**. Plusieurs profils sont déjà définis dans le standard BLE.

Le service créé va avoir besoin d'un UUID. On peut en générer un grâce à [ce site](https://www.uuidgenerator.net/).

On définit l'UUID ainsi obtenu pour qu'il soit facilement disponible dans notre code.

```cpp
#define UUID_SERVICE "d7cdcf85-c8bc-4ee9-b3f5-20d713ed7316"
```

#### Caractéristique

La caractéristique va avoir besoin d'un UUID aussi.

```cpp
#define UUID_CHARACTERISTIC "61c6017c-8c49-4e58-8cac-8b04ba8f7a75"
```

La liste des propriétés qu'on peut attribuer à une caractéristique GATT est la suivante :

* Broadcast (Diffuser) : permet de diffuser cette donnée dans des paquets de type "advertising" (`BLECharacteristic::BLEBroadcast`)
* Lire : autorise les clients à lire cette donnée (`BLECharacteristic::BLERead`)
* Modifier (Write) : autorise les clients à modifier cette donnée, avec confirmation (`BLECharacteristic::BLEWrite`)
* Modifier sans réponse : autorise les clients à modifier cette donnée, sans confirmation du serveur `BLECharacteristic::BLEWriteWithoutResponse`
* Notifier : permet au serveur d'attacher un handler de notification au changement de cette donnée `BLECharacteristic::BLENotify`
* Indiquer : permet au serveur d'attacher un handler d'indication au changement de cette donnée`BLECharacteristic::BLEIndicate`

Différence **notify** vs **indicate** : **notify** fait juste remonter l'information à la manière d'une notification, alors que **indicate** suppose que le client recevant l'indication doive renvoyer un `ACK` pour confirmer réception.

D'autres propriétés GATT existent pour assurer une protection des données, à savoir :

* Modifier de manière signée (Signed Write Command)
* **Queued Write** : ajouter une modification à une file

mais ces propriétés ne sont pas implémentées dans la librairie Arduino que nous utilisons.

Pour déclarer les propriété d'une caractéristique, il suffit de le faire à l'instantiation :

```cpp
// Création du service
BLEService * service_ptr = server_ptr->createService(UUID_SERVICE);

// Création de la caractéristique (~= donnée associée au service)
BLECharacteristic * charact_ptr = service_ptr->createCharacteristic(
    UUID_CHARACTERISTIC,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
);
```

Ici on crée une caractéristique avec les propriété `READ` et `WRITE`.

On peut maintenant enregistrer une valeur dans la caractéristique avec `setValue`.

#### Démarrer le service

Il faut maintenant :

1. Démarrer le service
2. Créer l'_advertising_
3. Enregistrer le service créé dans l'_advertising_
4. Démarrer l'_advertising_

```cpp
service_ptr->start();

// On diffuse les paquets d'advertising qui vont rendre visible notre service
BLEAdvertising * advertising_ptr = BLEDevice::getAdvertising();
advertising_ptr->addServiceUUID(UUID_SERVICE);
advertising_ptr->setScanResponse(true);

BLEDevice::startAdvertising();
```

### Tester

Pour voir le service associé au serveur et les caractéristiques qu'il contient, on utilise l'application Android [nrf Connect for mobile](https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp&hl=en_US). Si quelqu'un connaît une alternative, je suis intéressé.

Normalement il est possible de modifier la valeur via le téléphone et la consulter sur le serveur de l'ESP avec `getValue`.

**Indice** : il faut réussir à partager le pointeur sur la caractéristique entre la fonction `setup` et la fonction `loop`.

**Indice n°2** : `getValue` retourne une `String` C++ et `Serial.println` ne va pas forcément aimer.

## Scan client

Un service sans client n'est pas très intéressant. On va créer un client capable de scanner les différents services BLE disponible et de se connecter au bon pour en consulter la valeur.

Pour cet exercice, les headers suivants sont requis :

```cpp
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
```

### Création du scanner

On doit d'abord initialiser le module BLE puis créer un objet qui se chargera du scan :

```cpp

BLEDevice::init("Mon_Client_BLE");

BLEScan * BLEScan_ptr = BLEDevice::getScan();
```

### Préparation du scan

Pour afficher les informations associées aux services BLE découverts, on utilise un callback appelé lors de l'événement `onResult` de la classe `BLEAdvertisedDeviceCallbacks`

```cpp
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

```

On peut ensuite enregistrer le callback dans l'objet scanner :

```cpp
// On enregistre le callback créé
// Maintenant chaque service découvert sera affiché en console
BLEScan_ptr->setAdvertisedDeviceCallbacks(
    new FoundAdvertisedDeviceCallback()
);
```

Ces quelques paramètres supplémentaires peuvent être renseignés, mais je n'ai pas vu de différence sans :

```cpp
// Le scan en mode actif est plus rapide mais consomme plus d'énergie
BLEScan_ptr->setActiveScan(true);

BLEScan_ptr->setInterval(100);

// La fenêtre de scan doit être moins longue ou exactement égale à l'intervale
BLEScan_ptr->setWindow(100);
```

On démarre le scan dans la fonction `loop` :

```cpp
// Durée du scan en secondes
const uint32_t scan_duration = 5;
BLEScanResults foundDevices = BLEScan_ptr->start(scan_duration, false);
```

Vous devriez voir passer des résultats en console.

## Lire / Écrire avec un client

En repartant du code client précédent, on va lire la valeur proposée par le service, et la modifier.

### Connexion

Pour se connecter on a besoin d'un objet client, et de quelques flags accessibles dans le scope global.

On créé des callbacks ad hoc pour avoir l'information de la connexion :

```cpp
class CustomBLEClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient * client_ptr) {
        connected = true;
        Serial.println("Connected to service");
    }

    void onDisconnect(BLEClient * client_ptr) {
        connected = false;
        Serial.println("Disconnected from service");
    }
};
```


### Récupération de la valeur

## Credits

[Neil Kolban](https://github.com/nkolban/esp32-snippets/)

[Random Nerd Tutorials](https://randomnerdtutorials.com/esp32-bluetooth-low-energy-ble-arduino-ide/)