#include "alarma.h"
#include <Arduino.h>

// Pines para el zumbador y el LED
const int BUZZER_PIN = 27;
const int LED_PIN = 14;

// Definiciones de notas musicales
const int NOTE_C4 = 262;
const int NOTE_D4 = 294;
const int NOTE_E4 = 330;
const int NOTE_F4 = 349;
const int NOTE_G4 = 392;
const int NOTE_A4 = 440;
const int NOTE_B4 = 494;
const int NOTE_C5 = 523;

// Inicializa el zumbador
void configurarBuzzer() {
    ledcSetup(BUZZER_CHANNEL, 1000, BUZZER_RESOLUTION); // Frecuencia inicial 1kHz
    ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
    ledcWrite(BUZZER_CHANNEL, 0); // Silencio inicial
}

// Sonido simple de encendido/apagado
void beepSimple(float duration) {
    ledcWrite(BUZZER_CHANNEL, 512); // Tono simple (50% duty cycle)
    delay(duration * 1000);
    ledcWrite(BUZZER_CHANNEL, 0);
    delay(100);
}

// Genera un tono con frecuencia específica
void beepTone(int frequency, float duration) {
    if (frequency > 0) {
        ledcSetup(BUZZER_CHANNEL, frequency, BUZZER_RESOLUTION);
        ledcWrite(BUZZER_CHANNEL, 512); // 50% duty cycle
        delay(duration * 1000);
        ledcWrite(BUZZER_CHANNEL, 0);
    } else {
        delay(duration * 1000);
    }
}

// Tocar una nota musical
void playNote(int frequency, float duration) {
    if (frequency > 0) {
        beepTone(frequency, duration);
    } else {
        delay(duration * 1000); // Pausa
    }
    delay(50); // Pausa entre notas
}

// Tocar una melodía simple
void playMelody() {
    int melody[] = {NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5};
    int melodyLength = sizeof(melody) / sizeof(melody[0]);

    Serial.println("Tocando melodía...");
    for (int i = 0; i < melodyLength; i++) {
        playNote(melody[i], 0.3);
    }
}

// Tema clásico de Mario Bros (fragmento)
void playMarioTheme() {
    int mario_notes[] = {
        659, 659, 0, 659, 0, 523, 659, 0, 784,
        392, 0, 523, 0, 392, 0, 330, 0, 440,
        0, 494, 0, 466, 440, 0, 392, 659, 784, 880
    };
    float mario_durations[] = {
        0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.3,
        0.3, 0.15, 0.3, 0.15, 0.3, 0.15, 0.3, 0.15, 0.3,
        0.15, 0.3, 0.15, 0.15, 0.3, 0.15, 0.3, 0.15, 0.15, 0.15
    };

    int notesCount = sizeof(mario_notes) / sizeof(mario_notes[0]);

    Serial.println("Tocando tema de Mario...");
    for (int i = 0; i < notesCount; i++) {
        playNote(mario_notes[i], mario_durations[i]);
    }
}

// Sonido de alarma alternante
void alarm() {
    Serial.println("¡ALARMA!");
    for (int i = 0; i < 10; i++) {
        beepTone(2000, 0.15);
        beepTone(1000, 0.15);
    }
}

// Encender zumbador continuo (solo se puede apagar con buzzOff o cleanup)
void buzzOn() {
    ledcSetup(BUZZER_CHANNEL, 1000, BUZZER_RESOLUTION);
    ledcWrite(BUZZER_CHANNEL, 512); // Tono continuo
}

// Apagar zumbador
void buzzOff() {
    ledcWrite(BUZZER_CHANNEL, 0); // Apagar el PWM
    Serial.println("Buzzer apagado");
}

// Limpiar y apagar el zumbador
void cleanupBuzzer() {
    ledcWrite(BUZZER_CHANNEL, 0);
    ledcDetachPin(BUZZER_PIN);
    Serial.println("Buzzer desconfigurado");
}