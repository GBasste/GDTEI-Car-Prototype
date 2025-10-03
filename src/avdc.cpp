#include "avdc.h"
#include <driver/adc.h>

// Definiciones de pines y constantes
const int PIN_ADC = 34;
const int PIN_RELE = 25;
const float FACTOR_DIVISOR = 4.7;

// Variables para el ADC y el relé
adc1_channel_t canal_adc = ADC1_CHANNEL_6; // GPIO34 es ADC1_CHANNEL_6
gpio_num_t pin_rele = (gpio_num_t)PIN_RELE;

// Función para la configuración inicial
void configurarAdcRele() {
    // Configuración del ADC
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(canal_adc, ADC_ATTEN_DB_11);

    // Configuración del relé
    pinMode(pin_rele, OUTPUT);
    digitalWrite(pin_rele, HIGH); // Inicialmente desactivado (HIGH)
}

// Función del bucle principal
void ejecutarAdcRele() {
    // Lectura del ADC
    int valor_adc = adc1_get_raw(canal_adc);
    
    // Cálculo del voltaje
    // Nota: El voltaje de referencia del ADC del ESP32 es ~3.3V
    // Si usas la atenuación de 11dB, el rango máximo es ~3.6V, pero el factor de conversión
    // 3.3/4095 es una buena aproximación. Para mayor precisión, se puede usar `analogReadMilliVolts()`.
    float voltaje_adc = (float)valor_adc * (3.3 / 4095.0);
    float voltaje_real = voltaje_adc * FACTOR_DIVISOR;

    Serial.print("Voltaje real: ");
    Serial.print(voltaje_real, 2); // Muestra 2 decimales
    Serial.println(" V");

    // Control del relé
    if (voltaje_real > 3.5) {
        digitalWrite(pin_rele, LOW);  // Activar relé
    } else {
        digitalWrite(pin_rele, HIGH); // Desactivar relé
    }

    delay(500); // Espera de 0.5 segundos
}