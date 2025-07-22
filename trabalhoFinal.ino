#include <WebServer.h>
#include "wifi_config.h"
#include "server_routes.h"
#include <ArduinoJson.h>
#include "data_handler.h" 
#include <Arduino.h>
#include <SPIFFS.h>


WebServer server(80);
const size_t JSON_BUFFER_SIZE = 4096;

void setup() {
  Serial.begin(115200);
  connectToWiFi();

  if (!SPIFFS.begin(false)) {
    Serial.println("Erro ao iniciar SPIFFS");
    return;
  }
Serial.print("SPIFFS Tamanho Total: "); Serial.println(SPIFFS.totalBytes());
Serial.print("SPIFFS Espaço Usado: "); Serial.println(SPIFFS.usedBytes());
Serial.print("SPIFFS Espaço Livre: "); Serial.println(SPIFFS.totalBytes() - SPIFFS.usedBytes());
  Serial.println("Sistema de arquivos montado com sucesso."); 
  if (!loadInventory()) {
    Serial.println("Não foi possível carregar o inventário inicial. Usando inventário vazio.");
  }

  setupRoutes(server);
  server.begin();
}

void loop() {
  server.handleClient();
}
