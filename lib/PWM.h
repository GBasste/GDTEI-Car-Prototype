#ifndef MOTOR_PWM_H
#define MOTOR_PWM_H

#include <Arduino.h>

// Definiciones de canales y frecuencia
#define PWM_FREQUENCY 1000  // 1kHz
#define PWM_RESOLUTION 10   // 10 bits de resolución (0-1023)

/**
 * @brief Configura un pin como salida PWM.
 * * @param pin_numero El número del pin GPIO.
 * @param channel El canal LEDC a utilizar (0-15).
 */
void configurarPwm(int pin_numero, int channel);

/**
 * @brief Acelera un motor usando PWM.
 * * @param channel El canal LEDC del PWM.
 * @param velocidad_inicial Valor PWM inicial (0-1023).
 * @param velocidad_final Valor PWM final (0-1023).
 * @param tiempo_total Tiempo total de aceleración en segundos.
 * @param tipo Tipo de aceleración ("lineal", "suave", "rapido").
 */
void acelerarPwm(int channel, int velocidad_inicial, int velocidad_final, float tiempo_total, const char* tipo);

/**
 * @brief Desacelera un motor usando PWM.
 * * @param channel El canal LEDC del PWM.
 * @param velocidad_inicial Valor PWM inicial.
 * @param velocidad_final Valor PWM final (0-1023).
 * @param tiempo_total Tiempo total de desaceleración en segundos.
 */
void desacelerarPwm(int channel, int velocidad_inicial, int velocidad_final, float tiempo_total);

#endif