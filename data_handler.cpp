#include "data_handler.h" // Inclui o próprio cabeçalho
#include <SPIFFS.h> // Mude de LittleFS para SPIFFS
#include <Arduino.h>
#include <HTTPClient.h>

StaticJsonDocument<CAPACITY> inventoryDoc;

bool loadInventory() {
    File inventoryFile = SPIFFS.open("/data.json", "r");
    if (!inventoryFile) {
        Serial.println("AVISO: data.json não encontrado ou falha ao abrir para leitura. Criando inventário vazio.");
        inventoryDoc.clear();
        inventoryDoc["itens"] = JsonArray();
        return true; 
    }

    String fileContent = inventoryFile.readString();
    inventoryFile.close(); 

    Serial.println("DEBUG: Conteúdo lido de data.json:");
    Serial.println(fileContent); 

    DeserializationError error = deserializeJson(inventoryDoc, fileContent);

    if (error) {
        Serial.print(F("ERRO: deserializeJson() falhou ao carregar inventário: "));
        Serial.println(error.f_str());
        Serial.println("AVISO: Inventário corrompido ou vazio. Criando inventário vazio.");
        inventoryDoc.clear();
        inventoryDoc["itens"] = JsonArray();
        return false; 
    }

    Serial.println("DEBUG: 'inventoryDoc' (na RAM) APÓS carregar de data.json:");
    serializeJsonPretty(inventoryDoc, Serial);
    Serial.println();

    Serial.println("DEBUG: Inventário carregado com sucesso de data.json.");
    return true;
}

