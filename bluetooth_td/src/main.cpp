#include <Arduino.h>

#include "BluetoothSerial.h"

const int LED = 12;
BluetoothSerial ESP_BT;

void setup() {
	ESP_BT.begin("LEDoigtsdelamain"); //Changez ce nom afin de reconnaître facilement votre ESP au moment de l'appareillage
	pinMode (LED, OUTPUT);
}

void loop() {
    if (ESP_BT.available()){
        int incoming = ESP_BT.read();
        if (incoming == 49) // 49 : code ASCII pour '1'
        {
            digitalWrite(LED, HIGH);
            ESP_BT.println("La LED est allumée"); // Permet d'envoyer des données au client.
        }

        if (incoming == 48) // 48 : code ASCII pour '0'
        {
            digitalWrite(LED, LOW);
            ESP_BT.println("La LED est éteinte");
        }
    }
}