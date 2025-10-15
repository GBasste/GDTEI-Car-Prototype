#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h> // Se mantiene para compatibilidad, pero no se usa activamente para JSON
#include <HardwareSerial.h>

// --- Configuración de WiFi ---
extern const char* ssid; // Red WiFi
extern const char* password;      // Contraseña WiFi

// Agrega esta *DECLARACIÓN* después de los #include
// Le dice al compilador: "La definición existe en otro archivo"
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);

// --- Configuración de GPS ---
static const int RXPin = 16, TXPin = 17; // GPIOs para el Serial2 (RX2, TX2)
static const uint32_t GPSBaud = 9600;

extern WebSocketsClient webSocketGPS; 

// Definición del puerto serie para el GPS (Serial2 del ESP32)
HardwareSerial GPS_Serial(2); 

// Buffer para construir la sentencia NMEA
String NMEA_Sentence = "";
bool sentence_ready = false;

// Variables para almacenar los datos extraídos
String g_lat = "0.0";
String g_lon = "0.0";
String g_alt = "0.0";
int g_sats = 0;

// Variables globales para almacenar los datos extraídos en formato decimal
double g_lat_dd = 0.0; // Nueva variable para latitud en Grados Decimales
double g_lon_dd = 0.0; // Nueva variable para longitud en Grados Decimales

// ------------------------------------------
// FUNCIÓN PARA ENVIAR DATOS GPS FORMATEADOS COMO JSON
// ------------------------------------------
void sendGPSData() {
    StaticJsonDocument<256> doc; 

    // Usamos las variables globales que fueron actualizadas por parseNMEA
    doc["lat"] = g_lat;
    doc["lon"] = g_lon;
    doc["alt"] = g_alt;
    doc["sats"] = g_sats;
    
    String output;
    serializeJson(doc, output);

    // Enviar a Node-RED
    if (webSocketGPS.isConnected()) {
        webSocketGPS.sendTXT(output); 
        Serial.print("[WS] JSON GPS enviado: ");
        Serial.println(output);
    }
}

// ------------------------------------------
// FUNCIÓN DE PARSEO MANUAL DE GPGGA
// ------------------------------------------
void parseGPGGA(String nmea) {
    if (nmea.startsWith("$GPGGA")) {
        // ... (Tu código de localización de comas se mantiene) ...

        int idx = 0;
        int count = 0;
        int commas[15]; // Array para guardar las posiciones de las comas

        // 1. Encontrar las posiciones de las comas
        for (int i = 0; i < nmea.length() && count < 15; i++) {
            if (nmea.charAt(i) == ',') {
                commas[count++] = i;
            }
        }

        // 2. Extraer el estado del FIX
        String fixStatus = nmea.substring(commas[5] + 1, commas[6]);
        if (fixStatus != "1") {
            Serial.println("GPS: Sin FIX válido. No se envían datos.");
            g_sats = 0; // Aseguramos que los satélites sean 0
            return; // No enviar si no hay FIX
        }

        // 3. Extraer Satélites
        String satsStr = nmea.substring(commas[6] + 1, commas[7]);
        g_sats = satsStr.toInt();
        
        // 4. Extraer y Convertir Latitud
        String latDDMM = nmea.substring(commas[1] + 1, commas[2]);
        String latDir = nmea.substring(commas[2] + 1, commas[3]);
        g_lat = latDDMM; 

        if (latDDMM.length() > 0) {
            double rawLat = latDDMM.toDouble();
            int degrees = (int)(rawLat / 100);
            double minutes = rawLat - (degrees * 100);
            g_lat_dd = degrees + (minutes / 60.0);
            if (latDir == "S") g_lat_dd *= -1; // Sur es negativo
        }

        // 5. Extraer y Convertir Longitud
        String lonDDDMM = nmea.substring(commas[3] + 1, commas[4]);
        String lonDir = nmea.substring(commas[4] + 1, commas[5]);
        g_lon = lonDDDMM;
        
        if (lonDDDMM.length() > 0) {
            double rawLon = lonDDDMM.toDouble();
            int degrees = (int)(rawLon / 100);
            double minutes = rawLon - (degrees * 100);
            g_lon_dd = degrees + (minutes / 60.0);
            if (lonDir == "W") g_lon_dd *= -1; // Oeste es negativo
        }
        
        // 6. Extraer Altura
        String altStr = nmea.substring(commas[8] + 1, commas[9]);
        g_alt = altStr;

        // Mostrar lo extraído
        Serial.printf("GPS [GGA]: Lat/Lon: %s/%s | Alt: %s m | Sats: %d\n", 
                      g_lat.c_str(), g_lon.c_str(), g_alt.c_str(), g_sats);
    }
}

