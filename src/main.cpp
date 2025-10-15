#include <Arduino.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h> 
#include <HardwareSerial.h>
#include <freertos/task.h> // <- AÑADIR esta librería para FreeRTOS


// --- Configuración de WiFi ---
//const char* ssid = "Airtel-E5573-7A7B"; // Red WiFi
//const char* password = "9f12i2f2";      // Contraseña WiFi
const char* ssid = "LIB-9227236"; // Red WiFi
const char* password = "JtxfsD5tc9vw";      // Contraseña WiFi

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

// Variables de temporización no bloqueante
unsigned long previousMillis = 0;
const long interval_ultrasonic = 250; // Intervalo para leer y enviar datos (250ms = 4 veces/seg)

// *Asume que el Ultra se ejecuta cada 1000ms (1s)*
const long interval_gps_send = 500; // Enviar datos GPS solo cada 1.5 segundos
unsigned long previousMillisGPS = 0;

unsigned long previousMillis_pwm = 0;
bool decelerated = false; // Bandera para controlar el ciclo de PWM

// Declaración de funciones de Nuevo_Interruptor.cpp ---
void configurarSistema();
void ejecutarSistema();

// Declaracion de funciones de Control_Motores ---
void configurarPines();
void cerrarPuertas();
void abrirPuertas();
void apagarMotor();
void encenderMotor();
void apagarDireccion();

// --- DECLARACIONES DE FUNCIONES DE BuzzerControl.cpp ---
void configurarBuzzer();
void beepSimple(float duration);
// ... (pegar el resto de las declaraciones aquí)
void cleanupBuzzer();

// --- DECLARACIONES DE FUNCIONES DE GPSControl.cpp ---
void inicializarGPS();
void ejecutarGPS();
//void loopWebSocketGPS();

// --- DECLARACIONES DE FUNCIONES DE VoltageMonitor.cpp ---
void configurarComponentes();
void medirVoltajeYControlarRele();
// -------------------------------------------------------

// --- ULTRASONICO ---
void configurarUltrasonico();
void ejecutarUltrasonico();
//void loopWebSocketUltra();

// ---ALARMA ---
void alarm();

// --- DECLARACIONES DE FUNCIONES DE PwmControl.cpp ---
void acelerarPwm(int pin_numero, int velocidad_inicial, int velocidad_final, float tiempo_total, const char* tipo);
void desacelerarPwm(int pin_numero, int velocidad_final, float tiempo_total);

// ✅ DECLARACIÓN DE LA FUNCIÓN DE LA TAREA
void TaskWebSocketManager(void *pvParameters); 

// Función setup()
void setup() {
  Serial.begin(115200);

  // --- Configuración del módulo Buzzer ---
  configurarBuzzer();

  // Conexión WiFi (MANTENER AQUÍ PARA QUE SEA ÚNICA)
    Serial.print("Conectando a WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi conectado!");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
    // ---------------------------

    // *AJUSTE CRÍTICO: CONEXIÓN DE WEBSOCKETS AQUÍ*
    Serial.println("[WS] Inicializando clientes WebSocket...");
    
    // Conexión Ultra
    webSocketUltra.begin(nodeRed_host, nodeRed_port, ws_path_ultra);
    webSocketUltra.onEvent(webSocketEvent);
    webSocketUltra.setReconnectInterval(5000); // Reintenta cada 5s
    
    // Conexión GPS
    webSocketGPS.begin(nodeRed_host, nodeRed_port, ws_path_gps);
    webSocketGPS.onEvent(webSocketEvent);
    webSocketGPS.setReconnectInterval(5000); // Reintenta cada 5s

    // 2. CREAR TAREA PARA GESTIONAR LA CONEXIÓN WEBSOCKETS EN EL CORE 0
    // Asignamos 4KB de stack (memoria) y la prioridad 1
    xTaskCreatePinnedToCore(
        TaskWebSocketManager,   /* Función que implementa la tarea */
        "WebSocketTask",        /* Nombre de la tarea */
        4096,                   /* Tamaño de la pila (Stack) */
        NULL,                   /* Parámetro de la tarea */
        1,                      /* Prioridad de la tarea */
        NULL,                   /* Handle de la tarea (no necesario aquí) */
        0);                     /* ¡CRÍTICO! Pinned a Core 0 (el núcleo de red) */

    Serial.println("[WS] Tarea de red iniciada en Core 0.");
  
  // Inicializa la configuración de pines del Ultra
  configurarUltrasonico();

  // Inicializa el puerto Serial2 para la comunicación con el GPS
  inicializarGPS();

  // Inicializar el ADC y el pin del relé
  configurarComponentes(); 


  //Control_Motores
  // Inicializa todos los pines
  configurarPines(); 
    
  // La acción inicial del código Python
  encenderMotor(); // Asegura que el motor tenga alimentación
  abrirPuertas();
  apagarDireccion();

  // Conexión WiFi
  Serial.print("Conectando a WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
  // ---------------------------

  //Ultrasonicos
  configurarUltrasonico();
}

void loop() {
    /*webSocketUltra.loop(); 
    delay(1); // Da un respiro de 1ms al sistema operativo para manejar la pila de red
    webSocketGPS.loop(); */

    // 2. Temporizador para Ultrasonicos (Lectura y envío)
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval_ultrasonic) {
        previousMillis = currentMillis;
        ejecutarUltrasonico(); 
    }

    // 3. Control de Voltaje
    medirVoltajeYControlarRele(); 
    
    // 4. Lógica GPS (limitada por tiempo)
    if (currentMillis - previousMillisGPS >= interval_gps_send) {
        previousMillisGPS = currentMillis;
        
        // Esta función llama a la lectura Serial del GPS y al envío webSocketGPS.sendTXT()
        ejecutarGPS(); 
    }
    // 5. Lógica de Control de Movimiento (Reemplazo del delay de 10 segundos)
    if (!decelerated) {
        // Acelerar y mantener
        acelerarPwm(25, 0, 900, 4.0, "suave");
        // Establecer el tiempo en que debe comenzar la desaceleración
        previousMillis_pwm = currentMillis;
        decelerated = true; // El siguiente paso es esperar para desacelerar
    }

    // Esperar 5 segundos + 10 segundos = 15 segundos antes de repetir el ciclo
    if (decelerated && (currentMillis - previousMillis_pwm >= 15000)) { // 15 segundos
        // Desacelerar
        desacelerarPwm(25, 0, 1.5);
        decelerated = false; // Repetir el ciclo de aceleración/desaceleración
    }

    // 6. alarm()
    alarm();

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