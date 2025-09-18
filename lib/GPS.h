#ifndef GPS_H
#define GPS_H

#include <Arduino.h>

// Pines para la comunicación UART (RX y TX) con el módulo GPS
#define GPS_RX_PIN 17
#define GPS_TX_PIN 16
#define GPS_BAUDRATE 9600

// Estructura para almacenar los datos del GPS
typedef struct {
    float latitud;
    float longitud;
    int fix;
    int satelites;
} GpsData_t;

// Declaración de funciones
void configurarGps();
GpsData_t leerDatosGps();

#endif