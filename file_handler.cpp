#include "file_handler.h" 
#include <Arduino.h>
#include <SPIFFS.h>
using namespace fs; 

void handleFileRequest(WebServer& server, const String& path, const String& contentType) {
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
  } else {
    server.send(404, "text/plain", "Arquivo não encontrado");
  }
}
