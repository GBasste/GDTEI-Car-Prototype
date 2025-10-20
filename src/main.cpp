#include <Arduino.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h> 
#include <HardwareSerial.h>
#include <freertos/task.h> // <- AÑADIR esta librería para FreeRTOS


// --- Configuración de WiFi ---
const char* ssid = "Airtel-E5573-7A7B"; // Red WiFi
const char* password = "9f12i2f2";      // Contraseña WiFi
//const char* ssid = "LIB-9227236"; // Red WiFi
//const char* password = "JtxfsD5tc9vw";      // Contraseña WiFi
int espera = 0;

// --- DEFINICIÓN GLOBAL DE LA CONFIGURACIÓN DEL SERVIDOR ---
const char* nodeRed_host = "165.22.38.176";
uint16_t nodeRed_port = 1880;
// *NUEVAS RUTAS ÚNICAS*
const char* ws_path_ultra = "/ws-ultra-1"; // Ya la usas en Ultra.cpp, la movemos aquí
const char* ws_path_gps = "/ws-gps";       // Ya la usas en GPSControl.cpp, la movemos aquí

// ✅ DECLARA las dos instancias que necesitas
WebSocketsClient webSocketUltra; 
WebSocketsClient webSocketGPS; 


// ------------------------------------------
// FUNCIÓN CALLBACK DE WEBSOCKET
// ------------------------------------------
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      // Este mensaje es genérico, pero el loop ajustado debería reducir su frecuencia
      Serial.println("[WS] 🔴 ¡Cliente desconectado! Reintentando en 5s..."); 
      break;
    case WStype_CONNECTED:
      // CORRECCIÓN: Usamos la variable global para el GPS, o un mensaje simple
      Serial.printf("[WS] 🟢 ¡Conexión Establecida! Host: %s:%d\n", nodeRed_host, nodeRed_port);
      break;
    case WStype_ERROR:
      Serial.printf("[WS] 🟠 ¡ERROR de WebSocket! Código: %s\n", (char*)payload);
      break;
    default:
      break;
  }
}

// VARIABLES DE TEMPORIZACION NO BLOQUEANTE
//---Ultrasonicos---
unsigned long previousMillis = 0;
const long interval_ultrasonic = 250; // Intervalo para leer y enviar datos (250ms = 4 veces/seg)
//---GPS---
// *Asume que el Ultra se ejecuta cada 1000ms (1s)*
const long interval_gps_send = 500; // Enviar datos GPS solo cada 1.5 segundos
unsigned long previousMillisGPS = 0;
//Temporizador de avdc
unsigned long previousMillisVoltaje = 0;
// Leer el voltaje cada 1 segundo (1000ms) es suficiente
const long interval_voltaje = 1000;

// Declaración de funciones de Nuevo_Interruptor.cpp ---
void configurarSistema();
void ejecutarSistema();

//DECLARACION DE FUNCIONES
// --- DECLARACIONES DE FUNCIONES DE GPSControl.cpp ---
void inicializarGPS();
void ejecutarGPS();
// ---avdc.cpp ---
void configurarComponentes();
void medirVoltajeYControlarRele();
// --- ULTRASONICO ---
void configurarUltrasonico();
void ejecutarUltrasonico();
//void loopWebSocketUltra();
// ---CIERRE CENTRAL---
void CCPINCONFIG();
void CCEXE();

void TaskWebSocketManager(void *pvParameters); 

// Función setup()
void setup() {
    Serial.begin(115200);

    // 1. Inicialización de sistema y periféricos
    configurarSistema(); // Contiene Serial.begin() y pinModes del relé
    configurarUltrasonico(); // Inicializa la configuración de pines del Ultra (solo una vez)
    inicializarGPS(); // Inicializa el puerto Serial2 para la comunicación con el GPS
    configurarComponentes(); // Inicializar el ADC y el pin del relé/voltaje
    CCPINCONFIG(); // Inicialización de pines del Cierre Central
    // --- Configuración de WiFi (SOLO UNA VEZ) ---
    Serial.print("Conectando a WiFi...");
    WiFi.begin(ssid, password);
    int espera = 0; // Se debe inicializar dentro de setup o fuera de loop
    while (WiFi.status() != WL_CONNECTED && espera < 20 ) { // Usamos `< 20`
        delay(500);
        Serial.print(".");
        espera++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi conectado!");
        Serial.print("Dirección IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nNo se pudo establecer conexion WiFi.");
        // Considerar lógica de fallback o reiniciar aquí si es crítico.
    }
    // -------------------------------------------

    // *AJUSTE CRÍTICO: CONEXIÓN DE WEBSOCKETS AQUÍ*
    Serial.println("[WS] Inicializando clientes WebSocket...");
    
    // Conexión Ultra
    webSocketUltra.begin(nodeRed_host, nodeRed_port, ws_path_ultra);
    webSocketUltra.onEvent(webSocketEvent);
    webSocketUltra.setReconnectInterval(5000); 
    
    // Conexión GPS
    webSocketGPS.begin(nodeRed_host, nodeRed_port, ws_path_gps);
    webSocketGPS.onEvent(webSocketEvent);
    webSocketGPS.setReconnectInterval(5000); 

    // 2. CREAR TAREA PARA GESTIONAR LA CONEXIÓN WEBSOCKETS EN EL CORE 0
    xTaskCreatePinnedToCore(
        TaskWebSocketManager, 
        "WebSocketTask", 
        4096, 
        NULL, 
        1, 
        NULL, 
        0); 

    Serial.println("[WS] Tarea de red iniciada en Core 0.");
}

void loop() {
    // ---Interruptor---
    ejecutarSistema();

    // ---CIERRE CENTRAL---
    CCEXE();

    // 2. Temporizador para Ultrasonicos (Lectura y envío)
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval_ultrasonic) {
        previousMillis = currentMillis;
        ejecutarUltrasonico(); 
    }

    // 3. ✅ Control de Voltaje (AHORA NO BLOQUEANTE)
    if (currentMillis - previousMillisVoltaje >= interval_voltaje) {
        previousMillisVoltaje = currentMillis;
        medirVoltajeYControlarRele(); // Ahora se ejecuta cada 1s sin bloquear.
    }
    
    // 4. Lógica GPS (limitada por tiempo)
    if (currentMillis - previousMillisGPS >= interval_gps_send) {
        previousMillisGPS = currentMillis;
        
        // Esta función llama a la lectura Serial del GPS y al envío webSocketGPS.sendTXT()
        ejecutarGPS(); 
    }

    // No se necesita ningún delay() aquí.
}

// ------------------------------------------
// TAREA WEBSOCKETS EN CORE 0
// ------------------------------------------
void TaskWebSocketManager(void *pvParameters) {
    (void) pvParameters; // Para evitar advertencias de compilación si no se usa

    Serial.println("Core 0: Tarea WebSocket ejecutándose.");

    for (;;) {
        // Ejecutar los bucles de conexión. ¡Están en un núcleo separado!
        webSocketUltra.loop(); 
        webSocketGPS.loop(); 
        
        // Es crucial dar un pequeño 'sleep' a la tarea
        // para que otras tareas de red puedan ejecutarse.
        vTaskDelay(pdMS_TO_TICKS(1)); // Duerme 5ms, permitiendo que el SO ejecute otras cosas
    }
}