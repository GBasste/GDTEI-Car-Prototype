#include <Arduino.h>

// ------------------------------------------------------------------
// Configuraciones y Constantes
// ------------------------------------------------------------------

// === CONFIGURACIÓN ADC ===
const int ADC_PIN = 34; 

// La atenuación de 11dB permite leer hasta ~3.6V (VRef = 3.3V)
// En C++ de Arduino/ESP32 esto se configura en setup.
const int ADC_MAX_VALUE = 4095; // Resolución de 12 bits

// === CONFIGURACIÓN DEL RELÉ ===
// IN5 del módulo de 8 relés conectado al GPIO26
const int RELE_PIN = 26;
const float VOLTAJE_UMBRAL = 3.5; // Umbral de activación (3.5V)

// === CONFIGURACIÓN DEL DIVISOR DE VOLTAJE ===
// R1 = 68kΩ, R2 = 20kΩ → factor = (R1 + R2)/R2 = 4.4, pero usaremos tu valor 4.7
const float FACTOR_DIVISION = 4.7; 

// Declaración de funciones
void configurarComponentes();
void medirVoltajeYControlarRele();

// ------------------------------------------------------------------
// Implementación de funciones
// ------------------------------------------------------------------

/**
 * @brief Configura el pin del relé y el ADC.
 */
void configurarComponentes() {
    // Configuración del relé (equivalente a rele = Pin(26, Pin.OUT))
    pinMode(RELE_PIN, OUTPUT);
    // Inicialmente desactivado (HIGH si el relé es LOW-active)
    // Asumimos que HIGH desactiva el relé, como en tu código Python.
    digitalWrite(RELE_PIN, HIGH); 

    // Configuración del ADC (equivalente a adc = ADC(Pin(34)) y sus métodos)
    // La API de ESP32 en Arduino usa `analogRead` para la lectura.
    // Para configurar atenuación y resolución globalmente, usamos:
    analogSetAttenuation(ADC_11db); // Para leer hasta ~3.6V (ATTN_11DB)
    analogSetWidth(12);             // Resolución 12 bits (WIDTH_12BIT)
}

/**
 * @brief Lee el ADC, calcula el voltaje real y controla el relé.
 * (Equivalente a la función Adc() y el bucle while True en Python)
 */
void medirVoltajeYControlarRele() {
    // 1. Lectura del ADC (equivalente a valor_adc = adc.read())
    int valor_adc = analogRead(ADC_PIN);
    
    // 2. Cálculo del Voltaje ADC
    // Vref del ESP32 es ~3.3V
    // voltaje_adc = valor_adc * (3.3 / 4095)
    float voltaje_adc = (float)valor_adc * (3.3 / (float)ADC_MAX_VALUE);
    
    // 3. Cálculo del Voltaje Real
    // voltaje_real = voltaje_adc * factor
    float voltaje_real = voltaje_adc * FACTOR_DIVISION;

    Serial.print("Voltaje real: ");
    // round(voltaje_real, 2) se simula con el segundo parámetro de print
    Serial.print(voltaje_real, 2); 
    Serial.println(" V");

    // 4. Control del Relé
    if (voltaje_real > VOLTAJE_UMBRAL) {
        // ACTIVAR relé (LOW) - digitalWrite(rele, 0)
        digitalWrite(RELE_PIN, LOW); 
    } else {
        // DESACTIVAR relé (HIGH) - digitalWrite(rele, 1)
        digitalWrite(RELE_PIN, HIGH); 
    }
}