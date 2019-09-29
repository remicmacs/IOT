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

### Créer un service

Un service est un regroupement d'informations, comme par exemple la donnée d'un capteur. La liste officielle des services définie par le SIG est disponible [ici](https://www.bluetooth.com/specifications/gatt/services/).

...

Le service créé va avoir besoin d'un UUID. Vous pouvez en générer un grâce à [ce site](https://www.uuidgenerator.net/).

On défini l'UUID ainsi obtenu pour qu'il soit facilement disponible dans notre code.

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

Pour déclarer les propriété d'une caractéristique, il suffit de le faire à l'instanciation :

```cpp
BLECharacteristic *charac_ptr = service_ptr->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
);
```

Ici on crée une caractéristique avec les propriété `READ` et `WRITE`

#### Démarrer le service

Pour démarrer le service, la librairie Arduino nous fournit des méthodes statiques utilitaires.

```cpp

```

### Tester

Pour voir le service associé au serveur et les caractéristiques qu'il contient, [nrf Connect for mobile](https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp&hl=en_US)

## Credits

[Neil Kolban](https://github.com/nkolban/esp32-snippets/)

[Random Nerd Tutorials](https://randomnerdtutorials.com/esp32-bluetooth-low-energy-ble-arduino-ide/)