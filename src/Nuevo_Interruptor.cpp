#include <Arduino.h>

// Definiciones de Pines
const int boton_off = 15;    // Botón OFF (N/O: HIGH al presionar)
const int huella_on = 4;    // Pulso ON (HIGH al detectar)
const int salida_rele = 25; // Salida al Relé (LOW para activar, HIGH para desactivar)

// Definiciones de Estado del Relé
const int RELE_ACTIVADO = LOW;
const int RELE_DESACTIVADO = HIGH;

// Variable de estado del relé (mantener seguimiento)
static int estadoRele = RELE_DESACTIVADO; 

// Variables estáticas para Debounce y Detección de Estado
// Usaremos estas variables para un Debounce más robusto
static bool estadoActualBotonOff = false;
static bool estadoActualHuellaOn = false;
static unsigned long ultimoTiempoDebounce = 0;
const long tiempoDebounce = 50; // 50ms para Debounce (Antirrebote)

void configurarSistema() {
    // Configuración de Pines
    pinMode(boton_off, INPUT);    // Botón N/O
    pinMode(huella_on, INPUT);    // Pulso de Huella
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

/*
  La función detectarFlanco original es correcta para el flanco de subida
  (HIGH al presionar), pero la movemos a una lógica más limpia en loop()
  para un mejor control de las variables de estado y Debounce.

bool detectarFlanco(int pin, bool& estadoAnterior, bool nivelActivo) {
    bool estadoActual = (digitalRead(pin) == nivelActivo);
    bool flancoDetectado = estadoActual && !estadoAnterior;
    estadoAnterior = estadoActual; // Esta línea es crítica y funciona
    return flancoDetectado;
}
*/

// ----------------------------------------------------------------------

void ejecutarSistema() {
    // --- Lógica de Botón y Huella (Con Debounce Temporal) ---
    // Usamos millis() para un loop no bloqueante
    if ((millis() - ultimoTiempoDebounce) > tiempoDebounce) {
        
        // --- Lógica de Botón OFF (Pin 2) ---
        int lecturaBotonOff = digitalRead(boton_off);
        // El botón es N/O: Manda HIGH (5V) al presionar
        bool flancoBotonOff = (lecturaBotonOff == HIGH) && !estadoActualBotonOff;
        estadoActualBotonOff = (lecturaBotonOff == HIGH); // Actualiza el estado
        
        if (flancoBotonOff) {
            actualizarEstadoRele(RELE_DESACTIVADO, "Boton OFF (Pin 2)");
        }

        // --- Lógica de Huella ON (Pin 4) ---
        int lecturaHuellaOn = digitalRead(huella_on);
        // La huella manda HIGH (5V) al detectar
        bool flancoHuellaOn = (lecturaHuellaOn == HIGH) && !estadoActualHuellaOn;
        estadoActualHuellaOn = (lecturaHuellaOn == HIGH); // Actualiza el estado
        
        if (flancoHuellaOn) {
            actualizarEstadoRele(RELE_ACTIVADO, "Huella ON (Pin 4)");
        }
        
        // Actualizar el tiempo de la última revisión de debounce
        ultimoTiempoDebounce = millis();

        // --- AÑADE ESTA LÍNEA PARA DEPURAR ---
        // Esto solo se imprimirá cada 50ms
        Serial.printf("[DEBUG PINS] Boton OFF: %d, Huella ON: %d, Rele State: %d\n", 
                      lecturaBotonOff, lecturaHuellaOn, estadoRele);
        // ------------------------------------
    }

    // --- Procesamiento de Comandos Seriales (No requiere Debounce) ---
    if (Serial.available() > 0) {
        // Usar Serial.read() para leer un solo byte de forma no bloqueante
        char comando = Serial.read();

        // Imprimir mensaje solo si hay datos para leer
        Serial.println("--- COMANDO SERIAL ---");
        
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

        // Limpiar el buffer serial (procesar el resto del buffer)
        while(Serial.available() > 0) {
            Serial.read();
        }
        Serial.println("----------------------");
    }

    // El loop es no bloqueante ahora, no se necesita delay.
}