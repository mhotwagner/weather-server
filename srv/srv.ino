#include <ESP8266WiFi.h>
#include <SimpleDHT.h>
#include <ESP8266WebServer.h>

const int dhtPin = D5;

// sensor info
SimpleDHT22 dhtSensor(dhtPin);
const int dhtSampleFrequency= 5000;
unsigned long dhtLastSampleTime = 0;
byte temperature = 0;
byte humidity = 0;

// wifi info
const char* wifi_ssid = "Hotswag Manor";
const char* wifi_pass = "xoxo<3!!";

// server info
const int serverPort = 80;
ESP8266WebServer server(serverPort);

String header;

const int ledPin = 2;

void blink(int n) {
  for (int i = 0; i < n;  i++) {
    digitalWrite(ledPin, HIGH);
    delay(50);
    digitalWrite(ledPin, LOW);
    delay(50);
  }
}

void initializeWifi() {
  Serial.print("[INFO] Connecting to ");
  Serial.print(wifi_ssid);
  Serial.println("...");
  WiFi.begin(wifi_ssid, wifi_pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("[INFO] Connected");
  Serial.print("[INFO] IP: ");
  Serial.println(WiFi.localIP());
}

void initializeServer() {
  Serial.println("[INFO] Initializaing server...");
  
  server.on("/", [](){
    Serial.println("[INFO] GET /");
    String data = "{\"temperature\": " + String(temperature) + ", \"humidity\":" + String(humidity) + "}";
    server.send(200, "application/json", data);
  });

  server.on("/temperature", [](){
    Serial.println("[INFO] GET /temperature");
    server.sendHeader("Location", "/temperature/", true);
    server.send(302, "text/plain", "");
  });  

  server.on("/temperature/", [](){
    Serial.println("[INFO] GET /temperature/");
    server.send(200, "application/json", String(temperature));
  });
  
  server.on("/humidity", [](){
    Serial.println("[INFO] GET /humidity");
    server.sendHeader("Location", "/humidity/", true);
    server.send(302, "text/plain", "");
  });  

  server.on("/humidity/", [](){
    Serial.println("[INFO] GET /humidity/");
    server.send(200, "application/json", String(humidity));
  });

  server.begin();
  
  Serial.print("[INFO] Listening on port ");
  Serial.println(serverPort);
}

void sampleTemperature() {
  Serial.println("[INFO] Sampling...");
  dhtSensor.read(&temperature, &humidity, NULL);
  Serial.print("[INFO] Temperature: ");
  Serial.println(temperature);
  Serial.print("[INFO] Humidity: ");
  Serial.println(humidity);
}

void temperatureLoop() {
  unsigned long currentTime = millis();
  if (currentTime - dhtLastSampleTime >= dhtSampleFrequency) {
    dhtLastSampleTime = currentTime;
    sampleTemperature();
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);

  initializeWifi();
  initializeServer();
  
  server.begin();
}

void loop() {
  temperatureLoop();
  server.handleClient();
}
