// Load Wi-Fi library
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "APESP32_AP";
const char* password = "prootproot";

const int LED = 12;

const uint ServerPort = 80;
WebServer Server(ServerPort);

WiFiClient RemoteClient;

void led_on(){
    Serial.println("Led on");
    digitalWrite(LED, HIGH);
}
void led_off(){
    Serial.println("Led off");
    digitalWrite(LED, LOW);
}

boolean matchcommand(String req, String command){
    if (req.indexOf(command) != -1){
        return true;
    } else {
        return false;
    }
}

void handleOn() {
    Serial.println("Switch On");
    // Server.send(200, "text/html", "Switch On");
    // led_on();
}

void handleOff() {
    Serial.println("Switch Off");
    // Server.send(200, "text/html", "Switch Off");
    // led_off();
}

void homepage() {
    Serial.println("Sending message");
    String s;
    s = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<!DOCTYPE HTML>\n<html><body>\n";
    s +="<script language='javascript' type='text/javascript'>\n";
    s +="<!--\n";
    s +="function on()  { var xhr = new XMLHttpRequest(); xhr.open('GET', '/on',  true); xhr.send();}\n";
    s +="function off() { var xhr = new XMLHttpRequest(); xhr.open('GET', '/off', true); xhr.send();}\n";
    s +="//-->\n";
    s +="</script>\n";
    s +="<table align='center' style='cursor: pointer;'><tr>\n";
    s +="<td height='100' width='100' bgcolor='#f45942' align='center'><font size='20'><div onclick='off();'>Off</div></font></td>\n";
    s +="<td height='100' width='100' bgcolor='#41f492' align='center'><font size='20'><div onclick='on();'>On </div></font></td>\n";
    s += "</tr></table></body></html>\n";
    Server.send(200, "text/html", s);
}

void handleRoot() {
    Server.send(200, "text/plain", "You are connected");
}


void setup() {
    Serial.begin(115200);
    Serial.print("Configuring access point...");
    WiFi.softAP(ssid, password, true, 1);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    Server.on("/", homepage);
    Server.on("/on", handleOn);
    Server.on("/off", handleOff);
    Server.begin();
    Serial.println("HTTP server started");
    led_off();
}

void loop() {
    // Serial.printf("Stations connected = %d\n",WiFi.softAPgetStationNum());
    // Serial.print("AP IP address: ");
    // Serial.println(WiFi.softAPIP());
    Server.handleClient();
}