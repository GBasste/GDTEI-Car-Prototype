#include <Arduino.h>
#include <cmath> // Necesario para math.sqrt y math.sin

// ------------------------------------------------------------------
// Configuraciones y Constantes PWM
// ------------------------------------------------------------------

// La frecuencia típica para motores es 1kHz (1000 Hz)
#define PWM_FREQUENCY 1000 
// Resolución de 10 bits (0-1023), coincidiendo con el código Python
#define PWM_RESOLUTION 10  
// Canal de LEDC a usar (se puede elegir cualquiera, el 0 es común)
#define PWM_CHANNEL 0      

// Definición de una estructura para retornar los datos del PWM configurado
// Esto es el equivalente a retornar el objeto 'pwm' de Python.
struct PwmState_t {
    int pin;
    int channel;
    int current_duty;
};

// ------------------------------------------------------------------
// Declaraciones de funciones
// ------------------------------------------------------------------

PwmState_t acelerarPwm(int pin_numero, int velocidad_inicial, int velocidad_final, float tiempo_total, const char* tipo);
int desacelerarPwm(PwmState_t pwm_state, int velocidad_final, float tiempo_total);
// ------------------------------------------------------------------
// Implementación de funciones
// ------------------------------------------------------------------

/**
 * Acelera PWM gradualmente desde velocidad inicial hasta final.
 * * Retorna: Una estructura PwmState_t con el estado final.
 */
PwmState_t acelerarPwm(int pin_numero, int velocidad_inicial, int velocidad_final, float tiempo_total, const char* tipo) {
    
    // 1. Configurar el canal LEDC (LEDC es el nombre del PWM en ESP32)
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
        
        if (strcmp(tipo, "lineal") == 0) {
            // Aceleración lineal constante
            valor = (float)velocidad_inicial + (incremento * paso);
        } 
        else if (strcmp(tipo, "suave") == 0) {
            // Aceleración suave (curva seno: 0 a PI/2)
            float factor_suave = sin(progreso * PI / 2.0);
            valor = (float)velocidad_inicial + (diferencia * factor_suave);
        } 
        else if (strcmp(tipo, "rapido") == 0) {
            // Aceleración rápida al inicio, lenta al final (curva raíz cuadrada)
            float factor_rapido = sqrt(progreso);
            valor = (float)velocidad_inicial + (diferencia * factor_rapido);
        } else {
            // Por defecto, usar lineal si el tipo es inválido
            valor = (float)velocidad_inicial + (incremento * paso);
        }
        
        // Aplicar valor PWM, asegurando que esté dentro del rango
        int duty = constrain((int)round(valor), 0, 1023);
        ledcWrite(PWM_CHANNEL, duty);
        
        // Mostrar progreso
        if (paso % 10 == 0) {
            int porcentaje = (int)(progreso * 100);
            Serial.printf("    %3d%% - PWM: %d\n", porcentaje, duty);
        }
        
        delay(delay_ms);
    }
    
    Serial.printf("✅ Aceleración completada - PWM: %d\n", velocidad_final);

    // Retornar el estado final para usar en la desaceleración
    PwmState_t state = {pin_numero, PWM_CHANNEL, velocidad_final};
    return state;
}

// ------------------------------------------------------------------

/**
 * Desacelera PWM gradualmente hasta velocidad final.
 * * Retorna: Valor final aplicado.
 */
int desacelerarPwm(PwmState_t pwm_state, int velocidad_final, float tiempo_total) {
    
    // El 'pwm_obj' de Python ahora se pasa como 'pwm_state' en C++
    int velocidad_actual = pwm_state.current_duty;
    
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
        ledcWrite(pwm_state.channel, duty);
        
        if (paso % 10 == 0) {
            int porcentaje = (int)(progreso * 100);
            Serial.printf("    %3d%% - PWM: %d\n", porcentaje, duty);
        }
        
        delay(delay_ms);
    }
    
    // Asegurar que el valor final es el correcto
    ledcWrite(pwm_state.channel, velocidad_final);

    // Desvincular el pin para liberar el canal LEDC (opcional, pero buena práctica)
    // ledcDetachPin(pwm_state.pin);

    Serial.printf("✅ Desaceleración completada - PWM: %d\n", velocidad_final);
    return velocidad_final;
}