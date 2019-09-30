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

![GATT](https://i2.wp.com/randomnerdtutorials.com/wp-content/uploads/2018/06/GATT-BLE-ESP32.png?w=750&ssl=1)

Il va donc falloir créer un **serveur**, puis déclarer un **service**. On attachera ensuite une **caractéristique** à ce service.

### Créer un server

On déclare tous les objets nécessaires dans la méthode `setup()`.

```cpp
// Nom du serveur Bluetooth qui sera diffusé
BLEDevice::init("Mon_Service_BLE");

// Création du serveur
BLEServer * server_ptr = BLEDevice::createServer();
```

### Créer un service

Un service est un regroupement d'informations, comme par exemple la donnée d'un capteur.

La liste officielle des services définie par le SIG est disponible [ici](https://www.bluetooth.com/specifications/gatt/services/).

Un ou plusieurs services composent un **profil**. Plusieurs profils sont déjà définis dans le standard BLE.

Le service créé va avoir besoin d'un UUID. On peut en générer un grâce à [ce site](https://www.uuidgenerator.net/).

**Attention** : utilisez impérativement un UUID différent de celui des exemples sinon il y aura des **conflits** entre tous les services BLE.

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
* Notifier : permet d'attacher un handler de notification au changement de cette donnée `BLECharacteristic::BLENotify`
* Indiquer : permet d'attacher un handler d'indication au changement de cette donnée`BLECharacteristic::BLEIndicate`

Différence _notify_ vs _indicate_ : _notify_ fait juste remonter l'information à la manière d'une notification, alors que _indicate_ suppose que le client recevant l'indication doive renvoyer un `ACK` pour confirmer réception.

D'autres propriétés GATT existent pour assurer une protection des données, à savoir :

* Modifier de manière signée (_Signed Write Command_)
* _Queued Write_ : ajouter une modification à une file

mais ces propriétés ne sont pas implémentées dans la librairie que nous utilisons.

Pour déclarer les propriétés d'une caractéristique, il suffit de le faire à l'instantiation :

```cpp
// Création du service
BLEService * service_ptr = server_ptr->createService(UUID_SERVICE);

// Création de la caractéristique (~= donnée associée au service)
BLECharacteristic * charact_ptr = service_ptr->createCharacteristic(
    UUID_CHARACTERISTIC,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
);
```

Ici on crée une caractéristique avec les propriétés `READ` et `WRITE`.

On peut maintenant enregistrer une valeur dans la caractéristique avec `setValue`.

#### Démarrer le service

Il faut maintenant :

1. Démarrer le service
2. Créer l'_advertising_ pour que le service soit visible (_discoverable_)
3. Enregistrer le service créé dans l'_advertising_
4. Démarrer l'_advertising_

```cpp
service_ptr->start();

// On enregistre le service qui va être diffusé
BLEAdvertising * advertising_ptr = BLEDevice::getAdvertising();
advertising_ptr->addServiceUUID(UUID_SERVICE);
advertising_ptr->setScanResponse(true);

// On diffuse les paquets d'advertising qui vont rendre visible notre service
BLEDevice::startAdvertising();
```

### Tester

Pour voir le service associé au serveur et les caractéristiques qu'il contient, on utilise l'application Android [nrf Connect for mobile](https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp&hl=en_US). Si quelqu'un connaît une alternative, je suis intéressé.

Normalement il est possible de modifier la valeur via le téléphone et la consulter sur le serveur de l'ESP avec `getValue`.

**Indice** : dans le code serveur, il faut réussir à partager le pointeur sur la caractéristique entre la fonction `setup` et la fonction `loop`.

**Indice n°2** : `getValue` retourne une `String` C++ et `Serial.println` ne va pas forcément aimer.

**Attention** : essayez d'envoyer une quantité de donnée inférieure ou égale à la quantité initiale.

## Scan client

Un service sans client n'est pas très intéressant. On va créer un client capable de scanner les différents services BLE disponibles et de se connecter au bon pour en consulter la valeur.

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
            // Conversion d'une String C++ à un char * C
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

On crée des callbacks _ad hoc_ pour afficher l'information associé au services découverts :

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

On va créer une routine de connexion au service.

Pour se connecter on a besoin de : 

1. Disposer d'un objet représentant le dispositif BLE découvert lors du scan. Dans les exemples de code suivant ce sera la variable `BLEAdvertisedDevice * found_device`
2. Instantier un objet client représentant notre propre _device_
3. Récupérer le service cible s'il est bien associé à la _device_ découverte
4. Trouver la caractéristique correspondant à la valeur qu'on veut consulter

Pour disposer du pointeur `found_device`, à vous de voir si vous préférez le passer en argument ou le mettre dans le scope global, c'est pas un TD de C++, débrouillez-vous.

#### Création du client

```cpp
BLEClient * client_ptr = BLEDevice::createClient();
Serial.println("Client créé");

// Association des callbacks clients custom
client_ptr->setClientCallbacks(new CustomBLEClientCallback());

Serial.println("Callbacks enregistrés");
```

#### Connexion au _device_

```cpp
// Connexion au service BLE
client_ptr->connect(found_device);
Serial.println("Connecté au device");
```

#### Récupération du service

```cpp
// On récupère le pointeur sur le service recherché
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
```

#### Récupération de la caractéristique

```cpp
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

```

#### Savoir si le bon service a été trouvé

On met à jour le callback créé dans l'exercice précédent pour qu'on sache quand lancer la routine de connexion.

```cpp
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

```

### Lecture de la valeur

```cpp
// Lisons la valeur
if(remote_charac_ptr->canRead()) {
  std::string value = remote_charac_ptr->readValue();
  Serial.print("La valeur est : ");
  Serial.println(value.c_str());
}

```

### Modification de la valeur

Dans la fonction `loop`, on va modifier la valeur de la caractéristique distante toutes les secondes.

```cpp

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
```

Attention la condition `if (device_is_found && !connected) ` est importante car c'est ce qui va s'assurer qu'on est bien connecté au service.

## Exercice bonus

Maintenant que vous savez mettre à disposition un service BLE et lire/écrire une caractéristique, vous pouvez réimplémenter l'exercice avec une télécommande de LED mais via BLE !

## Credits

[Neil Kolban](https://github.com/nkolban/esp32-snippets/)

[Random Nerd Tutorials](https://randomnerdtutorials.com/esp32-bluetooth-low-energy-ble-arduino-ide/)

## Code complet

* [Serveur](https://github.com/remicmacs/IOT/blob/master/ble_td/src/main.cpp)
* [Client](https://github.com/remicmacs/IOT/blob/master/ble_client_td/src/main.cpp)