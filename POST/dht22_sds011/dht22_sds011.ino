#include <DHT_U.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SDS011.h>
#include <SoftwareSerial.h>

#define SDS_RX D2
#define SDS_TX D1

#define DHTPIN D4 
#define DHTTYPE DHT22   

const char* ssid     = "Minhyuk Nam의 iPhone";
const char* password = "ak4kh2sd2aun";
/*const char* ssid     = "MMlab1001";
const char* password = "mpeglab10";*/
const char* host = "ec2-13-209-35-182.ap-northeast-2.compute.amazonaws.com";

float p10,p25;
int error;

struct Air {
  float pm25;
  float pm10;
  float humidity;
  float temperature;
};

SoftwareSerial mySerial(D7, D8, false, 256);
DHT_Unified dht(DHTPIN, DHTTYPE);
ESP8266WebServer server(80);
WiFiClient client;
SDS011 sds;

float normalizePM25(float pm25, float humidity){
  return pm25/(1.0+0.48756*pow((humidity/100.0), 8.60068));
}

float normalizePM10(float pm10, float humidity){
  return pm10/(1.0+0.81559*pow((humidity/100.0), 5.83411));
}

float calculatePolutionPM25(float pm25){
  return pm25*100/25;
}

float calculatePolutionPM10(float pm10){
  return pm10*100/50;
}

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);
  sds.begin(SDS_RX,SDS_TX);
  dht.begin();
  connectToWiFi();
}

void loop() {
  Air airData = readPolution();
  if (client.connect(host,8000) & airData.pm25 > 0.0) {
    String postStr = "/api/room/K501";
    postStr +="/temp/";
    postStr += String(airData.temperature);
    postStr += "/humidity/";
    postStr += String(airData.humidity);
    postStr += "/co2/null";
    postStr += "/dust/";
    postStr += String(airData.pm10); 
    //postStr += "\r\n\r\n";

    Serial.println(String("POST ")+postStr+" HTTP/1.1\n");
    
    client.print("POST "+postStr+" HTTP/1.1\n");
    client.print("Host: ec2-13-209-35-182.ap-northeast-2.compute.amazonaws.com\n");
    client.print("Connection: close\n");
    //client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: 0");
    //client.print(postStr.length());
    client.print("\n\n");
    //client.print(postStr);
    
    /*client.print(String("GET /api")+postStr + " HTTP/1.1\r\n" +
              "Host: " + host + "\r\n" + 
              "Connection: close\r\n\r\n");*/
  }
  client.stop();

  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
  
  delay(5000); 
  
  server.handleClient();
}

void connectToWiFi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
  Serial.print("Connecting to ");
  Serial.println(ssid); 

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  startServer();
}

void startServer(){
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void handleRoot() {
    Air airData = readPolution();
    server.send(200, "text/plain", "PM2.5: " + String(airData.pm25) + " (" + String(calculatePolutionPM25(airData.pm25)) + "% normy) | PM10: " +  String(airData.pm10) + " (" + String(calculatePolutionPM10(airData.pm10)) + "% normy) | Temperature: " + airData.temperature + " | Humidity: " + airData.humidity);  
}

Air readPolution(){
  float temperature, humidity;
  error = sds.read(&p25,&p10);
  if (!error) {
    sensors_event_t event;  
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      Serial.println("Error reading temperature!");
    } else {
      temperature = event.temperature;
    }
  
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      Serial.println("Error reading humidity!");
    } else {
      humidity = event.relative_humidity;
    }
   
    Air result = (Air){normalizePM25(p25/10, humidity), normalizePM10(p10/10, humidity), humidity, temperature};
    return result;
    } else {
    Serial.println("Error reading SDS011");
    return (Air){0.0, 0.0, 0.0, 0.0};
    }
}
