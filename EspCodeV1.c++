#include <WiFi.h>
#include <DHT.h>
#include <ArduinoJson.h>

#define DHTPIN 26
#define DHTTYPE DHT22
#define PIRPIN 14
#define LDRPIN 35
#define ACSPIN 32
#define RELAY_FAN 25
#define LEDPIN 4

// ----- WiFi Credentials (REPLACE WITH YOUR OWN) -----
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

const char* serverIP = "192.168.0.103";
const int serverPort = 5000;

WiFiClient client;
DHT dht(DHTPIN, DHTTYPE);

float acsZero = 0;
float filteredCurrent = 0;

void setup(){
  Serial.begin(115200);

  pinMode(PIRPIN, INPUT);
  pinMode(RELAY_FAN, OUTPUT);
  pinMode(LEDPIN, OUTPUT);

  dht.begin();

  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi Connected");

  while(!client.connect(serverIP,serverPort)){
    delay(1000);
    Serial.println("Connecting...");
  }

  Serial.println("Server Connected");

  // ACS Calibration
  float sum=0;
  for(int i=0;i<300;i++){
    sum += analogRead(ACSPIN);
    delay(5);
  }

  acsZero = sum/300.0;
}

void loop(){

  if(!client.connected()) return;

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int pir = digitalRead(PIRPIN);
  int ldr = analogRead(LDRPIN);

  // ACS read
  float sample=0;
  for(int i=0;i<8;i++){
    sample += analogRead(ACSPIN);
    delayMicroseconds(300);
  }

  float rawAcs = sample/8.0;

  acsZero = (acsZero*0.999)+(rawAcs*0.001);

  float voltage = rawAcs*(3.3/4095.0);
  float zeroVoltage = acsZero*(3.3/4095.0);

  float current = (voltage-zeroVoltage)/0.185;

  filteredCurrent = (filteredCurrent*0.9)+(current*0.1);

  float power = filteredCurrent * 12.0;

  // Send JSON
  StaticJsonDocument<200> doc;

  doc["temp"]=temp;
  doc["hum"]=hum;
  doc["pir"]=pir;
  doc["ldr"]=ldr;
  doc["power"]=power;

  String json;
  serializeJson(doc,json);

  client.println(json);

  // Receive commands
  while(client.available()){

    String cmd = client.readStringUntil('\n');
    cmd.trim();

    if(cmd=="FAN_ON") digitalWrite(RELAY_FAN,HIGH);
    if(cmd=="FAN_OFF") digitalWrite(RELAY_FAN,LOW);
    if(cmd=="LED_ON") digitalWrite(LEDPIN,HIGH);
    if(cmd=="LED_OFF") digitalWrite(LEDPIN,LOW);
  }

  delay(1000);
}
