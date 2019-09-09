#include <Arduino.h>
#include <WiFi.h>
#include <inttypes.h>

const char* ssid = "APESP32";
const char* password =  "rootroot";

const int LED = 2;

const uint ServerPort = 23;
WiFiServer Server(ServerPort);

WiFiClient RemoteClient;

void checkForConnections()
{
  if (Server.hasClient())
  {
    // If we are already connected to another computer, 
    // then reject the new connection. Otherwise accept
    // the connection. 
    if (RemoteClient.connected())
    {
      Serial.println("Connection rejected");
      Server.available().stop();
    }
    else
    {
      Serial.println("Connection accepted");
      RemoteClient = Server.available();
    }
  }
}

void listen() {
  uint8_t ReceiveBuffer[30];
  while (RemoteClient.connected() && RemoteClient.available())
  {
    int Received = RemoteClient.read(ReceiveBuffer, sizeof(ReceiveBuffer));
    Serial.println((char *)ReceiveBuffer);

    if (strstr((char *) ReceiveBuffer, "On") != NULL) {
      Serial.println("Found On");
      digitalWrite(LED, HIGH);
    } else if (strstr((char *) ReceiveBuffer, "Off") != NULL) {
      Serial.println("Found Off");
      digitalWrite(LED, LOW);
    }
  }
}
void connectToNetwork() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Establishing connection to WiFi..");
  }

  int blink = 3;
  while (blink-- > 0) {
    digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
  }

  Server.begin();
  Serial.println("Connected to network");

}

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);

  WiFi.scanNetworks();
  connectToNetwork();

  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
}

void loop() {
    checkForConnections();
    listen();
}