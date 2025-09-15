#include <Arduino.h>

const int in1 = 0;
const int in2 = 2;
const int rele1 = 4;
const int rele2 = 13;

void setup() {
  Serial.begin(115200);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(rele1, OUTPUT);
  pinMode(rele2, OUTPUT);
}

void cerrar_puertas() {
  Serial.println("cerrar puertas");
  digitalWrite(rele2, HIGH);
  delay(200);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  delay(200);
  digitalWrite(rele2, LOW);
}

void abrir_puertas() {
  Serial.println("abrir puertas");
  digitalWrite(rele2, HIGH);
  delay(200);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  delay(200);
  digitalWrite(rele2, LOW);
}

void apagar_motor() {
  digitalWrite(rele1, HIGH);
}

void encender_motor() {
  digitalWrite(rele1, LOW);
}

void apagar() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  abrir_puertas();
}

void loop() {
  // Tu código aquí
}