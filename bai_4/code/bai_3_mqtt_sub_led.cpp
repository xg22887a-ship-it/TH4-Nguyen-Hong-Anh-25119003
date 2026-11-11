#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>

const char* ssid = "TEN_WIFI";
const char* pass = "MAT_KHAU";

const char* broker = "test.mosquitto.org";
const char* svId = "25119003";
String topicTele = String("aet2022/") + svId + "/telemetry";
String topicLed = String("aet2022/") + svId + "/cmd/led";
const int LED = 5;

WiFiClient wifi;
PubSubClient mqtt(wifi);
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;
String clientId;
bool ledOn = false;

void nhanLenh(char* t, byte* payload, unsigned int len) {
  String msg;
  for (unsigned int i = 0; i < len; i++) msg += (char)payload[i];
  msg.trim();
  msg.toLowerCase();
  Serial.printf("Nhan [%s]: %s\n", t, msg.c_str());
  if (msg == "1" || msg == "on") ledOn = true;
  else if (msg == "0" || msg == "off") ledOn = false;
  else return;
  digitalWrite(LED, ledOn);
  Serial.printf("LED GPIO%d -> %s\n", LED, ledOn ? "BAT" : "TAT");
}

void noiBroker() {
  while (!mqtt.connected()) {
    Serial.printf("Noi broker %s, client id %s ... ", broker, clientId.c_str());
    if (mqtt.connect(clientId.c_str())) {
      Serial.println("OK");
      mqtt.subscribe(topicLed.c_str());
      Serial.printf("Da subscribe %s\n", topicLed.c_str());
    } else {
      Serial.printf("loi %d, thu lai sau 2 s\n", mqtt.state());
      delay(2000);
    }
  }
}

void guiTelemetry() {
  sensors_event_t am, nhiet;
  aht.getEvent(&am, &nhiet);
  JsonDocument doc;
  doc["id"] = svId;
  doc["temp"] = round(nhiet.temperature * 10) / 10.0;
  doc["hum"] = round(am.relative_humidity * 10) / 10.0;
  doc["pres_hPa"] = round(bmp.readPressure() / 10.0) / 10.0;
  doc["uptime"] = millis() / 1000;
  doc["rssi"] = WiFi.RSSI();
  doc["led"] = ledOn ? 1 : 0;
  String out;
  serializeJson(doc, out);
  mqtt.publish(topicTele.c_str(), out.c_str());
  Serial.printf("Publish [%s]: %s\n", topicTele.c_str(), out.c_str());
}

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
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
  clientId = String("esp32-") + svId + "-" + String((uint32_t)ESP.getEfuseMac(), HEX);
  mqtt.setServer(broker, 1883);
  mqtt.setCallback(nhanLenh);
  noiBroker();
}

void loop() {
  if (!mqtt.connected()) noiBroker();
  mqtt.loop();
  static uint32_t tGui = 0;
  if (millis() - tGui >= 2000) {
    tGui = millis();
    guiTelemetry();
  }
}
