#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Arduino_JSON.h>
#include <sstream>
#include <string>
#include <cstring>
#include "credentials.h"
#include <Authorization.h>
#include <TwitchAPI.h>
#include <Networking.h>

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
  // Use WiFiClient class to create TCP connections
  WiFiClientSecure client;
  client.setInsecure();
  
  int connStatus = connect(client, OAUTH_HOST);
  if(connStatus == FAILED) {
    delay(5000);
    return;
  }

  String authRequest = getAuthorizationRequest(CLIENTID, CLIENTSECRET);
  client.print(authRequest);

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

  JSONVar json = JSON.parse(processResponse(client));  

  String accessToken = String((const char*) json["access_token"]);
  Serial.println(accessToken);

  connStatus = connect(client, TWITCH_API_HOST);
  if(connStatus == FAILED) {
    delay(5000);
    return;
  }

  String streamerRequest = getStreams(String("julialabs"), CLIENTID, accessToken);  
  client.println(streamerRequest);

  timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 10000) {
      Serial.println(">>> Client Timeout !"); 
      client.stop();
      delay(60000);
      return;
    }
  }

  delay(1000);

  String response = processResponse(client);

  Serial.println("Recebendo stream data");
  Serial.println(response);


  Serial.println("closing connection");
  client.stop();

  Serial.println("wait 5 sec...");
  delay(5000);
}