bool saveInventory() {
    File inventoryFile = SPIFFS.open("/data.json", "w");
    if (!inventoryFile) {
        Serial.println("ERRO: Falha ao abrir data.json para escrita.");
        return false;
    }

    Serial.println("DEBUG: Conteúdo do 'inventoryDoc' (na RAM) ANTES de salvar no SPIFFS:");
    serializeJsonPretty(inventoryDoc, Serial); 
    Serial.println();

    
    size_t bytesWritten = serializeJson(inventoryDoc, inventoryFile);

    if (bytesWritten == 0) { 
        Serial.println("ERRO: Falha ao escrever JSON no data.json. Nenhum byte escrito.");
        inventoryFile.close(); 
        return false;
    } else {
        Serial.print("DEBUG: ");
        Serial.print(bytesWritten);
        Serial.println(" bytes escritos com sucesso para data.json.");
    }

    inventoryFile.close();
    Serial.println("DEBUG: Arquivo data.json fechado.");
    return true;
}
void handleNewProduct(WebServer& server) {
    if (server.method() == HTTP_POST && server.hasArg("plain")) {
        String requestBody = server.arg("plain");
        Serial.println("Received POST request body:");
        Serial.println(requestBody);

        StaticJsonDocument<512> newProductDoc; 
        DeserializationError error = deserializeJson(newProductDoc, requestBody);

        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            server.send(400, "text/plain", "Invalid JSON format");
            return;
        }

       
        if (newProductDoc.containsKey("id") && newProductDoc.containsKey("name") &&
            newProductDoc.containsKey("amount") && newProductDoc.containsKey("critic") &&
            newProductDoc.containsKey("price")) {

            JsonArray itens = inventoryDoc["itens"].as<JsonArray>();

            bool idExists = false;
            for (JsonObject item : itens) {
                if (item["id"] == newProductDoc["id"]) {
                    idExists = true;
                    break;
                }
            }

            if (idExists) {
                server.send(409, "text/plain", "ID do produto já existe."); // Conflict
                Serial.println("Erro: ID do produto já existe.");
                return;
            }

            itens.add(newProductDoc); 

            if (saveInventory()) {
                verificarItensCriticos();
                server.send(201, "text/plain", "Produto adicionado com sucesso!"); // 201 Created
                Serial.println("Novo produto adicionado e inventário salvo.");
            } else {
                server.send(500, "text/plain", "Erro interno ao salvar o inventário.");
                Serial.println("Erro: Falha ao salvar o inventário após adicionar o produto.");
            }
        } else {
            server.send(400, "text/plain", "Campos obrigatórios ausentes no JSON do produto.");
            Serial.println("Erro: Campos obrigatórios ausentes no JSON do produto.");
        }
    } else {
        server.send(405, "text/plain", "Method Not Allowed or No JSON Body"); // Or 400 for bad request
    }
}
 void handleEditProduct(WebServer& server) {
     if (server.method() == HTTP_POST && server.hasArg("plain")) { 
         String requestBody = server.arg("plain");
         Serial.println("Received POST/PUT request body for edit:");
         Serial.println(requestBody);

         StaticJsonDocument<512> updatedProductDoc;
         DeserializationError error = deserializeJson(updatedProductDoc, requestBody);

         if (error) {
             Serial.print(F("deserializeJson() failed: "));
             Serial.println(error.f_str());
             server.send(400, "text/plain", "Formato JSON inválido.");
             return;
         }

         if (!updatedProductDoc.containsKey("id")) {
             server.send(400, "text/plain", "ID do produto ausente para edição.");
             Serial.println("Erro: ID do produto ausente para edição.");
             return;
         }

         String productIdToEdit = updatedProductDoc["id"].as<String>();
         JsonArray itens = inventoryDoc["itens"].as<JsonArray>();
         bool itemFound = false;

         for (JsonObject item : itens) {
             if (item["id"] == productIdToEdit) {
                 if (updatedProductDoc.containsKey("name")) {
                     item["name"] = updatedProductDoc["name"];
                 }
                 if (updatedProductDoc.containsKey("amount")) {
                     item["amount"] = updatedProductDoc["amount"];
                 }
                 if (updatedProductDoc.containsKey("critic")) {
                     item["critic"] = updatedProductDoc["critic"];
                 }
                 if (updatedProductDoc.containsKey("price")) {
                     item["price"] = updatedProductDoc["price"];
                 }
                 if (updatedProductDoc.containsKey("cost")) {
                     item["cost"] = updatedProductDoc["cost"];
                 }
                 if (updatedProductDoc.containsKey("category")) {
                     item["category"] = updatedProductDoc["category"];
                 }
                 if (updatedProductDoc.containsKey("location")) {
                     item["location"] = updatedProductDoc["location"];
                 }

                 itemFound = true;
                 break;
             }
         }

         if (itemFound) {
             if (saveInventory()) {
                  verificarItensCriticos(); 
                 server.send(200, "text/plain", "Produto atualizado com sucesso!"); // 200 OK para sucesso de edição
                 Serial.println("Produto atualizado e inventário salvo.");
             } else {
                 server.send(500, "text/plain", "Erro interno ao salvar o inventário após edição.");
                 Serial.println("Erro: Falha ao salvar o inventário após edição.");
             }
         } else {
             server.send(404, "text/plain", "Produto não encontrado."); // 404 Not Found se o ID não existe
             Serial.println("Erro: Produto não encontrado para edição.");
         }
     } else {
         server.send(405, "text/plain", "Método Não Permitido ou Corpo JSON Ausente.");
     }
 }
