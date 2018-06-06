#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS D3
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const char* ssid     = "Minhyuk Nam의 iPhone";
const char* password = "ak4kh2sd2aun";

const char* host = "api.thingspeak.com";
const char* streamId   = "....................";
const char* privateKey = "BYDOBDVHNSYEBI73"; /*** 수정필요 ****/

void setup() {
  Serial.begin(115200);
  sensors.begin();

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("WiFi not connected");
  }
}

void loop() {
  delay(30000);

  sensors.requestTemperatures();
  float strTemp=sensors.getTempCByIndex(0);
  
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  String getStr = "GET /update?api_key=";
  getStr += privateKey;
  getStr += "&field1=";
  getStr += String(strTemp);

  client.print(getStr + " HTTP/1.1\r\n" +
              "Host: " + host + "\r\n" + 
              "Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 60000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
}
