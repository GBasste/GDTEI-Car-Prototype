#include <Arduino.h>

// Definición de pines
const int pin_apagar = 2;   // Botón de OFF (Normalmente Abierto, LOW al presionar)
const int pin_encender = 4; // Pulso del sensor de huella (HIGH al detectar)
const int pin_rele = 35;    // Conexión al relé (Activa con LOW)

// Estados de control
enum ReleEstado {
  APAGADO,
  ENCENDIDO
};
ReleEstado estadoActualRele = APAGADO; // Inicialmente el relé está APAGADO (HIGH)

// Variables para debounce y detección de flancos
unsigned long tiempoUltimoCambio = 0;
const long retardoDebounce = 50; // Retardo para debounce en ms

// --- Funciones de control del relé ---

/**
 * @brief Cambia el estado del relé.
 * * @param nuevoEstado El estado deseado (ENCENDIDO o APAGADO).
 */
void cambiarEstadoRele(ReleEstado nuevoEstado) {
  if (nuevoEstado != estadoActualRele) {
    estadoActualRele = nuevoEstado;
    if (estadoActualRele == ENCENDIDO) {
      // El relé activa con LOW
      digitalWrite(pin_rele, LOW); 
      Serial.println("-> Rele ACTIVADO (LOW)");
    } else {
      // El relé desactiva con HIGH (estado por defecto de Pin 35)
      digitalWrite(pin_rele, HIGH);
      Serial.println("-> Rele DESACTIVADO (HIGH)");
    }
  }
}


void configurarSistema() {
  // Configuración de pines
  // Botón de OFF y sensor de huella configurados como INPUT (asumiendo que los voltajes de 1.6V y 3.3V son detectados como HIGH por el ESP32/Arduino)
  pinMode(pin_apagar, INPUT); 
  pinMode(pin_encender, INPUT); 
  pinMode(pin_rele, OUTPUT);

  // Inicialización del relé al estado APAGADO (HIGH)
  cambiarEstadoRele(APAGADO); 

  // Inicialización de la comunicación serial
  Serial.begin(115200);
  while (!Serial) {
    // Esperar que Serial esté listo (solo necesario para algunas placas)
    delay(10);
  }
  Serial.println("\n--- Sistema de Control de Relé Iniciado ---");
}


void ejecutarSistema() {
  unsigned long tiempoActual = millis();

  // --- Lógica de Control por Botones/Huella (Detección de Flancos y Debounce) ---
  
  // Lectura de los pines
  // Usamos el estado contrario para el botón de apagar ya que es N/O y está conectado a LOW al presionar
  bool boton_apagar_activo = (digitalRead(pin_apagar) == HIGH); // El N/O va de 0V a 1.6V (HIGH) al presionar
  bool huella_encender_activa = (digitalRead(pin_encender) == HIGH); // La huella va de 0V a 1.6V (HIGH) al detectar

  // Solo procesar si ha pasado el tiempo de debounce
  if ((tiempoActual - tiempoUltimoCambio) > retardoDebounce) {
    
    // Detección de activación (botón ON / huella)
    if (huella_encender_activa && estadoActualRele == APAGADO) {
      // Solo encender si actualmente está apagado y se detecta la señal de la huella
      cambiarEstadoRele(ENCENDIDO);
      tiempoUltimoCambio = tiempoActual;
    } 
    
    // Detección de desactivación (botón OFF)
    else if (boton_apagar_activo && estadoActualRele == ENCENDIDO) {
      // Solo apagar si actualmente está encendido y se detecta la señal del botón
      cambiarEstadoRele(APAGADO);
      tiempoUltimoCambio = tiempoActual;
    }
  }

  // --- Procesamiento de Comandos Seriales ---

  if (Serial.available() > 0) {
    // Limpiar el buffer si es una nueva línea de comando
    // Esto previene que se impriman mensajes incompletos o comandos viejos
    while (Serial.peek() == '\n' || Serial.peek() == '\r') {
      Serial.read();
    }
    
    Serial.println("\n--- Probador de Relé por Serial ---");
    Serial.print("Estado actual: ");
    Serial.println(estadoActualRele == ENCENDIDO ? "ENCENDIDO" : "APAGADO");
    Serial.println("Activar relé (1) / Apagar relé (0): ");
    
    // Leer el comando enviado
    int comando = Serial.parseInt();
    
    // Limpiar cualquier residuo después del parseInt (como el '\n')
    while(Serial.available() > 0) {
      Serial.read();
    }

    Serial.print("Comando recibido: ");
    Serial.println(comando);
    
    if (comando == 1) {
      cambiarEstadoRele(ENCENDIDO);
    } else if (comando == 0) {
      cambiarEstadoRele(APAGADO);
    } else {
      Serial.println("Comando no válido. Use 1 o 0.");
    }
  }
}