#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

// WiFi
const char* ssid = "Airtel-E5573-7A7B";
const char* password = "9f12i2f2";

// WebSocket
WebSocketsClient webSocket;

// Pines
#define TRIG_FRONT 12
#define ECHO_FRONT 14
#define TRIG_BACK 5
#define ECHO_BACK 18
#define LED_PIN 10

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket Desconectado");
      break;
    case WStype_CONNECTED:
      Serial.printf("✓ WebSocket Conectado: %s\n", payload);
      break;
    default:
      break;
  }
}

float measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return -1;
  
  float distance = (duration * 0.0343) / 2;
  return distance;
}

void sendData(float front, float back) {
  StaticJsonDocument<200> doc;
  doc["timestamp"] = millis();
  doc["sensor_frontal"] = front;
  doc["sensor_trasero"] = back;
  doc["device_id"] = "esp32_ultrasonicos";
  
  String payload;
  serializeJson(doc, payload);
  
  if (webSocket.sendTXT(payload)) {
    Serial.println("✓ Datos enviados");
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
  } else {
    Serial.println("✗ Error enviando datos");
  }
}

void configurarUltrasonico() {
  Serial.begin(115200);
  
  pinMode(TRIG_FRONT, OUTPUT);
  pinMode(ECHO_FRONT, INPUT);
  pinMode(TRIG_BACK, OUTPUT);
  pinMode(ECHO_BACK, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.printf("✓ WiFi conectado: %s\n", WiFi.localIP().toString().c_str());
  
  // WebSocket
  webSocket.begin("165.22.38.176", 1880, "/ws-ultra-1");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}

void ejecutarUltrasonico() {
  webSocket.loop();
  
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 5000) {
    lastSend = millis();
    
    float front = measureDistance(TRIG_FRONT, ECHO_FRONT);
    float back = measureDistance(TRIG_BACK, ECHO_BACK);
    
    Serial.printf("Sensor Frontal: %.1f cm\n", front);
    Serial.printf("Sensor Trasero: %.1f cm\n", back);
    
    if (front > 0 && back > 0 && webSocket.isConnected()) {
      sendData(front, back);
    }
    Serial.println("----------------------------------------");
  }
}