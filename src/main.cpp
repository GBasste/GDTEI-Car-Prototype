#include <Arduino.h>

// Archivos de encabezado de tus módulos
#include "AVDC.h"
#include "GPS.h" 
#include "PWM.h"
#include "ultrasonico.h"

// Declaración de funciones existentes
void configurarSistema();
void ejecutarSistema();
void CamConfig();
void Camexe();
void motorset();
void cerrar_puertas();
void abrir_puertas();
void apagar_motor();
void encender_motor();
void apagar();

// --- DECLARACIONES DE FUNCIONES DE BuzzerControl.cpp ---
void configurarBuzzer();
void beepSimple(float duration);
// ... (pegar el resto de las declaraciones aquí)
void cleanupBuzzer();

// Definiciones de pines y canales PWM para el motor
const int MOTOR_PIN = 14; 
const int MOTOR_CHANNEL = 0;

// Función setup()
void setup() {
    Serial.begin(115200);
    
    // --- Configuración de tus funciones originales ---
    configurarSistema();
    CamConfig();
    motorset();

    // --- Configuración del módulo ADC/Relé ---
    configurarAdcRele();
    
    // --- Configuración del módulo PWM del motor ---
    configurarPwm(MOTOR_PIN, MOTOR_CHANNEL);

    // --- Configuración del módulo Buzzer ---
    configurarBuzzer();

    // --- Configuración del módulo Control Motor ---
    configurarControlMotor();
    
    // --- Configuración del módulo Motor L298N ---
    configurarMotorL298N();
    configurarUltrasonico();

    configurarGps(); // <--- Llama a la nueva función de configuración
}

// Función loop()
void loop() {
    // --- Bucle del módulo ADC/Relé ---
    ejecutarAdcRele();
    
    // --- Bucle del módulo Control Motor ---
    ejecutarControlMotor();
    ejecutarUltrasonico();

    //--- alarma ---
    alarm();
    
    // --- Ejemplo de uso del módulo L298N ---
    // Puedes llamar a la función de prueba para verificar el funcionamiento
    // Comenta o elimina esta sección si no la necesitas.
    testSimpleMotor();
    
    // --- Ejemplo de uso del módulo GPS ---
    GpsData_t datosGps = leerDatosGps();

    if (datosGps.fix > 0) {
        Serial.println("✅ Coordenadas:");
        Serial.print("   Latitud: ");
        Serial.println(datosGps.latitud, 6); // Muestra 6 decimales
        Serial.print("   Longitud: ");
        Serial.println(datosGps.longitud, 6); // Muestra 6 decimales
        Serial.print("   Satélites: ");
        Serial.println(datosGps.satelites);
    } else {
        Serial.print("❌ Sin fix aún... Satélites: ");
        Serial.println(datosGps.satelites);
    }
    
    delay(1000); // Esperar 1 segundo antes de la siguiente lectura
    
    // Aquí puedes llamar a otras funciones de tu proyecto
    // como Camexe(), ejecutarSistema(), etc.
    // Por ejemplo:
    // Camexe();
    // ejecutarSistema();

    
    
}