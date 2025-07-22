#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <ESPmDNS.h>
#include <WiFi.h>

const char* ssid = "IFCE";
const char* password = "ifcewifi";

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
