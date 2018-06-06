#define choose 1

#if (choose == 1)
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

const char* ssid     = "Minhyuk Nam의 iPhone";
const char* password = "ak4kh2sd2aun";

const char* host = "api.thingspeak.com";
const char* streamId   = "....................";
const char* privateKey = "3HZ2DSXIIY26FCKB"; /*** 수정필요 ****/

class Co2 {
  public:
    Co2(void);
    void addchar(char);
    int getvalue(void);
  private:
    int _value;
    char _buf[10];
    int _bp;
    int stat;
};

Co2::Co2() {
  _value = 0;
  stat = 1;
  _bp = 0;
}

void Co2::addchar(char c) {
    switch(stat) {
      case 1:
        if (c == 0x0D) stat = 2;
        break;
      case 2:
        if (c == 0x0A) { stat = 3; _bp=0; }
        else Serial.println("error c="+ String(c));
        break;
      case 3:
        if (c >= '0' && c <= '9')  _buf[_bp++] = c;
        else if (c == 0x0D) { 
          stat = 2; 
          _buf[_bp++] = 0;
          _value = atoi(_buf); 
          _bp = 0;
        }
        else Serial.println("error c="+ String(c));
    }  
}

int Co2::getvalue() {
  int t = _value;
  _value = 0;
  return(t);
}

SoftwareSerial co2port(D3, 13);
Co2 co2;
void setup() {
  Serial.begin(115200);
  co2port.begin(9600);

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("WiFi not connected");
}
}

void loop() { 
  int x;
  if (co2port.available() > 0) {
    co2.addchar((char)co2port.read());
    if (x = co2.getvalue()) Serial.println("Got Co2="+ String(x));
  }

  if (x!=0){
    
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  String getStr = "GET /update?api_key=";
  getStr += privateKey;
  getStr += "&field1=";
  getStr += String(x);

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
}
#endif

#if (choose==2)
#include <SoftwareSerial.h>
class Co2 {
  public:
    Co2(void);
    void addchar(char);
    int getvalue(void);
  private:
    int _value;
    char _buf[10];
    int _bp;
    int stat;
};

Co2::Co2() {
  _value = 0;
  stat = 1;
  _bp = 0;
}

void Co2::addchar(char c) {
    switch(stat) {
      case 1:
        if (c == 0x0D) stat = 2;
        break;
      case 2:
        if (c == 0x0A) { stat = 3; _bp=0; }
        else Serial.println("error c="+ String(c));
        break;
      case 3:
        if (c >= '0' && c <= '9')  _buf[_bp++] = c;
        else if (c == 0x0D) { 
          stat = 2; 
          _buf[_bp++] = 0;
          _value = atoi(_buf); 
          _bp = 0;
        }
        else Serial.println("error c="+ String(c));
    }  
}

int Co2::getvalue() {
  int t = _value;
  _value = 0;
  return(t);
}

SoftwareSerial co2port(D3, 13);
Co2 co2;
void setup() {
  Serial.begin(115200);
  co2port.begin(9600);
}
void loop() { 
  int x;
  if (co2port.available() > 0) {
    co2.addchar((char)co2port.read());
    if (x = co2.getvalue()) Serial.println("Got Co2="+ String(x));
  }
}
#endif
