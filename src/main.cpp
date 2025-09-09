#include <Arduino.h>

// Declaración de funciones
//Interruptor
void configurarSistema();
void ejecutarSistema();
//Cams
void CamConfig();
void Camexe();

void setup() {
  Serial.begin(115200);
  configurarSistema();
  CamConfig();
}

void loop() {
  ejecutarSistema();
  Camexe();
}