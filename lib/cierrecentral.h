#ifndef MOTOR_L298N_H
#define MOTOR_L298N_H

#include <Arduino.h>

// Pines para el control del motor
#define IN1_PIN 32
#define IN2_PIN 33
#define IN3_PIN 25
#define IN4_PIN 26

// Declaración de funciones
void configurarMotorL298N();
void moverMotorL298N(bool direccion1);
void detenerMotorL298N();
void testSimpleMotor();

#endif