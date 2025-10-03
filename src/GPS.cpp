#include <Arduino.h>

// Definiciones de pines UART para el GPS
// En ESP32, usamos Serial2 para un puerto UART adicional.
const int GPS_RX_PIN = 16;
const int GPS_TX_PIN = 17;
const long GPS_BAUDRATE = 9600;

// Declaraciones de funciones
float convertirADecimal(String coord, char direccion);
String leerLineaGPS();
bool extraerCoordenadasGPGGA(String linea, float &latitud, float &longitud, int &fixStatus, int &satelites);
void inicializarGPS();
void ejecutarGPS();

// ------------------------------------------------------------------
// Implementación de funciones
// ------------------------------------------------------------------

// Inicializa el puerto Serial2 para el módulo GPS
void inicializarGPS() {
    Serial2.begin(GPS_BAUDRATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    Serial.println("🛰️ Esperando datos GPS...");
}

// Función auxiliar para convertir DDMM.MMMM (Grados Minutos) a grados decimales
float convertirADecimal(String coord, char direccion) {
    if (coord.length() < 5) return 0.0;
    
    // Encuentra la posición del punto decimal
    int pointIndex = coord.indexOf('.');
    if (pointIndex == -1) return 0.0;
    
    // Extraer los grados (DD o DDD)
    String degreesStr = coord.substring(0, pointIndex - 2);
    float degrees = degreesStr.toFloat();
    
    // Extraer los minutos (MM.MMMM)
    String minutesStr = coord.substring(pointIndex - 2);
    float minutes = minutesStr.toFloat();
    
    float decimal = degrees + minutes / 60.0;
    
    // Aplicar dirección (N/S, E/W)
    if (direccion == 'S' || direccion == 'W') {
        decimal = -decimal;
    }
    return decimal;
}

// Lee una línea completa del buffer serial (equivalente a uart.readline())
String leerLineaGPS() {
    if (Serial2.available()) {
        String linea = Serial2.readStringUntil('\n');
        linea.trim(); // Quita espacios y retornos de carro
        // El ESP32 maneja la codificación UTF-8 por defecto, así que no es necesario decode()
        return linea;
    }
    return ""; // Retorna String vacío si no hay datos
}

// Procesa una línea GPGGA para extraer coordenadas (equivalente a extraer_coordenadas_GPGGA)
// Nota: Usa referencias (&) para actualizar los valores de latitud, longitud, etc.
bool extraerCoordenadasGPGGA(String linea, float &latitud, float &longitud, int &fixStatus, int &satelites) {
    if (!linea.startsWith("$GPGGA")) {
        return false;
    }
    
    // El método split() no es estándar en Arduino String, usamos indexOf y substring.
    int startIndex = 0;
    int commaIndex = -1;
    
    // Partes: $GPGGA, [1] Hora, [2] Lat, [3] N/S, [4] Lon, [5] E/W, [6] Fix, [7] Satelites
    String partes[15]; // Solo necesitamos hasta el índice 7
    int partCount = 0;

    for (int i = 0; i < 15; i++) {
        commaIndex = linea.indexOf(',', startIndex);
        if (commaIndex == -1) {
            partes[i] = linea.substring(startIndex);
            partCount = i + 1;
            break;
        }
        partes[i] = linea.substring(startIndex, commaIndex);
        startIndex = commaIndex + 1;
        partCount = i + 1;
    }
    
    // Verificar que tenemos suficientes partes y que los campos 2, 4, 6 y 7 no están vacíos
    if (partCount < 8 || partes[2].isEmpty() || partes[4].isEmpty() || partes[6].isEmpty() || partes[7].isEmpty()) {
        return false;
    }

    // [2] Latitud, [3] Dirección Lat
    latitud = convertirADecimal(partes[2], partes[3].charAt(0));
    
    // [4] Longitud, [5] Dirección Lon
    longitud = convertirADecimal(partes[4], partes[5].charAt(0));
    
    // [6] Estado de Fix (0=No fix, 1=GPS fix, etc.)
    fixStatus = partes[6].toInt();
    
    // [7] Número de satélites
    satelites = partes[7].toInt();
    
    return true;
}


// Bucle de ejecución principal para el GPS (para llamar desde loop())
void ejecutarGPS() {
    String linea = leerLineaGPS();
    
    if (!linea.isEmpty()) {
        float lat, lon;
        int fix, sats;
        
        if (extraerCoordenadasGPGGA(linea, lat, lon, fix, sats)) {
            
            if (fix == 0) {
                Serial.print("❌ Sin fix aún... Satélites: ");
                Serial.println(sats);
            } else {
                Serial.println("✅ Coordenadas:");
                Serial.print("  Latitud : ");
                Serial.println(lat, 6); // Imprimir con 6 decimales
                Serial.print("  Longitud: ");
                Serial.println(lon, 6);
                Serial.print("  Satélites: ");
                Serial.println(sats);
            }
        }
    }
    // El 'sleep(1)' de Python se reemplaza por el delay en el loop() de Arduino 
    // o se maneja en el archivo principal (main.cpp) para no bloquear el sistema.
}