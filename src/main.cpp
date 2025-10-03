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

// ---ALARMA ---
void alarm();

// --- DECLARACIONES DE FUNCIONES DE PwmControl.cpp ---
void acelerarPwm(int pin_numero, int velocidad_inicial, int velocidad_final, float tiempo_total, const char* tipo);
void desacelerarPwm(int pin_numero, int velocidad_final, float tiempo_total);

// Función setup()
void setup() {
    Serial.begin(115200);

    // --- Configuración del módulo Buzzer ---
    configurarBuzzer();

    // Inicializa el puerto Serial2 para la comunicación con el GPS
    inicializarGPS(); 

    // Inicializar el ADC y el pin del relé
    configurarComponentes(); 


    //Control_Motores
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
    
    //--- PWM ---
    // 1. Llamada a la función de Aceleración
    acelerarPwm(
        25,          // Pin GPIO del motor
        0,           // Velocidad inicial
        900,         // Velocidad final
        4.0,         // Tiempo total (segundos)
        "suave"      // Tipo de curva
    );
    
    delay(5000); // Mantener la velocidad por 5 segundos

    // 2. Llamada a la función de Desaceleración
    // El pin_numero debe ser el mismo usado en acelerarPwm
    desacelerarPwm(
        25,          // Pin GPIO del motor
        0,           // Velocidad final (detener)
        1.5          // Tiempo total (segundos)
    );
    
    delay(10000); // Esperar 10 segundos antes de repetir
}