void handleDeleteProduct(WebServer& server, const String& productId) {
    if (!loadInventory()) {
        server.send(500, "application/json", "{\"error\": \"Falha ao carregar inventário para exclusão.\"}");
        return;
    }

    JsonArray products = inventoryDoc["itens"].as<JsonArray>();
    if (!products) {
        server.send(500, "application/json", "{\"error\": \"Estrutura do inventário inválida.\"}");
        return;
    }

    bool foundAndDeleted = false;
    for (int i = products.size() - 1; i >= 0; i--) {
        JsonObject product = products[i];
        if (product["id"] == productId) {
            products.remove(i);
            foundAndDeleted = true;
            break; 
        }
    }

    if (foundAndDeleted) {
        if (saveInventory()) {
            server.send(200, "application/json", "{\"message\": \"Produto excluído com sucesso!\"}");
        } else {
            server.send(500, "application/json", "{\"error\": \"Falha ao salvar inventário após exclusão.\"}");
        }
    } else {
        server.send(404, "application/json", "{\"error\": \"Produto não encontrado.\"}");
    }
}

 int findAndUpdateItemQuantity(String itemId, int change) {
     if (inventoryDoc.containsKey(itemId)) {
         int currentQuantity = inventoryDoc[itemId]["quantity"];
         int newQuantity = currentQuantity + change;

         if (newQuantity < 0) {
             Serial.println("Tentativa de quantidade negativa.");
             return -1;
         }
        
         if (!saveInventory()) {
             Serial.println("Erro ao salvar inventário após atualização.");
         }

         return newQuantity;
     }
     return -1; 
 }

 void verificarItensCriticos() {
  JsonArray itens = inventoryDoc["itens"].as<JsonArray>();

  for (JsonObject item : itens) {
    int amount = item["amount"];
    int critic = item["critic"];

    if (amount <= critic) {
      String nome = item["name"].as<String>();
      String mensagem = "ALERTA: Produto '" + nome + "' está com estoque crítico (" + String(amount) + ").";
      Serial.println("DEBUG: Enviando alerta: " + mensagem);
      enviarNotificacaoWhatsapp(mensagem);
    }
  }
}


 void handleDataGet(WebServer& server) {
     String jsonOutput;
     serializeJson(inventoryDoc, jsonOutput); 
     server.send(200, "application/json", jsonOutput); 
 }
void handleGetProductById(WebServer& server, const String& productId) {
    Serial.print("Buscando produto com ID: ");
    Serial.println(productId);

    if (!loadInventory()) {
        server.send(500, "application/json", "{\"error\": \"Falha ao carregar inventário.\"}");
        return;
    }

    JsonArray itens = inventoryDoc["itens"].as<JsonArray>();
    if (!itens) {
        server.send(500, "application/json", "{\"error\": \"Estrutura do inventário inválida (itens não é um array).\"}");
        return;
    }

    for (JsonObject item : itens) {
        if (item["id"] == productId) {
            String jsonOutput;
            serializeJson(item, jsonOutput);
            Serial.print("Produto encontrado e enviado: ");
            Serial.println(jsonOutput);
            server.send(200, "application/json", jsonOutput);
            return; 
        }
    }

    Serial.println("Produto não encontrado.");
    server.send(404, "application/json", "{\"error\": \"Produto não encontrado.\"}");
}
void handleDataPost(WebServer& server) {
    String itemId = server.arg("id"); 
    int change = server.arg("change").toInt(); 

    int newQuantity = findAndUpdateItemQuantity(itemId, change); 

    if (newQuantity != -1) {
        JsonDocument responseDoc;
        responseDoc["status"] = "success";
        responseDoc["newQuantity"] = newQuantity;
        String jsonResponse;
        serializeJson(responseDoc, jsonResponse);
        server.send(200, "application/json", jsonResponse);
    } else {
        server.send(404, "application/json", "{\"status\":\"error\", \"message\":\"Item nao encontrado ou quantidade invalida\"}");
    }
}

void enviarNotificacaoWhatsapp(String mensagem) {
  String numero = "+558588471298"; 
  String apikey = "4954453";  
  mensagem.replace(" ", "+");     

  String url = "https://api.callmebot.com/whatsapp.php?phone=" + numero + "&text=" + mensagem + "&apikey=" + apikey;

  HTTPClient http;
  http.begin(url);
  int httpResponseCode = http.GET();
  http.end();

  if (httpResponseCode > 0) {
    Serial.print("Notificação enviada com sucesso. Código: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Erro ao enviar notificação. Código: ");
    Serial.println(httpResponseCode);
  }
}
