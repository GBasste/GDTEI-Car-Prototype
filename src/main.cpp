#include <Arduino.h>


// Declaración de funciones existentes
void configurarSistema();
void ejecutarSistema();
void CamConfig();
void Camexe();
void motorset();
void cerrar_puertas();
void abrir_puertas();
void apagar_motor();
void encender_motor();
void apagar();

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
// Declaraciones de funciones (prototipos) de MotorControl.cpp
PwmState_t acelerarPwm(int pin_numero, int velocidad_inicial, int velocidad_final, float tiempo_total, const char* tipo);
int desacelerarPwm(PwmState_t pwm_state, int velocidad_final, float tiempo_total);

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
    
    // --- Configuración de tus funciones originales ---
    configurarSistema();
    CamConfig();
    motorset();

    // --- Configuración del módulo PWM del motor ---
    configurarPwm(MOTOR_PIN, MOTOR_CHANNEL);

    // --- Configuración del módulo Buzzer ---
    configurarBuzzer();

    // Inicializa el puerto Serial2 para la comunicación con el GPS
    inicializarGPS(); 

    // Inicializar el ADC y el pin del relé
    configurarComponentes(); 

    // --- Configuración del módulo Control Motor ---
    configurarControlMotor();
    
    // --- Configuración del módulo Motor L298N ---
    configurarMotorL298N();
    configurarUltrasonico();

    configurarGps(); // <--- Llama a la nueva función de configuración
}

// Función loop()
void loop() {
    // La función que mide el voltaje y controla el relé
    medirVoltajeYControlarRele(); 
    
    // El delay(500) ya está incluido dentro de la función, pero 
    // si lo quitas de VoltageMonitor.cpp, lo debes poner aquí:
    // delay(500); 
    
    // --- Bucle del módulo Control Motor ---
    ejecutarControlMotor();
    ejecutarUltrasonico();

    //--- alarma ---
    alarm();
    
    // --- Ejemplo de uso del módulo L298N ---
    // Puedes llamar a la función de prueba para verificar el funcionamiento
    // Comenta o elimina esta sección si no la necesitas.
    testSimpleMotor();
    
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