#include "PWM.h"
#include <math.h>

// Variables globales para los canales PWM
int _pwm_channels[16] = {0}; // Almacena el último valor duty por canal

/**
 * @brief Configura un pin como salida PWM.
 */
void configurarPwm(int pin_numero, int channel) {
    ledcSetup(channel, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(pin_numero, channel);
    ledcWrite(channel, 0); // Inicialmente apagado
}

/**
 * @brief Acelera un motor usando PWM.
 */
void acelerarPwm(int channel, int velocidad_inicial, int velocidad_final, float tiempo_total, const char* tipo) {
    int pasos = 50;
    float incremento = (float)(velocidad_final - velocidad_inicial) / pasos;
    float delay_ms = (tiempo_total * 1000.0) / pasos;
    
    Serial.printf("🚀 Acelerando PWM Canal %d\n", channel);
    Serial.printf("   %d -> %d en %.2fs\n", velocidad_inicial, velocidad_final, tiempo_total);
    
    ledcWrite(channel, velocidad_inicial);

    for (int paso = 0; paso <= pasos; paso++) {
        float valor = 0;
        float progreso = (float)paso / pasos;

        if (strcmp(tipo, "lineal") == 0) {
            valor = velocidad_inicial + (incremento * paso);
        } else if (strcmp(tipo, "suave") == 0) {
            float factor_suave = sin(progreso * M_PI / 2.0);
            valor = velocidad_inicial + ((velocidad_final - velocidad_inicial) * factor_suave);
        } else if (strcmp(tipo, "rapido") == 0) {
            float factor_rapido = sqrt(progreso);
            valor = velocidad_inicial + ((velocidad_final - velocidad_inicial) * factor_rapido);
        }

        ledcWrite(channel, (int)valor);
        _pwm_channels[channel] = (int)valor; // Guardar el valor actual

        if (paso % 10 == 0) {
            int porcentaje = (int)(progreso * 100);
            Serial.printf("   %3d%% - PWM: %d\n", porcentaje, (int)valor);
        }

        delay(delay_ms);
    }
    
    Serial.printf("✅ Aceleración completada - PWM: %d\n", velocidad_final);
}

/**
 * @brief Desacelera un motor usando PWM.
 */
void desacelerarPwm(int channel, int velocidad_inicial, int velocidad_final, float tiempo_total) {
    int pasos = 30;
    float diferencia = (float)(velocidad_inicial - velocidad_final);
    float incremento = diferencia / pasos;
    float delay_ms = (tiempo_total * 1000.0) / pasos;
    
    Serial.printf("🛑 Desacelerando PWM Canal %d\n", channel);
    Serial.printf("   %d -> %d en %.2fs\n", velocidad_inicial, velocidad_final, tiempo_total);

    for (int paso = 0; paso <= pasos; paso++) {
        float valor = velocidad_inicial - (incremento * paso);
        ledcWrite(channel, (int)valor);
        _pwm_channels[channel] = (int)valor; // Guardar el valor actual

        if (paso % 10 == 0) {
            int porcentaje = (int)(((float)paso / pasos) * 100);
            Serial.printf("   %3d%% - PWM: %d\n", porcentaje, (int)valor);
        }

        delay(delay_ms);
    }

    Serial.printf("✅ Desaceleración completada - PWM: %d\n", velocidad_final);
}