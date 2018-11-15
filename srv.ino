#include <ESP8266WiFi.h>
#include <SimpleDHT.h>

const int dhtPin = D5;

// sensor info
SimpleDHT11 dhtSensor(dhtPin);
const int dhtSampleFrequency= 1000;
int dhtSampleCounter = 0;
byte temperature = 0;
byte humidity = 0;

// wifi info
const char* wifi_ssid = "Hotswag Manor";
const char* wifi_pass = "xoxo<3!!";

// server info
const int serverPort = 3001;
WiFiServer server(serverPort);

String header;

const int ledPin = D9;

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
  server.begin();
  Serial.print("[INFO] Listening on port ");
  Serial.println(serverPort);
}

void serverLoop() {
  WiFiClient client = server.available();

  if (client) { // new connection
    blink(2);
    Serial.println("[INFO] New client connected");
    digitalWrite(ledPin, HIGH);
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;

        if (c == '\n') { // newline?
          if (currentLine.length() == 0) { // a blank currentLine and c == \n means the end of the request, apparently
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:application/json");
            client.println("Connection: close");
            client.println();
            client.print("{\"temperature\":");
            client.print(temperature);
            client.print(",\"humidity\":");
            client.print(humidity);
            client.println("}");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    header = "";
    client.stop();
    Serial.println("[INFO] Client disconnected");
  }
}

void sampleTemperature() {
  Serial.println("[INFO] Sampling temperature");
  dhtSensor.read(&temperature, &humidity, NULL);
  Serial.print("[INFO] Temperature: ");
  Serial.println(temperature);
  Serial.print("[INFO] Humidity: ");
  Serial.println(humidity);
}

void temperatureLoop() {
  if (dhtSampleCounter == dhtSampleFrequency) {
    blink(3);
    dhtSampleCounter = 0;
    sampleTemperature();
  }
  dhtSampleCounter++;
}

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);

  initializeWifi();
  
  server.begin();
}

void loop() {
  temperatureLoop();
  serverLoop();
}
