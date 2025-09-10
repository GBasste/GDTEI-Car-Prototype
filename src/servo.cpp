#include "control_motor.h"

// Variables de control de estado
bool ctrl_right = false;
bool ctrl_left = false;

// Función para inicializar los pines
void configurarControlMotor() {
    // Configuración de los pines de los LEDs como salida
    pinMode(LED_LEFT, OUTPUT);
    pinMode(LED_CENTRO, OUTPUT);
    pinMode(LED_RIGHT, OUTPUT);

    // Configuración de los pines de los botones como entrada con PULL_UP
    // El PULL_UP interno asegura que la lectura sea HIGH (1) cuando no se presiona
    pinMode(BOTON_RIGHT, INPUT_PULLUP);
    pinMode(BOTON_LEFT, INPUT_PULLUP);
    
    apagarLeds();
    Serial.println("Controlador de motor iniciado...");
}

// Función para apagar todos los LEDs
void apagarLeds() {
    digitalWrite(LED_LEFT, LOW);
    digitalWrite(LED_CENTRO, LOW);
    digitalWrite(LED_RIGHT, LOW);
}

// Lógica de control principal
void ejecutarControlMotor() {
    // Leer el estado de los botones
    int estado_right = digitalRead(BOTON_RIGHT); // 0 = presionado, 1 = no presionado
    int estado_left = digitalRead(BOTON_LEFT);   // 0 = presionado, 1 = no presionado

    // Giro a la derecha (mantener presionado botón derecho)
    if (estado_right == LOW && estado_left == HIGH) {
        digitalWrite(LED_RIGHT, HIGH);
        digitalWrite(LED_LEFT, LOW);
        digitalWrite(LED_CENTRO, LOW);
        ctrl_right = true;
        ctrl_left = false;
    } 
    // Giro a la izquierda (mantener presionado botón izquierdo)
    else if (estado_right == HIGH && estado_left == LOW) {
        digitalWrite(LED_LEFT, HIGH);
        digitalWrite(LED_RIGHT, LOW);
        digitalWrite(LED_CENTRO, LOW);
        ctrl_left = true;
        ctrl_right = false;
    } 
    // Corrección automática cuando se sueltan los botones
    else if (estado_right == HIGH && estado_left == HIGH) {
        if (ctrl_left) {
            Serial.println("Corrigiendo giro izquierdo...");
            for (int i = 0; i < 4; i++) {
                digitalWrite(LED_RIGHT, HIGH);
                digitalWrite(LED_CENTRO, HIGH);
                digitalWrite(LED_LEFT, LOW);
                delay(200);
                apagarLeds();
                delay(100);
            }
            ctrl_left = false;
        } else if (ctrl_right) {
            Serial.println("Corrigiendo giro derecho...");
            for (int i = 0; i < 4; i++) {
                digitalWrite(LED_LEFT, HIGH);
                digitalWrite(LED_CENTRO, HIGH);
                digitalWrite(LED_RIGHT, LOW);
                delay(200);
                apagarLeds();
                delay(100);
            }
            ctrl_right = false;
        }
    } 
    // Ambos botones están presionados, mantener posición central
    else if (estado_right == LOW && estado_left == LOW) {
        digitalWrite(LED_CENTRO, HIGH);
        digitalWrite(LED_RIGHT, LOW);
        digitalWrite(LED_LEFT, LOW);
        ctrl_right = false;
        ctrl_left = false;
    }

    delay(50); // Pequeña pausa para evitar lecturas excesivas
}