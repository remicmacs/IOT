#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

HTTPClient http;

#define BUTTON_IN 27

const char* ssid = "APESP32";
const char* password = "rootroot";
String endpoint = "http://192.168.43.145/";

int previous = LOW;

/**
 * Display the payload of a HTTP response.
 */
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
  WiFi.scanNetworks();
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Establishing connection to WiFi..");
  }

  Serial.println("Connected to network");

}

void setup() {
  Serial.begin(115200);

  // Set the button pin as input
  pinMode(BUTTON_IN, INPUT_PULLDOWN);

  connectToNetwork();

  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());

  // Change IP here according to the other ESP's IP
  // http.begin(endpoint);
  // int httpCode = http.GET();
  // handleResponse(httpCode);

}

/**
 * Called when the button is pressed
 */
void toggle() {
  bool light = false;
  http.begin(endpoint + "get");
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
    http.begin(endpoint + "off");

  } else {
    http.begin(endpoint + "on");
  }

  httpCode = http.GET();
  handleResponse(httpCode);
}

void loop() {
  delay(100);
  int state = digitalRead(BUTTON_IN);
  if (previous != state && state == HIGH) {
    Serial.println("Toggle");
    // toggle();
  }

  previous = state;

  Serial.printf("%s\n", (HIGH == state) ? "HIGH" : "LOW");
}
