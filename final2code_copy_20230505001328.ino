#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include <ArduinoJson.h>

const char* ssid = "V2030";
const char* password = "272930ps";
const char* apiKey = "1bb0f855a65c387ef58a0919012c3f00";

const int MOISTURE_SENSOR_PIN = A0;
const int RELAY_PIN = D1;
const int PUMP_ON_TIME = 5000;

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;  

String jsonBuffer;

void setup() {
  Serial.begin(115200); 
  pinMode(MOISTURE_SENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    if(WiFi.status()== WL_CONNECTED){
      String serverPath = "http://api.openweathermap.org/data/2.5/forecast?lat=30.7712&lon=76.5783&cnt=1&appid=" + String(apiKey);
      
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(JSON.typeof(jsonBuffer));

      DynamicJsonDocument doc(2048);
      DeserializationError error = deserializeJson(doc, jsonBuffer);

      if (error) {
        Serial.print("Error parsing JSON data: ");
        Serial.println(error.c_str());
      } else {
        int temperature = doc["list"][0]["main"]["temp"];
        Serial.print("Temperature: ");
        Serial.print(temperature + -273.15);
        Serial.println("°C");

        int humidity = doc["list"][0]["main"]["humidity"];
        Serial.print("Humidity: ");
        Serial.print(humidity);
        Serial.println("%");

        int moistureLevel = analogRead(MOISTURE_SENSOR_PIN);
        Serial.print("Moisture Level: ");
        Serial.println(moistureLevel); 

        float pop = doc["list"][0]["pop"];
        Serial.print("Probability of Precipitation: ");
        Serial.print(pop*100);
        Serial.println("%");
        
        // If probability of precipitation is greater than 50%, turn on the relay module
        // If moisture level is below a threshold and temperature and humidity are within acceptable ranges, turn on the pump for a set time
        // If probability of precipitation is greater than 50%, turn on the relay module
// If moisture level is below a threshold and temperature and humidity are within acceptable ranges, turn on the pump for a set time
if (moistureLevel > 800 && pop < 50) {
  
digitalWrite(RELAY_PIN, LOW); // turn on relay module
    Serial.println("Watering plants...");
    delay(PUMP_ON_TIME);
    digitalWrite(RELAY_PIN, HIGH); // turn off relay module
    Serial.println("Done watering plants.");
} else {
  digitalWrite(RELAY_PIN, HIGH);
  Serial.println("Relay module turned off.");
}



      }
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

