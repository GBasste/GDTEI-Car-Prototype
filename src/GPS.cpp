#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h> // Se mantiene para compatibilidad, pero no se usa activamente para JSON
#include <HardwareSerial.h>

// --- Configuración de WiFi ---
extern const char* ssid; // Red WiFi
extern const char* password;      // Contraseña WiFi

// --- Configuración de WebSocket para Node-RED ---
extern const char* nodeRed_host;
extern const uint16_t nodeRed_port;
extern const char* nodeRed_path;   // Ruta del nodo WebSocket In en Node-RED

// Agrega esta *DECLARACIÓN* después de los #include
// Le dice al compilador: "La definición existe en otro archivo"
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);

// --- Configuración de GPS ---
static const int RXPin = 16, TXPin = 17; // GPIOs para el Serial2 (RX2, TX2)
static const uint32_t GPSBaud = 9600;

WebSocketsClient webSocketGPS; 

// Definición del puerto serie para el GPS (Serial2 del ESP32)
HardwareSerial GPS_Serial(2); 

// Buffer para construir la sentencia NMEA
String NMEA_Sentence = "";
bool sentence_ready = false;

// ------------------------------------------
// FUNCIÓN PARA EXTRAER Y MOSTRAR SATÉLITES (Corregida)
// ------------------------------------------
void extractAndPrintSatellites(String nmea) {
  
  // --- BUSCAR SENTENCIA $GPGSV (Satélites en Vista) ---
  if (nmea.startsWith("$GPGSV")) {
    // La sentencia $GPGSV tiene esta estructura:
    // $GPGSV, [1]Num Sentencias, [2]Num Sentencia, [3]TOTAL Satélites en Vista, ...
    
    // Necesitamos el campo [3]. Contamos las comas:
    int firstComma = nmea.indexOf(',');
    int secondComma = nmea.indexOf(',', firstComma + 1);
    int thirdComma = nmea.indexOf(',', secondComma + 1);
    int fourthComma = nmea.indexOf(',', thirdComma + 1); 
    
    // El número de satélites está entre la tercera y la cuarta coma.
    if (thirdComma > 0 && fourthComma > thirdComma) {
      String totalSatellitesStr = nmea.substring(thirdComma + 1, fourthComma);
      
      // Mostrar el número de satélites
      Serial.print("Satélites (GSV - En Vista): ");
      Serial.println(totalSatellitesStr);
      return;
    }
  }
}

// ------------------------------------------
// FUNCIÓN DE CONEXIÓN WIFI
// ------------------------------------------
void connectWiFi() {
  Serial.println();
  Serial.print("Conectando a WiFi ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("WiFi conectado. IP: ");
  Serial.println(WiFi.localIP());
}

// ------------------------------------------
// SETUP
// ------------------------------------------
void inicializarGPS() {
  Serial.begin(115200);
  // NOTA: Recuerda que la tasa de fábrica del GPS es 9600
  GPS_Serial.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin); // Inicializa Serial2 para el GPS

  // Conectar a WiFi
  connectWiFi();

  // Conectar a WebSocket
  webSocketGPS.begin(nodeRed_host, nodeRed_port, nodeRed_path);
  webSocketGPS.onEvent(webSocketEvent);
  webSocketGPS.setReconnectInterval(5000); 
  Serial.println("[WS] Intentando conectar a Node-RED...");
}

// ------------------------------------------
// LOOP
// ------------------------------------------
void ejecutarGPS() {
  webSocketGPS.loop(); 
  
  // 1. LECTURA Y CONSTRUCCIÓN DE LA SENTENCIA NMEA CRUDA
  while (GPS_Serial.available() > 0) {
    char inChar = GPS_Serial.read();
    
    if (inChar == '\n') { 
      // Si encontramos el final de la línea, la sentencia está lista
      sentence_ready = true;
      break; 
    }
    // Evitar añadir el retorno de carro (\r) si existe
    if (inChar != '\r') {
      NMEA_Sentence += inChar;
    }
  }

  // 2. ENVIAR LA SENTENCIA NMEA COMPLETA
  if (sentence_ready) {
      
    // --- A. MOSTRAR EN MONITOR SERIAL (La sentencia NMEA cruda) ---
    Serial.println("------------------------------------");
    Serial.print("NMEA Recibida: ");
    Serial.println(NMEA_Sentence);
    
    // 🟢 NUEVA FUNCIÓN: Llamamos a la función para extraer y mostrar los satélites
    extractAndPrintSatellites(NMEA_Sentence);
    
    Serial.println("------------------------------------");

    // --- B. ENVIAR DATOS A NODE-RED POR WEBSOCKET ---
    if (webSocketGPS.isConnected()) {
        
        // Enviamos la cadena NMEA directamente.
        webSocketGPS.sendTXT(NMEA_Sentence); 
        Serial.print("[WS] Sentencia NMEA enviada: ");
        Serial.println(NMEA_Sentence);
    }

    // Resetear para la siguiente lectura
    NMEA_Sentence = "";           
    sentence_ready = false;
  }

  // Pequeña pausa
  delay(10); 
}