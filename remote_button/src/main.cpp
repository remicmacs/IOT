#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

HTTPClient http;

#define LED_OUT 12
#define BUTTON_IN 27

const char* ssid = "APESP32";
const char* password = "rootroot";

bool light = false;
int previous = LOW;

void handleResponse(int httpCode) {
  if (httpCode > 0) { //Check for the returning code
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
  } else {
    Serial.println("Error on HTTP request");
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
    digitalWrite(LED_OUT, LOW);
    delay(100);
    digitalWrite(LED_OUT, HIGH);
    delay(100);
  }

  Serial.println("Connected to network");

}

void setup() {
  Serial.begin(115200);
  pinMode(LED_OUT, OUTPUT);
  pinMode(BUTTON_IN, INPUT_PULLDOWN);

  WiFi.scanNetworks();
  connectToNetwork();

  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
  http.begin("http://192.168.43.145/");

  int httpCode = http.GET();
  handleResponse(httpCode);
  digitalWrite(LED_OUT, HIGH);
}

void toggle() {
  http.begin("http://192.168.43.145/get");
  int httpCde = http.GET();
  if (httpCde > 0) { //Check for the returning code
    String payload = http.getString();
    light = payload.equals("true");
    Serial.println(httpCde);
    Serial.println(light);
  } else {
    Serial.println("Error on HTTP request");

  }
  int httpCode = -1;
  if (light) {
    digitalWrite(LED_OUT, HIGH);
    http.begin("http://192.168.43.145/off");

  } else {
    digitalWrite(LED_OUT, LOW);
    http.begin("http://192.168.43.145/on");
  }
  light = !light;

  httpCode = http.GET(); //Make the request
  handleResponse(httpCode);
}

void loop() {
  int state = digitalRead(BUTTON_IN);
  if (previous != state && state == HIGH) {
    toggle();
  }

  previous = state;
  delay(100);
}