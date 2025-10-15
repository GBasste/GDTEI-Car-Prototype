#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

// --- Configuración de WiFi ---
extern const char* ssid; // Red WiFi
extern const char* password;      // Contraseña WiFi

// Agrega esta *DECLARACIÓN* después de los #include
// Le dice al compilador: "La definición existe en otro archivo"
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);

extern WebSocketsClient webSocketUltra;

#define TRIG_FRONT 12
#define ECHO_FRONT 14
#define TRIG_BACK 5
#define ECHO_BACK 18


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
  webSocketUltra.sendTXT(payload);
}

void configurarUltrasonico() {
  //Serial.begin(115200);
  //delay(500);
  
  pinMode(TRIG_FRONT, OUTPUT);
  pinMode(ECHO_FRONT, INPUT);
  pinMode(TRIG_BACK, OUTPUT);
  pinMode(ECHO_BACK, INPUT);
  
  digitalWrite(TRIG_FRONT, LOW);
  digitalWrite(TRIG_BACK, LOW);
  
  /*WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nWiFi: %s\n", WiFi.localIP().toString().c_str());
  
  webSocketUltra.begin("165.22.38.176", 1880, "/ws-ultra-1");
  webSocketUltra.onEvent(webSocketEvent);
  webSocketUltra.setReconnectInterval(5000);
  
  Serial.println("Iniciado\n");*/
  Serial.println("[Ultra] Pines configurados.");
}

/*void loopWebSocketUltra() {
  webSocketUltra.loop();
}*/

void ejecutarUltrasonico() {
  
  float front = measureDistance(TRIG_FRONT, ECHO_FRONT);
  float back = measureDistance(TRIG_BACK, ECHO_BACK);
  
  Serial.printf("F: %.1f cm | T: %.1f cm\n", front, back);
  
  // CAMBIO CRÍTICO: Envía SIEMPRE que haya conexión, sin importar si los 
  // valores son negativos o cero. Node-RED puede manejar el -1.0.
  if (webSocketUltra.isConnected()) {
      sendData(front, back); 
  }

  /*if (front > 0 && back > 0 && webSocketUltra.isConnected()) {
    sendData(front, back);
  }*/
}