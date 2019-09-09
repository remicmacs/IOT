#include <Arduino.h>
#include <WiFi.h>

const char* ssid = "APESP32";
const char* password = "rootroot";

const int LED = 2;

const uint ServerPort = 23;
WiFiServer Server(ServerPort);

WiFiClient RemoteClient;

/**
 * Connection routine for telnet clients
 */
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
    } else {
      Serial.println("Connection accepted");
      RemoteClient = Server.available();
    }
  }
}

/**
 * Client handling routine
 */
void listen() {
  char * buffer;
  uint8_t inputBuffer[30];
  while (RemoteClient.connected() && RemoteClient.available())
  {
    // Number of bytes received
    int received = RemoteClient.read(inputBuffer, sizeof(inputBuffer));

    // Processing needed size of buffer
    int needed = snprintf(NULL, 0, "Received %d bytes", received);
    // Allocating buffer
    buffer = (char *) malloc(needed + 1);

    // Displaying buffer
    sprintf(buffer, "Received %d bytes", received);
    Serial.println(buffer);
    Serial.println((char *)inputBuffer);

    free(buffer);

      // Allocating buffer
      buffer = (char *) malloc(received-1);
      strncpy(buffer, (char *) inputBuffer, received-2);
      buffer[received-2] = '\0';

    // Find if a command is found in the message
    if (strcmp(buffer, "On") == 0) {
      Serial.println("Found On");
      digitalWrite(LED, HIGH);
    } else if (strcmp(buffer, "Off") == 0) {
      Serial.println("Found Off");
      digitalWrite(LED, LOW);
    } else {
      Serial.print("Command not implemented yet : \"");
      Serial.print(buffer);
      Serial.println("\"");
    }
  }
}

void connectToNetwork() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Establishing connection to WiFi..");
  }

  // Blink three times if connection to WiFi is successful
  int blink = 3;
  while (blink-- > 0) {
    digitalWrite(LED, HIGH);
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