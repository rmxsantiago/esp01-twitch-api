#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Arduino_JSON.h>
#include <sstream>
#include <string>
#include <cstring>
#include "credentials.h"

const char* ssid = WIFIID;
const char* password = WIFIPASS;

void setup() {  
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);  
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);    
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);
}


void loop() {   
  Serial.print("connecting to ");
  Serial.print("id.twitch.tv");

  // Use WiFiClient class to create TCP connections
  WiFiClientSecure client;
  client.setInsecure();
  
  if (!client.connect("id.twitch.tv", 443)) {
    Serial.println("connection failed");
    Serial.println("wait 5 sec...");
    delay(5000);
    return;
  }  

  String data = "client_id=" + String(CLIENTID) + "&client_secret=" + String(CLIENTSECRET) + "&grant_type=client_credentials";
  
//   This will send the request to the server
  client.println("POST /oauth2/token HTTP/1.1");
  client.println("Host: id.twitch.tv");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.println("Content-Length: " + String(data.length()));
  client.println();
  client.println(data);


  //read back one line from server
  Serial.println("receiving from remote server");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      delay(60000);
      return;
    }
  }

  std::stringstream ss;
  bool capturing = false;
  while (client.available()) {
    char ch = static_cast<char>(client.read());
    if(ch == '{' || capturing == true || ch == '}') {
      capturing = true;
      ss << ch;
      if(ch == '}') {
        capturing = false;
      }
    }    
  }  

  JSONVar json = JSON.parse(ss.str().c_str());  

  String access_token = String((const char*) json["access_token"]);
  Serial.println(access_token);

  if (!client.connect("api.twitch.tv", 443)) {
    Serial.println("connection failed");
    Serial.println("wait 5 sec...");
    delay(5000);
    return;
  }  

  Serial.println("GET /helix/streams?user_login=julialabs HTTP/1.1");
  client.println("GET /helix/streams?user_login=julialabs HTTP/1.1");
  client.println("Host: api.twitch.tv"); 
  client.println("Authorization: Bearer " + access_token);
  client.println("Client-Id: " + String(CLIENTID));
  client.println();
  client.println();

  timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 10000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      delay(60000);
      return;
    }
  }
  ss.clear();

  delay(1000);

  capturing = false;
  String response = "";
  while (client.available()) {    
    char ch = static_cast<char>(client.read());
    if(ch == '{' || capturing == true || ch == '}') {
      capturing = true;
      response.concat(ch);
      if(ch == '}') {
        capturing = false;
      }
    }
  }

  Serial.println("Recebendo stream data");
  Serial.println(response);


  Serial.println("closing connection");
  client.stop();

  Serial.println("wait 5 sec...");
  delay(5000);
}