// ------------------------------------------
// FUNCIÓN PARA MOSTRAR ESTADO GPS EN MONITOR SERIAL
// ------------------------------------------
void printGPSStatus() {
    Serial.println("====================================");
    Serial.println("ESTADO GPS ACTUALIZADO:");
    Serial.printf("  Latitud (DDMM.MMMM): %s\n", g_lat.c_str());
    Serial.printf("  Longitud (DDDMM.MMMM): %s\n", g_lon.c_str());
    Serial.printf("  Altitud (m): %s\n", g_alt.c_str());
    Serial.printf("  Satélites en uso (GGA): %d\n", g_sats);
    Serial.println("====================================");
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
  // NOTA: Recuerda que la tasa de fábrica del GPS es 9600
  GPS_Serial.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin); // Inicializa Serial2 para el GPS
/*
  // Conectar a WiFi
  //connectWiFi();

  // Conectar a WebSocket
  webSocketGPS.begin("165.22.38.176", 1880, "/ws-gps");
  webSocketGPS.onEvent(webSocketEvent);
  webSocketGPS.setReconnectInterval(5000); 
  Serial.println("[WS] Intentando conectar a Node-RED...");*/
  Serial.println("[GPS] Puerto Serial2 inicializado para GPS.");
}

/*void loopWebSocketGPS() {
  webSocketGPS.loop();
}*/

// ------------------------------------------
// LOOP
// ------------------------------------------
void ejecutarGPS() {
    bool is_gpgga = false; // Bandera para saber si procesamos un GPGGA

    // 1. LECTURA Y CONSTRUCCIÓN DE LA SENTENCIA NMEA CRUDA
    // ... (Tu código de lectura se mantiene) ...

    while (GPS_Serial.available() > 0) {
        char inChar = GPS_Serial.read();
        
        if (inChar == '\n') { 
            sentence_ready = true;
            break; 
        }
        if (inChar != '\r') {
            NMEA_Sentence += inChar;
        }
    }

    // 2. PROCESAR Y ENVIAR LA SENTENCIA NMEA COMPLETA
    if (sentence_ready) {
        
        // --- 2.1. Procesar la Sentencia GPGGA ---
        if (NMEA_Sentence.startsWith("$GPGGA")) {
            parseGPGGA(NMEA_Sentence);
            is_gpgga = true; // Marcamos que procesamos un GPGGA
        }

        // --- 2.2. Enviar la Sentencia NMEA Completa por WebSocket ---
        if (webSocketGPS.isConnected()) {
            webSocketGPS.sendTXT(NMEA_Sentence); 
            Serial.print("[WS] Sentencia GPS enviada: ");
            Serial.println(NMEA_Sentence);
        }
        
        // 🚨 NUEVO: Mostrar el formato de coordenadas de Maps
        if (is_gpgga && g_sats > 0) { 
            // Formato de Maps: Latitud, Longitud (e.g., 40.7127, -74.0059)
            Serial.printf("➡️ **Coordenadas Maps (DD): %.6f, %.6f**\n", 
                          g_lat_dd, g_lon_dd); 
            
            // Mantener la impresión de estado completo para referencia
            printGPSStatus();
        }
        
        // Restablecer
        NMEA_Sentence = ""; 
        sentence_ready = false;
    }
}