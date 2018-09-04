#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include "index.h"

const char* ssid = "namyangjuhackathon2";
const char* password = "12345678";

ESP8266WebServer server(80);

const int led = 13;
int buzzerPin = 0; 
int temp = 0;

void handleRoot() {
  String s = MAIN_page;
  digitalWrite(led, 1);
  server.send(200, "text/html", s);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(led, 0);
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void handleADC() {
 int ldrval = analogRead(A0);
 String ldrvalValue = String(ldrval);
 
 server.send(200, "text/plane", ldrvalValue);
}

void setup(void){

  pinMode (buzzerPin, OUTPUT); 
  
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());  
  /*analogWrite(buzzerPin,500);*/
  
  HTTPClient http;
  HTTPClient http2;
  String url = "http://10.0.100.64:20000/busdata/23940";
  http.begin(url);
  
  String data = String ("{\"bus_id\":\"23940\",\"bus_name\":\"남양주종합촬영소\",\"concentration\":")+ String(analogRead(A0)) +String(",\"location\":{\"Lat\" :127.3370167 , \"Lng\": 37.59255}}");
  Serial.println("before get");
  int httpCode = http.GET();
  Serial.println("after get");
  delay(20000);
  Serial.println(httpCode); 
  http.end();
  if (httpCode == 200) { // 정상적으로 요청이 된 경우
      String payload = http.getString();
      Serial.println("Already exists");
      Serial.println(payload); 
  }
  if(httpCode == 404){
      Serial.println("post new one");
      String url2 = "http://10.0.100.64:20000/busdata";
      http2.begin(url2);
      http2.addHeader("Content-Type", "application/json");
      int httpCode2 = http2.POST(data);
      http2.end();
  }
  delay(40000);
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });
  server.on("/readADC", handleADC);
  //server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  Serial.println(analogRead(A0));
  /*
  if(analogRead(A0)>= 900){
     analogWrite(buzzerPin,500);
     delay(1000);
     analogWrite(buzzerPin, 0);   
    for(int i=0; i<5;i++){
      analogWrite(buzzerPin,500);
      delay(500);
      analogWrite(buzzerPin,0);
      delay(500);
    
    }
  }
  */
  
  int temp = analogRead(A0);
  Serial.println("=====");
  Serial.println(analogRead(A0));
  HTTPClient http;
  String url = "http://10.0.100.64:20000/busdata";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  String data = String ("{\"bus_id\":\"23940\",\"concentration\": ") + String(analogRead(A0)) + String("}");
  int httpCode = http.PATCH(data);
  http.end();
  delay(10000);
  server.handleClient();
}
