#include "esp_camera.h"
#include <WiFi.h>
#include <WebSocketsClient.h>
#include "base64.h"

// WiFi
const char* ssid = "Airtel-E5573-7A7B";
const char* password = "9f12i2f2";

// WebSocket
WebSocketsClient webSocket;

// CONFIGURACIÓN PARA CADA CÁMARA - CAMBIAR SEGÚN LA CÁMARA:
// Para Cámara Frontal:
const char* websocket_path = "/ws-cam-front";
const char* camera_name = "Frontal";

// Para Cámara Interior - descomenta estas líneas y comenta las de arriba:
// const char* websocket_path = "/ws-cam-inside";
// const char* camera_name = "Interior";

// Para Cámara Trasera - descomenta estas líneas y comenta las de arriba:
// const char* websocket_path = "/ws-cam-rear";
// const char* camera_name = "Trasera";

// Pines cámara AI Thinker (estándar)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// Configuración cámara
camera_config_t config = {
  .pin_pwdn       = PWDN_GPIO_NUM,
  .pin_reset      = RESET_GPIO_NUM,
  .pin_xclk       = XCLK_GPIO_NUM,
  .pin_sscb_sda   = SIOD_GPIO_NUM,
  .pin_sscb_scl   = SIOC_GPIO_NUM,
  .pin_d7         = Y9_GPIO_NUM,
  .pin_d6         = Y8_GPIO_NUM,
  .pin_d5         = Y7_GPIO_NUM,
  .pin_d4         = Y6_GPIO_NUM,
  .pin_d3         = Y5_GPIO_NUM,
  .pin_d2         = Y4_GPIO_NUM,
  .pin_d1         = Y3_GPIO_NUM,
  .pin_d0         = Y2_GPIO_NUM,
  .pin_vsync      = VSYNC_GPIO_NUM,
  .pin_href       = HREF_GPIO_NUM,
  .pin_pclk       = PCLK_GPIO_NUM,
  .xclk_freq_hz   = 20000000,
  .ledc_timer     = LEDC_TIMER_0,
  .ledc_channel   = LEDC_CHANNEL_0,
  .pixel_format   = PIXFORMAT_JPEG,
  .frame_size     = FRAMESIZE_QVGA,    // 320x240
  .jpeg_quality   = 12,                // Calidad (1-63, menor = mejor)
  .fb_count       = 2
};

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%s] WebSocket Desconectado\n", camera_name);
      break;
      
    case WStype_CONNECTED:
      Serial.printf("[%s] ✅ WebSocket Conectado a: %s\n", camera_name, payload);
      break;
      
    case WStype_TEXT:
      Serial.printf("[%s] Mensaje recibido: %s\n", camera_name, payload);
      break;
      
    default:
      break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.printf("Inicializando ESP32-CAM %s\n", camera_name);

  // Conectar WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.printf("✅ WiFi conectado - IP: %s\n", WiFi.localIP().toString().c_str());

  // Inicializar cámara
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("❌ Error inicializando cámara: 0x%x\n", err);
    return;
  }
  Serial.printf("✅ Cámara %s inicializada\n", camera_name);

  // Configurar WebSocket - CAMBIAR IP POR LA DE TU SERVIDOR NODE-RED
  webSocket.begin("165.22.38.176", 1880, websocket_path);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
  Serial.printf("✅ WebSocket configurado para %s\n", websocket_path);
}

void loop() {
  webSocket.loop();

  // Enviar imagen cada 333ms (~3 FPS)
  static unsigned long lastCapture = 0;
  if (millis() - lastCapture > 333) {
    lastCapture = millis();
    
    if (webSocket.isConnected()) {
      sendImage();
    }
  }
}

void sendImage() {
  // Capturar imagen
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.printf("[%s] ❌ Error capturando imagen\n", camera_name);
    return;
  }

  // Convertir a base64 - ENVIAR SOLO EL BASE64 PURO
  String imageBase64 = base64::encode(fb->buf, fb->len);
  
  // Enviar por WebSocket (tu función Node-RED agrega el data:image/jpeg;base64,)
  bool sent = webSocket.sendTXT(imageBase64);
  
  if (sent) {
    Serial.printf("[%s] ✅ Imagen enviada (%d bytes)\n", camera_name, fb->len);
  } else {
    Serial.printf("[%s] ❌ Error enviando imagen\n", camera_name);
  }

  // Liberar memoria
  esp_camera_fb_return(fb);
}