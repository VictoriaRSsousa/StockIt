// data_handler.h
#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

#include <ArduinoJson.h> // Certifique-se de que a biblioteca ArduinoJson está instalada
#include <WebServer.h>   // Incluir WebServer para as declarações de funções


const size_t CAPACITY = 4736; // Exemplo de capacidade, ajuste conforme seu JSON


extern StaticJsonDocument<CAPACITY> inventoryDoc; 

// Declarar as funções de manipulação de dados
bool loadInventory();
bool saveInventory();
void handleNewProduct(WebServer& server);
void handleDataGet(WebServer& server);
void handleDataPost(WebServer& server);
void handleDeleteProduct(WebServer& server, const String& productId);
int findAndUpdateItemQuantity(String itemId, int change);
void handleEditProduct(WebServer& server); 
void handleGetProductById(WebServer& server, const String& productId);
void verificarItensCriticos();
void enviarNotificacaoWhatsapp(String mensagem);
#endif // DATA_HANDLER_H
