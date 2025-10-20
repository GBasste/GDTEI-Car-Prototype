#include <Arduino.h>

const int ledPin2 = 3;
const int ledPin1 = 1;
const int buttonPin = 23;

bool lastButtonState = HIGH;
bool currentButtonState = HIGH;
bool ledState = false;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void CCPINCONFIG() {
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

// Variables globales (añádelas en la parte superior de Cierre_Central.cpp)
static unsigned long tiempoInicioBloqueo = 0;
static bool bloqueoActivo = false;
const unsigned long duracionBloqueo = 2000; // 2000ms (2 segundos)

void CCEXE() {
    // --- 1. Manejo del bloqueo activo (NO BLOQUEANTE) ---
    if (bloqueoActivo) {
        // Si han pasado 2 segundos desde el inicio del bloqueo
        if (millis() - tiempoInicioBloqueo >= duracionBloqueo) {
            // Fin del bloqueo: Apagar ambos LEDs/Actuadores
            digitalWrite(ledPin1, LOW);
            digitalWrite(ledPin2, LOW);
            bloqueoActivo = false; // Permitir nueva lectura de botón
        }
        // Si el bloqueo está activo, salimos inmediatamente.
        return; 
    }

    // --- 2. Lógica del Botón (solo si NO estamos en bloqueo) ---
    int reading = digitalRead(buttonPin);

    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != currentButtonState) {
            currentButtonState = reading;

            if (currentButtonState == LOW) { // Flanco de bajada (botón presionado)
                ledState = !ledState;

                // INICIO DE LA ACCIÓN y del TEMPORIZADOR NO BLOQUEANTE
                if (ledState) {
                    digitalWrite(ledPin1, HIGH);
                    digitalWrite(ledPin2, LOW);
                } else {
                    digitalWrite(ledPin1, LOW);
                    digitalWrite(ledPin2, HIGH);
                }

                // Activar el temporizador de bloqueo
                tiempoInicioBloqueo = millis();
                bloqueoActivo = true;
            }
        }
    }

    lastButtonState = reading;
}
