#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <WiFi.h>

//const char* ssid = "MARCILEY";
//const char* password = "86697070";
const char* ssid = "iPhone";
const char* password = "123456789";

void connectToWiFi() {
  WiFi.disconnect(true);
  WiFi.begin(ssid, password);
  Serial.print("Conectando-se ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado com IP: " + WiFi.localIP().toString());
}

#endif
