#include <WiFi.h>
#include <WebServer.h>

// Ganti dengan WiFi/hotspot yang dipakai
const char* ssid = "TI Atas";
const char* password = "ubyunggul";

const int LED_PIN = 6;
bool ledState = false;

WebServer server(80);

void setup() {
Serial.begin(115200);
pinMode(LED_PIN, OUTPUT);
digitalWrite(LED_PIN, LOW);
WiFi.mode(WIFI_STA);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}

Serial.print("IP ESP32: ");
Serial.println(WiFi.localIP());
server.on("/", handleRoot);
server.on("/on", handleOn);
server.on("/off", handleOff);
server.begin();
}

void loop() {
 server.handleClient();
}

void handleOn() {
ledState = true;
digitalWrite(LED_PIN, HIGH);
server.sendHeader("Location", "/");
server.send(303);
}

void handleOff() {
ledState = false;
digitalWrite(LED_PIN, LOW);
server.sendHeader("Location", "/");
server.send(303);
}

void handleRoot() {
String status = ledState ? "MENYALA" : "MATI";
String html = "<!DOCTYPE html><html><head>";
html += "<meta name='viewport' content='width=device-width'>";
html += "<title>Kontrol LED</title></head><body>";
html += "<h1>Kontrol LED ESP32</h1>";
html += "<p>Status: " + status + "</p>";
html += "<a href='/on'><button>ON</button></a> ";
html += "<a href='/off'><button>OFF</button></a>";
html += "</body></html>";
server.send(200, "text/html", html);
}


