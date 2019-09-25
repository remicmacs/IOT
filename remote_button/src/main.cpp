#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

HTTPClient http;

#define LED_OUT 12
#define BUTTON_IN 27

const char* ssid = "APESP32";
const char* password = "rootroot";
String endpoint = "http://192.168.43.145/";


bool light = false;
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

  WiFi.scanNetworks();
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
    digitalWrite(LED_OUT, HIGH);
    http.begin(endpoint + "off");

  } else {
    digitalWrite(LED_OUT, LOW);
    http.begin(endpoint + "on");
  }
  light = !light;

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
