#include <Arduino.h>

// Declaración de funciones
void configurarSistema();
void ejecutarSistema();

void setup() {
  Serial.begin(115200);
  configurarSistema();
}

void loop() {
  ejecutarSistema();
}