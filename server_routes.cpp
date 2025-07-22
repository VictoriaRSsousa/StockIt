#include "server_routes.h"
#include "file_handler.h"
#include "data_handler.h"
#include "utils.h"
#include <Arduino.h>
#include <SPIFFS.h>
#include <cstring>

void setupRoutes(WebServer& server) {

    // --- Rotas para arquivos estáticos (CSS, JS) ---
    // Estas rotas são para os arquivos que o navegador vai requisitar SEPARADAMENTE
    // depois de carregar o HTML.
    server.on("/style.css", HTTP_GET, [&server]() {
        handleFileRequest(server, "/style.css", "text/css");
    });
    server.on("/form.css", HTTP_GET, [&server]() {
        handleFileRequest(server, "/form.css", "text/css");
    });
    server.on("/formEdit.css", HTTP_GET, [&server]() { // Rota para o CSS da página de edição
        handleFileRequest(server, "/formEdit.css", "text/css");
    });
    server.on("/script.js", HTTP_GET, [&server]() {
        handleFileRequest(server, "/script.js", "application/javascript");
    });
    server.on("/form.js", HTTP_GET, [&server]() { // JavaScript usado tanto para form quanto formEdit
        handleFileRequest(server, "/form.js", "application/javascript");
    });
        server.on("/form.js", HTTP_GET, [&server]() { // JavaScript usado tanto para form quanto formEdit
        handleFileRequest(server, "/formEdit.js", "application/javascript");
    });

    // --- Rotas para as páginas HTML ---
    // Cada uma dessas serve apenas o HTML principal da página.
    server.on("/form", HTTP_GET, [&server]() {
        handleFileRequest(server, "/form.html", "text/html");
    });
    server.on("/", HTTP_GET, [&server]() {
        handleFileRequest(server, "/index.html", "text/html");
    });


    // --- Rotas da API ---
    server.on("/api/inventory", HTTP_GET, [&server]() {
        handleDataGet(server);
    });
    server.on("/api/editProduct", HTTP_POST, [&server](){
        handleEditProduct(server);
    });
    server.on("/api/addProduct", HTTP_POST, [&server](){
        handleNewProduct(server);
    });

    // --- Handler de 404 (onNotFound) ---
    server.onNotFound([&server]() {
        String uri = server.uri();
        String methodString;
        if (server.method() == HTTP_GET) methodString = "GET";
        else if (server.method() == HTTP_POST) methodString = "POST";
        else if (server.method() == HTTP_PUT) methodString = "PUT";
        else if (server.method() == HTTP_DELETE) methodString = "DELETE";
        else methodString = "UNKNOWN";

        Serial.print("onNotFound: URI = ");
        Serial.print(uri);
        Serial.print(", Method = ");
        Serial.println(methodString);

        if (server.method() == HTTP_DELETE && uri.startsWith("/api/deleteProduct/")) {
            String productId = uri.substring(uri.lastIndexOf('/') + 1);
            if (productId.length() > 0) {
                handleDeleteProduct(server, productId);
            } else {
                server.send(400, "application/json", "{\"error\": \"ID do produto ausente na URL.\"}");
            }
        }
       
        else if (server.method() == HTTP_GET && uri.startsWith("/formEdit/") && uri.length() > strlen("/formEdit/")) { // CORRIGIDO: strlen("/formEdit/")
            String idParam = uri.substring(uri.lastIndexOf('/') + 1);
            Serial.print("Acessada rota /formEdit/ com ID: "); // Corrigido para /formEdit/
            Serial.println(idParam);
            handleFileRequest(server, "/formEdit.html", "text/html");
              handleFileRequest(server, "/form.css", "text/css");
              handleFileRequest(server, "/form.js", "application/javascript");
        }
        else if (server.method() == HTTP_GET && uri.startsWith("/api/product/") && uri.length() > strlen("/api/product/")) {
            String productId = uri.substring(uri.lastIndexOf('/') + 1);
            Serial.print("API: Buscando produto por ID (GET /api/product/): ");
            Serial.println(productId);
            handleGetProductById(server, productId); // Chama a nova função
        }
        else {
            Serial.println("404: Nenhuma rota correspondente encontrada.");
            server.send(404, "text/plain", "Not Found");
        }
    });
}
