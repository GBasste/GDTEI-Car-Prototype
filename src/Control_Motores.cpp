#include <Arduino.h>

// ------------------------------------------------------------------
// Configuraciones y Constantes de Pines
// ------------------------------------------------------------------

// Pines de Dirección del Puente H (H-Bridge) o Driver:
const int IN1_PIN = 0;
const int IN2_PIN = 2;

// Pines de Control de Alimentación del Motor (Relés):
// rele1 controla la alimentación principal del motor
const int RELE1_PIN = 4;
// rele2 controla la alimentación de la dirección (in1/in2) para evitar conflictos
const int RELE2_PIN = 13;

// Declaración de funciones
void configurarPines();
void cerrarPuertas();
void abrirPuertas();
void apagarMotor();
void encenderMotor();
void apagarDireccion();

// ------------------------------------------------------------------
// Implementación de funciones
// ------------------------------------------------------------------

/**
 * @brief Configura todos los pines como salidas.
 */
void configurarPines() {
    pinMode(IN1_PIN, OUTPUT);
    pinMode(IN2_PIN, OUTPUT);
    pinMode(RELE1_PIN, OUTPUT);
    pinMode(RELE2_PIN, OUTPUT);
    
    // Inicializar pines en estado apagado (LOW)
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, LOW);
    // Asumimos que los relés son LOW-active (HIGH = OFF)
    digitalWrite(RELE1_PIN, HIGH); 
    digitalWrite(RELE2_PIN, HIGH);
}

/**
 * @brief Cierra las puertas (dirección hacia un lado, activa relé2 temporalmente).
 */
void cerrarPuertas() {
    Serial.println("Cerrar puertas");
    
    // 1. Activa relé2 (asumiendo LOW = ON para relés)
    digitalWrite(RELE2_PIN, LOW);
    delay(200); // time.sleep_ms(200)

    // 2. Establece la dirección (equivalente a in1.on(), in2.off())
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
    
    delay(200); // time.sleep_ms(200)
    
    // 3. Desactiva relé2
    digitalWrite(RELE2_PIN, HIGH);
}

/**
 * @brief Abre las puertas (dirección contraria, activa relé2 temporalmente).
 */
void abrirPuertas() {
    Serial.println("Abrir puertas");
    
    // 1. Activa relé2
    digitalWrite(RELE2_PIN, LOW);
    delay(200); // time.sleep_ms(200)

    // 2. Establece la dirección (equivalente a in1.off(), in2.on())
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
    
    delay(200); // time.sleep_ms(200)

    // 3. Desactiva relé2
    digitalWrite(RELE2_PIN, HIGH);
}

/**
 * @brief Desactiva la alimentación principal del motor (apagar motor).
 */
void apagarMotor() {
    // rele1.on() -> LOW si el relé es LOW-active
    digitalWrite(RELE1_PIN, HIGH);
    Serial.println("Motor apagado");
}

/**
 * @brief Activa la alimentación principal del motor (encender motor).
 */
void encenderMotor() {
    // rele1.off() -> LOW si el relé es LOW-active
    digitalWrite(RELE1_PIN, LOW);
    Serial.println("Motor encendido");
}

/**
 * @brief Apaga la dirección (detiene el motor si está encendido y permite que se frene).
 */
void apagarDireccion() {
    // in1.off() y in2.off()
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, LOW);
    Serial.println("Dirección apagada (freno inercial)");
}

// ------------------------------------------------------------------
// NO se incluye el bucle 'abrir_puertas()' ya que se debe llamar
// desde el setup() o loop() de tu main.cpp.
// ------------------------------------------------------------------