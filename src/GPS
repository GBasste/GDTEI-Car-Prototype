#include "GPS.h"
#include <HardwareSerial.h>

// Crear una instancia de HardwareSerial para el puerto UART
HardwareSerial SerialGPS(2); // Utiliza el puerto UART 2

// Función privada para convertir las coordenadas de DMS a decimal
float convertirADecimal(String coord, char direccion) {
    // Busca el punto decimal
    int dotIndex = coord.indexOf('.');
    if (dotIndex == -1) {
        return 0.0;
    }
    
    // Extrae los grados y minutos como strings
    String degreesStr = coord.substring(0, dotIndex - 2);
    String minutesStr = coord.substring(dotIndex - 2);

    // Convierte a números flotantes
    float grados = degreesStr.toFloat();
    float minutos = minutesStr.toFloat();
    
    float decimal = grados + (minutos / 60.0);
    
    // Aplica el signo según la dirección
    if (direccion == 'S' || direccion == 'W') {
        decimal = -decimal;
    }
    return decimal;
}

// Función de configuración inicial del módulo GPS
void configurarGps() {
    SerialGPS.begin(GPS_BAUDRATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    Serial.println("🛰️ Esperando datos GPS...");
}

// Función para leer y procesar una línea del GPS
GpsData_t leerDatosGps() {
    GpsData_t gpsData = {0.0, 0.0, 0, 0}; // Inicializar con valores por defecto

    // Si hay datos disponibles en el puerto serie
    if (SerialGPS.available()) {
        String linea = SerialGPS.readStringUntil('\n');
        linea.trim(); // Eliminar espacios en blanco y saltos de línea

        // Si la línea es GPGGA, procesarla
        if (linea.startsWith("$GPGGA")) {
            // Dividir la línea por comas
            int lastIndex = 0;
            int count = 0;
            int commaIndices[20]; // Un array para almacenar las posiciones de las comas

            for (int i = 0; i < linea.length(); i++) {
                if (linea.charAt(i) == ',') {
                    commaIndices[count] = i;
                    count++;
                }
            }

            // Asegurarse de que haya suficientes partes
            if (count > 6) {
                // Extraer la latitud, dirección, longitud, dirección, fix y satélites
                String latStr = linea.substring(commaIndices[1] + 1, commaIndices[2]);
                char latDir = linea.charAt(commaIndices[2] + 1);
                String lonStr = linea.substring(commaIndices[3] + 1, commaIndices[4]);
                char lonDir = linea.charAt(commaIndices[4] + 1);
                String fixStr = linea.substring(commaIndices[5] + 1, commaIndices[6]);
                String satsStr = linea.substring(commaIndices[6] + 1, commaIndices[7]);

                // Convertir los datos
                gpsData.fix = fixStr.toInt();
                gpsData.satelites = satsStr.toInt();

                if (gpsData.fix > 0) {
                    gpsData.latitud = convertirADecimal(latStr, latDir);
                    gpsData.longitud = convertirADecimal(lonStr, lonDir);
                }
            }
        }
    }
    return gpsData;
}