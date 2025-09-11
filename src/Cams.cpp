#include "esp_camera.h"
#include <WiFi.h>
#include <WebSocketsClient.h>
#include "base64.h"  // Para codificar imagen

// WiFi
const char* ssid = "Airtel-E5573-7A7B";
const char* password = "9f12i2f2";

// WebSocket
WebSocketsClient webSocket;

// Pines cámara AI Thinker
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
camera_config_t camera_config = {
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
  .frame_size     = FRAMESIZE_QVGA,     // Mejora resolución
  .jpeg_quality   = 10,                 // Calidad media
  .fb_count       = 1
};

// Evento WebSocket
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_CONNECTED) {
    Serial.println("WebSocket conectado");
  } else if (type == WStype_DISCONNECTED) {
    Serial.println("WebSocket desconectado");
  }
}

void CamConfig() {
  Serial.begin(115200);
  delay(1000);

  // Conexión WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n WiFi conectado");
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());

  // Inicializar cámara
  if (esp_camera_init(&camera_config) != ESP_OK) {
    Serial.println("Error al inicializar cámara");
    return;
  }

  // WebSocket al servidor Node-RED
  webSocket.begin("34.172.41.136", 1880, "/ws-cam"); // Interior
  webSocket.begin("34.172.41.136", 1880, "/ws-camfront"); // Frente
  webSocket.begin("34.172.41.136", 1880, "/ws-camrear"); // Atras
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}

void Camexe() {
  webSocket.loop();

  static unsigned long lastCapture = 0;
  if (millis() - lastCapture > 300) { //Cada 300ms (3.3 FPS aprox)
    lastCapture = millis();

    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("⚠️ Error capturando imagen");
      return;
    }

    // Codificar en base64 y enviar
    String imageBase64 = base64::encode(fb->buf, fb->len);
    String fullPayload = "data:image/jpeg;base64," + imageBase64;

    webSocket.sendTXT(fullPayload);
    Serial.println("Imagen enviada");

    esp_camera_fb_return(fb);
  }
}
