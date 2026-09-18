#include "secrets.h"  // Copie secrets.example.h e preencha localmente.
  #include <Wire.h>              
  #include <Adafruit_GFX.h>       
  #include <Adafruit_SSD1306.h>   
  #include <ESP8266WiFi.h>        
  #include <ESP8266HTTPClient.h>  
  #include <WiFiClientSecure.h>  

  #define SCREEN_WIDTH 128        
  #define SCREEN_HEIGHT 64        

  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

  #define MQ135_PIN A0          
  #define MQ7_PIN D0           
  #define LED_PIN D4              

  const char* THINGSPEAK_SERVER = "http://api.thingspeak.com/update";

  int readings[128];
  int readingIndex = 0;

  int readMQ135() {
    return analogRead(MQ135_PIN);  
  }

  int readMQ7() {
    return digitalRead(MQ7_PIN);   
  }

  String evaluateAirQuality(int value) {
    if (value > 700) {
      return "Péssima";
    } else if (value > 500) {
      return "Ruim";
    } else if (value > 300) {
      return "Moderada";
    } else {
      return "Boa";
    }
  }

  void drawGraph() {
    display.fillRect(0, 32, 128, 32, BLACK);  
    for (int i = 1; i < 128; i++) {
      int y1 = 64 - (readings[(readingIndex + i - 1) % 128] / 16);  
      int y2 = 64 - (readings[(readingIndex + i) % 128] / 16);
      if (y1 >= 32 && y1 < 64 && y2 >= 32 && y2 < 64) {
        display.drawLine(i - 1, y1, i, y2, WHITE);
      }
    }
  }

  void sendToSupabase(int mq135Value, int mq7Value, String airQuality) {
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      WiFiClientSecure client;
      client.setInsecure();  

      String url = String(SUPABASE_URL) + "/rest/v1/Sensores";
   
      String jsonBody = "{\"MQ135\":" + String(mq135Value) + 
                        ", \"MQ7\": " + String(mq7Value) + ", " +  
                        "\"Qualidade\":\"" + airQuality + "\", " + 
                        "\"TimeStamp\": \"now()\"}";

      Serial.println("URL Supabase: " + url);
      Serial.println("JSON Body: " + jsonBody);

      http.begin(client, url);
      http.addHeader("Content-Type", "application/json");
   
      String authHeader = "Bearer " + String(SUPABASE_API_KEY);
      String apiKeyHeader = String(SUPABASE_API_KEY);  


      http.addHeader("Authorization", authHeader);  
      http.addHeader("apikey", apiKeyHeader);  

      int httpResponseCode = http.POST(jsonBody);  

      if (httpResponseCode > 0) {
        Serial.print("[HTTP] Response Code: ");
        Serial.println(httpResponseCode);
        String response = http.getString();  
        Serial.print("[HTTP] Resposta: ");
        Serial.println(response);
      } else {
        Serial.print("[HTTP] Error Code: ");
        Serial.println(httpResponseCode);
        if (httpResponseCode == 0) {
          Serial.println("[HTTP] Erro de conexão com SSL.");
        } else {
          Serial.println("[HTTP] Requisição falhou.");
        }
      }

      http.end();
    } else {
      Serial.println("[WiFi] Não está conectado");
    }
  }

  void sendToThingSpeak(int value1) {
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      WiFiClient client; 
      String url = String(THINGSPEAK_SERVER) + "?api_key=" + THINGSPEAK_API_KEY + "&field1=" + value1;

      http.begin(client, url); 
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0) {
        Serial.print("[HTTP] Response Code: ");
        Serial.println(httpResponseCode);
        String response = http.getString();  
        Serial.print("[HTTP] Resposta: ");
        Serial.println(response);
      } else {
        Serial.print("[HTTP] Error Code: ");
        Serial.println(httpResponseCode);
        Serial.println("[HTTP] Falha na requisição. Verifique a URL ou a chave de API.");
      }
      http.end();
    } else {
      Serial.println("[WiFi] Não está conectado");
    }
  }

  void setup() {

    Serial.begin(9600);

    
    Serial.println("Iniciando display OLED...");
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      Serial.println("Falha ao iniciar o display OLED");
      while (1);  
    }
    display.clearDisplay();

   
    WiFi.begin(SSID, PASSWORD);
    Serial.print("[WiFi] Conectando ao Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println(" conectado!");

 
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Conectado ao Wi-Fi!");
    display.display();
  }

  void loop() {

    int mq135Value = readMQ135();
    int mq7Value = readMQ7();


    String airQuality = evaluateAirQuality(mq135Value);

    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("MQ-135: ");
    display.println(mq135Value);
    display.print("MQ-7: ");
    display.println(mq7Value);
    display.print("Qualidade do Ar: ");
    display.println(airQuality);
    display.display();

  
    sendToSupabase(mq135Value, mq7Value, airQuality);

    
    sendToThingSpeak(mq135Value);


    readings[readingIndex] = mq135Value;
    readingIndex = (readingIndex + 1) % 128;


    drawGraph();

    delay(300000);  
  }
