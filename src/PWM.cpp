#include <Arduino.h>
#include <cmath> // Necesario para sqrt y sin

// ------------------------------------------------------------------
// Configuraciones Globales (para simplificar el paso de objetos)
// ------------------------------------------------------------------

// Canal de LEDC a usar para el PWM. Se usa una variable global para el módulo.
// Si controlas varios motores, necesitarías usar varios canales (0, 1, 2, etc.).
const int PWM_CHANNEL = 0; 
const int PWM_FREQUENCY = 1000; // Frecuencia 1kHz
const int PWM_RESOLUTION = 10;  // Resolución 10 bits (0-1023)

// Variable global para almacenar el último valor de duty cycle aplicado 
// por acelerarPwm, necesario para desacelerarPwm.
int last_duty_cycle = 0; 

// ------------------------------------------------------------------
// Declaraciones de funciones
// ------------------------------------------------------------------

void acelerarPwm(int pin_numero, int velocidad_inicial, int velocidad_final, float tiempo_total, const char* tipo);
void desacelerarPwm(int pin_numero, int velocidad_final, float tiempo_total);

// ------------------------------------------------------------------
// Implementación de funciones
// ------------------------------------------------------------------

/**
 * @brief Acelera PWM gradualmente desde velocidad inicial hasta final.
 */
void acelerarPwm(int pin_numero, int velocidad_inicial, int velocidad_final, float tiempo_total, const char* tipo) {
    
    // 1. Configurar el canal LEDC y asignar el pin
    ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(pin_numero, PWM_CHANNEL);
    
    // 2. Calcular pasos
    float diferencia = (float)velocidad_final - (float)velocidad_inicial;
    int pasos = 50; 
    float incremento = diferencia / pasos;
    // Conversión de segundos a milisegundos para delay()
    long delay_ms = (long)((tiempo_total / pasos) * 1000.0);
    
    Serial.print("🚀 Acelerando PWM Pin ");
    Serial.println(pin_numero);
    Serial.printf("    %d -> %d en %.1fs\n", velocidad_inicial, velocidad_final, tiempo_total);
    
    // Establecer velocidad inicial
    ledcWrite(PWM_CHANNEL, velocidad_inicial);
    
    // 3. Acelerar gradualmente
    for (int paso = 0; paso <= pasos; paso++) {
        float valor = 0.0;
        float progreso = (float)paso / pasos;
        
        // Determinar el valor según el tipo de curva
        if (strcmp(tipo, "lineal") == 0) {
            valor = (float)velocidad_inicial + (incremento * paso);
        } 
        else if (strcmp(tipo, "suave") == 0) {
            // Curva seno: 0 a PI/2
            float factor_suave = sin(progreso * PI / 2.0);
            valor = (float)velocidad_inicial + (diferencia * factor_suave);
        } 
        else if (strcmp(tipo, "rapido") == 0) {
            // Curva raíz cuadrada
            float factor_rapido = sqrt(progreso);
            valor = (float)velocidad_inicial + (diferencia * factor_rapido);
        } else {
            // Por defecto, lineal
            valor = (float)velocidad_inicial + (incremento * paso);
        }
        
        // Aplicar valor PWM y guardarlo en la global
        int duty = constrain((int)round(valor), 0, 1023);
        ledcWrite(PWM_CHANNEL, duty);
        last_duty_cycle = duty; // Almacena el valor actual
        
        // Mostrar progreso
        if (paso % 10 == 0) {
            int porcentaje = (int)(progreso * 100);
            Serial.printf("    %3d%% - PWM: %d\n", porcentaje, duty);
        }
        
        delay(delay_ms);
    }
    
    Serial.printf("✅ Aceleración completada - PWM: %d\n", velocidad_final);
    last_duty_cycle = velocidad_final; // Asegura que el duty final sea correcto
}

// ------------------------------------------------------------------

/**
 * @brief Desacelera PWM gradualmente hasta velocidad final.
 * * *NOTA: Esta función usa la variable global 'last_duty_cycle' para saber desde dónde
 * desacelerar. Asume que el pin ya está asignado al PWM_CHANNEL.*
 */
void desacelerarPwm(int pin_numero, int velocidad_final, float tiempo_total) {
    
    // Usamos el último valor aplicado por acelerarPwm
    int velocidad_actual = last_duty_cycle;
    
    float diferencia = (float)velocidad_actual - (float)velocidad_final;
    int pasos = 30;
    float incremento = diferencia / pasos;
    long delay_ms = (long)((tiempo_total / pasos) * 1000.0);

    Serial.println("🛑 Desacelerando PWM");
    Serial.printf("    %d -> %d en %.1fs\n", velocidad_actual, velocidad_final, tiempo_total);
    
    // Desacelerar gradualmente
    for (int paso = 0; paso <= pasos; paso++) {
        float progreso = (float)paso / pasos;
        float valor = (float)velocidad_actual - (incremento * paso);
        
        // Aplicar valor PWM
        int duty = constrain((int)round(valor), 0, 1023);
        ledcWrite(PWM_CHANNEL, duty);
        last_duty_cycle = duty; // Actualiza el duty cycle
        
        if (paso % 10 == 0) {
            int porcentaje = (int)(progreso * 100);
            Serial.printf("    %3d%% - PWM: %d\n", porcentaje, duty);
        }
        
        delay(delay_ms);
    }
    
    // Asegurar que el valor final es el correcto y apagar el PWM
    ledcWrite(PWM_CHANNEL, velocidad_final);
    last_duty_cycle = velocidad_final;

    Serial.printf("✅ Desaceleración completada - PWM: %d\n", velocidad_final);
    
    // Opcional: Desvincular el pin si ya no se usará el PWM
    // ledcDetachPin(pin_numero);
}