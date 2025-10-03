#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

// Definiciones de canales y frecuencia
#define BUZZER_CHANNEL 13    // Canal LEDC para el zumbador
#define BUZZER_RESOLUTION 10 // 10 bits de resolución (0-1023)

// Declaraciones de funciones públicas
void configurarBuzzer();
void beepSimple(float duration);
void beepTone(int frequency, float duration);
void playNote(int frequency, float duration);
void playMelody();
void playMarioTheme();
void alarm();
void buzzOn();
void buzzOff();
void cleanupBuzzer();

#endif