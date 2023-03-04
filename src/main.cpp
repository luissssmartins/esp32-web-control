#include <Arduino.h>

#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

const char* ssid = "";
const char* password = "";

const int espLedPin = 2;

int keepAlive = 0;

String ledState, keepAliveValue;

AsyncWebServer server(80);

String httpPlaceHolderProcessor(const String& val) {
  Serial.println(val);

  if (val == "STATE") {

    if (digitalRead(espLedPin)) {
      ledState = "LIGADO";
    } else {
      ledState = "DESLIGADO";
    }

    Serial.println(ledState);

    return ledState;

  } else if (val == "KEEPALIVE") {
    keepAliveValue = keepAlive;

    return keepAliveValue;
  }

  return String();
}

void setup() {
  Serial.begin(115200);

  pinMode(espLedPin, OUTPUT);

  if (!SPIFFS.begin(true)) {
    Serial.println("Erro ao carregar flash system do ESP32.");
    return;
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);

    Serial.println("Conectando ao wifi...");
  }

  Serial.println(WiFi.localIP());

  defineRoutes();

  server.begin();
}

void loop() {
  keepAlive++;
}

void defineRoutes() {

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), httpPlaceHolderProcessor);
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(espLedPin, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, httpPlaceHolderProcessor);
  });

  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(espLedPin, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, httpPlaceHolderProcessor);
  });
}