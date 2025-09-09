#include <Arduino.h>

// Declaración de funciones
//Interruptor
void configurarSistema();
void ejecutarSistema();
//Cams
void CamConfig();
void Camexe();
//Motores
void motorset();
void cerrar_puertas();
void abrir_puertas();
void apagar_motor();
void encender_motor();
void apagar();

void setup() {
  Serial.begin(115200);
  configurarSistema();
  CamConfig();
  motorset();
}

void loop() {
  ejecutarSistema();
  Camexe();
  cerrar_puertas();
  abrir_puertas();
  apagar_motor();
  encender_motor();
  apagar();
}