#include <Arduino.h>

#include "ultrasonico.h"
// Archivos de encabezado de tus módulos
#include "adc_rele.h"
#include "motor_pwm.h"
#include "buzzer.h"
#include "control_motor.h"
#include "motor_l298n.h" // <--- Incluye el nuevo archivo

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
}

// Función loop()
void loop() {
    // --- Bucle del módulo ADC/Relé ---
    ejecutarAdcRele();
    
    // --- Bucle del módulo Control Motor ---
    ejecutarControlMotor();
    ejecutarUltrasonico();
    
    // --- Ejemplo de uso del módulo L298N ---
    // Puedes llamar a la función de prueba para verificar el funcionamiento
    // Comenta o elimina esta sección si no la necesitas.
    testSimpleMotor();
    
    // Aquí puedes llamar a otras funciones de tu proyecto
    // como Camexe(), ejecutarSistema(), etc.
    // Por ejemplo:
    // Camexe();
    // ejecutarSistema();
    
}