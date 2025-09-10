#include "motor_l298n.h"

// Función de configuración de los pines
void configurarMotorL298N() {
    pinMode(IN1_PIN, OUTPUT);
    pinMode(IN2_PIN, OUTPUT);
    pinMode(IN3_PIN, OUTPUT);
    pinMode(IN4_PIN, OUTPUT);

    // Detener el motor al inicio
    detenerMotorL298N();
}

// Función para mover el motor en una dirección
// 'direccion1' = true -> dirección 1
// 'direccion1' = false -> dirección 2
void moverMotorL298N(bool direccion1) {
    if (direccion1) {
        digitalWrite(IN1_PIN, HIGH);
        digitalWrite(IN2_PIN, LOW);
        digitalWrite(IN3_PIN, HIGH);
        digitalWrite(IN4_PIN, LOW);
    } else {
        digitalWrite(IN1_PIN, LOW);
        digitalWrite(IN2_PIN, HIGH);
        digitalWrite(IN3_PIN, LOW);
        digitalWrite(IN4_PIN, HIGH);
    }
}

// Función para detener el motor
void detenerMotorL298N() {
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, LOW);
    digitalWrite(IN3_PIN, LOW);
    digitalWrite(IN4_PIN, LOW);
}

// Función de prueba simple para verificar el funcionamiento
void testSimpleMotor() {
    Serial.println("=== TEST SIMPLE ===");

    // 1. Todo apagado
    detenerMotorL298N();
    Serial.println("1. Todo apagado");
    delay(10000);

    // 2. Probando dirección 1
    Serial.println("2. Probando dirección 1...");
    moverMotorL298N(true);
    delay(10000);

    // 3. Parar
    Serial.println("3. Parado");
    detenerMotorL298N();
    delay(10000);

    // 4. Probando dirección 2
    Serial.println("4. Probando dirección 2...");
    moverMotorL298N(false);
    delay(10000);
    
    // 5. Probando dirección 1 de nuevo
    Serial.println("5. Probando dirección 1 de nuevo...");
    moverMotorL298N(true);
    delay(10000);

    Serial.println("\n¿Se movió el motor? Si no:");
    Serial.println("- Revisar alimentación 12V");
    Serial.println("- Revisar jumper ENA");
    Serial.println("- Revisar cables OUT1/OUT2");
}