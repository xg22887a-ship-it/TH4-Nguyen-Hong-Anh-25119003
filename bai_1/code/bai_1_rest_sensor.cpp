#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>

const char* ssid = "TEN_WIFI";
const char* pass = "MAT_KHAU";

WebServer server(80);
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;

float docNhiet() {
  sensors_event_t am, nhiet;
  aht.getEvent(&am, &nhiet);
  return nhiet.temperature;
}

float docAm() {
  sensors_event_t am, nhiet;
  aht.getEvent(&am, &nhiet);
  return am.relative_humidity;
}

float docApSuat() { return bmp.readPressure() / 100.0; }

void handleSensor() {
  JsonDocument doc;
  doc["id"] = "25119003";
  doc["nhom"] = "nhom3";
  doc["temp"] = round(docNhiet() * 10) / 10.0;
  doc["hum"] = round(docAm() * 10) / 10.0;
  doc["pres_hPa"] = round(docApSuat() * 10) / 10.0;
  doc["uptime"] = millis() / 1000;
  doc["rssi"] = WiFi.RSSI();
  doc["ip"] = WiFi.localIP().toString();
  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
  Serial.printf("GET /api/sensor -> %s\n", out.c_str());
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  Serial.println(aht.begin() ? "AHT20 (0x38) OK" : "AHT20 KHONG TIM THAY");
  Serial.println(bmp.begin(0x77) ? "BMP280 (0x77) OK" : "BMP280 KHONG TIM THAY");
  WiFi.begin(ssid, pass);
  Serial.printf("Dang noi vao %s", ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nIP: ");
  Serial.println(WiFi.localIP());
  server.on("/api/sensor", HTTP_GET, handleSensor);
  server.begin();
  Serial.println("REST server cong 80: GET /api/sensor");
}

void loop() {
  server.handleClient();
}
