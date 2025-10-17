#include <Arduino.h>

// Definiciones de Pines
const int boton_off = 2;    // Botón OFF (N/O: HIGH al presionar)
const int huella_on = 4;    // Pulso ON (HIGH al detectar)
const int salida_rele = 15; // Salida al Relé (LOW para activar, HIGH para desactivar)

// Definiciones de Estado del Relé
const int RELE_ACTIVADO = LOW;
const int RELE_DESACTIVADO = HIGH;

// Variable de estado del relé (mantener seguimiento)
static int estadoRele = RELE_DESACTIVADO; 

void configurarSistema() {
    // Configuración de Pines
    pinMode(boton_off, INPUT);  // Botón N/O
    pinMode(huella_on, INPUT);  // Pulso de Huella
    pinMode(salida_rele, OUTPUT); // Conexión al relé

    // Inicializar el relé en estado DESACTIVADO (HIGH por defecto)
    digitalWrite(salida_rele, estadoRele);

    // Configuración de Comunicación Serial
    Serial.begin(115200);
    // Esperar que Serial esté listo (para placas con USB nativo)
    while (!Serial) {
        delay(10);
    }
    delay(1000);
    Serial.println("Sistema de control de relé inicializado.");
    Serial.print("Estado inicial del relé: ");
    Serial.println(estadoRele == RELE_ACTIVADO ? "ACTIVADO" : "DESACTIVADO");
}

void actualizarEstadoRele(int nuevoEstado, const char* fuente) {
    // Solo actualiza si el estado es diferente
    if (estadoRele != nuevoEstado) {
        estadoRele = nuevoEstado;
        digitalWrite(salida_rele, estadoRele);
        Serial.print("Relé cambiado a ");
        Serial.print(estadoRele == RELE_ACTIVADO ? "ACTIVADO" : "DESACTIVADO");
        Serial.print(" por: ");
        Serial.println(fuente);
    }
}

// ----------------------------------------------------------------------

// Función para manejar el flanco de subida (Press Event)
bool detectarFlanco(int pin, bool& estadoAnterior, bool nivelActivo) {
    // El nuevo voltaje de entrada (5V al activar) se mantiene en HIGH
    bool estadoActual = (digitalRead(pin) == nivelActivo);
     bool flancoDetectado = estadoActual && !estadoAnterior;
    estadoAnterior = estadoActual;
    return flancoDetectado;
}

// ----------------------------------------------------------------------

void ejecutarSistema() {
    // Variables estáticas para detección de flancos
    static bool estadoAnteriorBotonOff = false;
    static bool estadoAnteriorHuellaOn = false;

    // --- Lógica de Botón OFF (Pin 2) ---
    // El botón es N/O: Manda HIGH (5V) al presionar
    if (detectarFlanco(boton_off, estadoAnteriorBotonOff, HIGH)) {
        actualizarEstadoRele(RELE_DESACTIVADO, "Boton OFF (Pin 2)");
    }

    // --- Lógica de Huella ON (Pin 4) ---
    // La huella manda HIGH (5V) al detectar
    if (detectarFlanco(huella_on, estadoAnteriorHuellaOn, HIGH)) {
        actualizarEstadoRele(RELE_ACTIVADO, "Huella ON (Pin 4)");
    }

    // --- Procesamiento de Comandos Seriales ---
    if (Serial.available() > 0) {
        // Imprimir mensaje solo si hay datos para leer
        Serial.println("--- COMANDO SERIAL ---");
        Serial.print("Estado actual (1=ON, 0=OFF): ");
        
        // Usar Serial.read() para leer un solo byte de forma no bloqueante
        char comando = Serial.read();

        if (comando == '1') {
            actualizarEstadoRele(RELE_ACTIVADO, "Comando Serial '1'");
        } else if (comando == '0') {
            actualizarEstadoRele(RELE_DESACTIVADO, "Comando Serial '0'");
        } else if (comando != 10 && comando != 13) { // Ignorar Enter (LF/CR)
            // Solo imprimir error si no es un carácter de control
            Serial.print("Comando no valido recibido: '");
            Serial.print(comando);
            Serial.println("'. Use 1 o 0.");
        }

        // Limpiar el buffer serial (asegura que solo se procesa un comando)
        while(Serial.available() > 0) {
            Serial.read();
        }
        Serial.println("----------------------");
    }

    delay(50); // Delay para estabilidad y debounce
}