#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

const char* ssid = "Airtel-E5573-7A7B";
const char* password = "9f12i2f2";

WebSocketsClient webSocket;

#define TRIG_FRONT 12
#define ECHO_FRONT 14
#define TRIG_BACK 5
#define ECHO_BACK 18

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  if(type == WStype_CONNECTED) {
    Serial.println("WebSocket Conectado");
  } else if(type == WStype_DISCONNECTED) {
    Serial.println("WebSocket Desconectado");
  }
}

float measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH, 30000);
  
  if (duration < 150) return -1;
  
  float distance = (duration * 0.0343) / 2.0;
  
  if (distance > 400) return -1;
  
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
  webSocket.sendTXT(payload);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  
  pinMode(TRIG_FRONT, OUTPUT);
  pinMode(ECHO_FRONT, INPUT);
  pinMode(TRIG_BACK, OUTPUT);
  pinMode(ECHO_BACK, INPUT);
  
  digitalWrite(TRIG_FRONT, LOW);
  digitalWrite(TRIG_BACK, LOW);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nWiFi: %s\n", WiFi.localIP().toString().c_str());
  
  webSocket.begin("165.22.38.176", 1880, "/ws-ultra-1");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
  
  Serial.println("Iniciado\n");
}

void loop() {
  webSocket.loop();
  
  float front = measureDistance(TRIG_FRONT, ECHO_FRONT);
  delay(60);
  float back = measureDistance(TRIG_BACK, ECHO_BACK);
  
  Serial.printf("F: %.1f cm | T: %.1f cm\n", front, back);
  
  if (front > 0 && back > 0 && webSocket.isConnected()) {
    sendData(front, back);
  }
  
  delay(200);
}