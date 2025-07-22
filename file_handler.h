#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <WebServer.h> 
#include <FS.h>        
#include <Arduino.h>
#include <SPIFFS.h>
using namespace fs; 

void handleFileRequest(WebServer& server, const String& path, const String& contentType);

#endif
