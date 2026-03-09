#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <ArduinoJson.h>

#define DHTPIN 26
#define DHTTYPE DHT22

#define PIRPIN 14
#define LDRPIN 35
#define RELAY_FAN 25
#define LEDPIN 4

DHT dht(DHTPIN, DHTTYPE);

WebServer server(80);

// Fan/LED states
String fan_state = "OFF";
String led_state = "OFF";

// WiFi AP credentials
const char* ssid = "ESP32-AP";
const char* password = "12345678";

// HTML page
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head><title>ESP32 Dashboard</title></head>
<body>
<h2>ESP32 Sensor Dashboard</h2>
<p>Temperature: <span id="t"></span></p>
<p>Humidity: <span id="h"></span></p>
<p>Light: <span id="l"></span></p>
<p>Motion: <span id="m"></span></p>
<p>Fan: <span id="f"></span></p>
<p>LED: <span id="led"></span></p>

<script>
setInterval(()=>{
  fetch("/data")
    .then(r=>r.json())
    .then(d=>{
      document.getElementById("t").innerText=d.temp;
      document.getElementById("h").innerText=d.hum;
      document.getElementById("l").innerText=d.ldr;
      document.getElementById("m").innerText=d.pir;
      document.getElementById("f").innerText=d.fan;
      document.getElementById("led").innerText=d.led;
    })
},1000)
</script>

</body>
</html>
)rawliteral";

// JSON endpoint
void handleData() {
  StaticJsonDocument<200> doc;
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int pir = digitalRead(PIRPIN);
  int ldr = analogRead(LDRPIN);

  // Fan/LED logic
  fan_state = (temp > 20) ? "ON" : "OFF";
  led_state = (pir == 1) ? "ON" : "OFF";

  digitalWrite(RELAY_FAN, (fan_state == "ON") ? HIGH : LOW);
  digitalWrite(LEDPIN, (led_state == "ON") ? HIGH : LOW);

  doc["temp"] = temp;
  doc["hum"] = hum;
  doc["pir"] = pir;
  doc["ldr"] = ldr;
  doc["fan"] = fan_state;
  doc["led"] = led_state;

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void setup() {
  Serial.begin(115200);
  pinMode(PIRPIN, INPUT);
  pinMode(RELAY_FAN, OUTPUT);
  pinMode(LEDPIN, OUTPUT);
  dht.begin();

  // Start WiFi AP
  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  Serial.println(WiFi.localIP());

  // WebServer routes
  server.on("/", [](){ server.send(200, "text/html", htmlPage); });
  server.on("/data", handleData);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
