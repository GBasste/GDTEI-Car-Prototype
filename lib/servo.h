#ifndef CONTROL_MOTOR_H
#define CONTROL_MOTOR_H

#include <Arduino.h>

// Pines para los LEDs y botones
#define LED_LEFT 4
#define LED_CENTRO 16
#define LED_RIGHT 17
#define BOTON_RIGHT 19
#define BOTON_LEFT 18

// Declaraciones de funciones
void configurarControlMotor();
void apagarLeds();
void ejecutarControlMotor();

#endif