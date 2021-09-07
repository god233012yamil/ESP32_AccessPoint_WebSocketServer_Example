// Main include file for the Arduino SDK.
#include <Arduino.h>

// Include file to support esp32 Wifi.
#include <WiFi.h>

// Include file to support Websockets.
// Arduino Websockets is a library for writing modern WebSockets applications with Arduino.
// by Gil Mainmon
// url: https://github.com/Links2004/arduinoWebSockets
#include <WebSocketsServer.h>

// Global variable declaration and initialization.
WebSocketsServer webSocket = WebSocketsServer(81);
const char *ssid = "SSID";
const char *password = "Password";
IPAddress ip = {0, 0, 0, 0};

// This function will be called when a WiFi event takes place.
void WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info){  
    switch(event) {
        case SYSTEM_EVENT_AP_START:
            // Get the Access Point IP address.
            ip = WiFi.softAPIP();
            Serial.printf("\nAP started, IP address: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
            break;
        
        case SYSTEM_EVENT_AP_STOP:
            Serial.println("AP stopped.");
            break;  
        
        case SYSTEM_EVENT_AP_STACONNECTED:
            Serial.println("A station has connected.");          
            break; 
        
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            Serial.println("A station has disconnected.");
            break;   

        case SYSTEM_EVENT_STA_GOT_IP: 
            Serial.println("A station got its IP.");
            break;             
        
        case SYSTEM_EVENT_STA_START:
        case SYSTEM_EVENT_STA_CONNECTED:
        case SYSTEM_EVENT_AP_STA_GOT_IP6:           
        case SYSTEM_EVENT_STA_DISCONNECTED: 
        default:
            break;
    }
}

// This function will be called when a WebSocket event takes place.
void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch (type) {
      //
      case WStype_TEXT:
        // Echoes what we received via default UART.
        Serial.printf("Websocket received: %s", payload);
        break;
      //  
      case WStype_CONNECTED:
        ip = webSocket.remoteIP(num);        
        Serial.printf("Socket Client %u with ip %d.%d.%d.%d has connected\r\n", num, ip[0], ip[1], ip[2], ip[3]);
        break; 
      //  
      case WStype_DISCONNECTED:
        ip = webSocket.remoteIP(num);
        Serial.printf("Socket Client %u with ip %d.%d.%d.%d has disconnected\r\n", num, ip[0], ip[1], ip[2], ip[3]);
        break;
      //  
      case WStype_ERROR:
        Serial.println("WebSocket Error!");
        break;  
      //
      default:
        Serial.println("WebSocket Unknown Event!");
        break;     

    }
    
    //
    /*if (type == WStype_TEXT) {
      // Echoes what we received via default UART.
      Serial.printf("Websocket received: %s", payload); 
      
   //   
   } else if (type == WStype_CONNECTED) {
      ip = webSocket.remoteIP(num);        
      Serial.printf("Socket Client %u with ip %d.%d.%d.%d has connected\r\n", num, ip[0], ip[1], ip[2], ip[3]);
   
   //   
   } else if (type == WStype_DISCONNECTED) {
      ip = webSocket.remoteIP(num);
      Serial.printf("Socket Client %u with ip %d.%d.%d.%d has disconnected\r\n", num, ip[0], ip[1], ip[2], ip[3]);
   
   //   
   } else if (type == WStype_ERROR) {
      Serial.println("WebSocket Error!");  
   
   } else {
      //nothing to do
   }*/

}

//
void setup() {  
  // Init LED and turn off
  //pinMode(led_pin, OUTPUT);
  //digitalWrite(led_pin, LOW);
  
  // Start Serial (UART0).
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring The Access Point..."); 

  // Setup a callback for WiFi.
  WiFi.onEvent(WiFiEvent);
  
  // Setup WiFi access point mode.
  WiFi.mode(WIFI_MODE_AP);
  
  // WiFi.softAP(const char* ssid, const char* password, int channel, int ssid_hidden, int max_connection)
  // ssid: name for the access point – maximum of 63 characters;
  // password: minimum of 8 characters; set to NULL if you want the access point to be open;
  // channel: Wi-Fi channel number (1-13)
  // ssid_hidden: (0 = broadcast SSID, 1 = hide SSID)
  // max_connection: maximum simultaneous connected clients (1-4)  
  WiFi.softAP(ssid, password);
  
  // Start the WebSocket server. 
  webSocket.begin();

  // Setup a callback for the WebSocket server.
  webSocket.onEvent(onWebSocketEvent);
}

// Main loop.
void loop() {
  
  // Loop for and handle WebSocket data.
  webSocket.loop();

  // If the UART0 has received data. 
  if (Serial.available() > 0) {
      
      // Read the UART data.
      char c[] = {(char)Serial.read()};

      // Send data to all clients.
      webSocket.broadcastTXT(c, sizeof(c));
  }  
  
}