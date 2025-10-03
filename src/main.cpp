#include <Arduino.h>


// Declaración de funciones de Nuevo_Interruptor.cpp ---
void configurarSistema();
void ejecutarSistema();

// Declaracion de funciones de Control_Motores ---
void configurarPines();
void cerrarPuertas();
void abrirPuertas();
void apagarMotor();
void encenderMotor();
void apagarDireccion();

// --- DECLARACIONES DE FUNCIONES DE BuzzerControl.cpp ---
void configurarBuzzer();
void beepSimple(float duration);
// ... (pegar el resto de las declaraciones aquí)
void cleanupBuzzer();

// Declaración de la estructura para manejar el estado del PWM
struct PwmState_t {
    int pin;
    int channel;
    int current_duty;
}

// --- DECLARACIONES DE FUNCIONES DE GPSControl.cpp ---
void inicializarGPS();
void ejecutarGPS();

// --- DECLARACIONES DE FUNCIONES DE VoltageMonitor.cpp ---
void configurarComponentes();
void medirVoltajeYControlarRele();
// -------------------------------------------------------


// --- ULTRASONICO ---
void configurarUltrasonico();
void ejecutarUltrasonico();

// Función setup()
void setup() {
    Serial.begin(115200);

    // --- Configuración del módulo PWM del motor ---
    configurarPwm(MOTOR_PIN, MOTOR_CHANNEL);

    // --- Configuración del módulo Buzzer ---
    configurarBuzzer();

    // Inicializa el puerto Serial2 para la comunicación con el GPS
    inicializarGPS(); 

    // Inicializar el ADC y el pin del relé
    configurarComponentes(); 

   // Inicializa todos los pines
    configurarPines(); 
    
    // La acción inicial del código Python
    encenderMotor(); // Asegura que el motor tenga alimentación
    abrirPuertas();
    apagarDireccion();
}

// Función loop()
void loop() {
    // La función que mide el voltaje y controla el relé
    medirVoltajeYControlarRele(); 
    
    // El delay(500) ya está incluido dentro de la función, pero 
    // si lo quitas de VoltageMonitor.cpp, lo debes poner aquí:
    // delay(500); 
    
    // Control_Motores ---
    // Tu lógica principal iría aquí, por ejemplo:
    // Si se recibe un comando de Node-RED para cerrar:
    // cerrarPuertas();
    // delay(5000); // Espera a que termine de cerrar
    // apagarDireccion();

    //--- alarma ---
    alarm();
    
    // La función que lee y procesa los datos NMEA
    ejecutarGPS();
    
    // Pausa mínima para no saturar el CPU (equivalente al sleep(1) del Python)
    // El GPS envía datos a 1Hz (cada segundo), así que un delay de 500ms es suficiente.
    delay(500); 

    // --------------------------------------------------------
    // EJEMPLO DE USO: Acelerar linealmente
    // --------------------------------------------------------
    Serial.println("\n--- INICIO CICLO ---");
    
    // Acelera de 0 a 800 en 3.0 segundos de forma "suave"
    PwmState_t motor_state = acelerarPwm(
        25,          // Pin GPIO del motor
        0,           // Velocidad inicial
        800,         // Velocidad final
        3.0,         // Tiempo total (segundos)
        "suave"      // Tipo de curva
    );
    
    delay(4000); // Mantener la velocidad por 4 segundos
    
    // Desacelera el motor de 800 a 0 en 1.5 segundos
    desacelerarPwm(
        motor_state, // Estado actual retornado por acelerarPwm
        0,           // Velocidad final
        1.5          // Tiempo total (segundos)
    );
    
    delay(5000); // Esperar 5 segundos antes de repetir
    
    // Aquí puedes llamar a otras funciones de tu proyecto
    // como Camexe(), ejecutarSistema(), etc.
    // Por ejemplo:
    // Camexe();
    // ejecutarSistema();

    
    
}