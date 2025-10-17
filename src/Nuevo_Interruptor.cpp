#include <Arduino.h>

const int boton = 2;
const int huella = 4;
const int salida = 35;

void configurarSistema() {
  pinMode(boton, INPUT); // Botón de OFF (SIN pull-up, tienes divisor externo)
  pinMode(salida, OUTPUT);       // Conexión al relé activador
  pinMode(huella, INPUT); // Pulso de la huella

  Serial.begin(115200);
  // Esperar que Serial esté listo
  while (!Serial) {
    delay(10);
  }
  delay(1000);
}

void ejecutarSistema() {
  // Detectar botón presionado OFF
  static bool botonPresionadoAntes = false;
  bool botonPresionado = (digitalRead(boton) == LOW);

  // Solo mostrar mensaje cuando se presiona (no mientras se mantiene)
  if (botonPresionado && !botonPresionadoAntes) {
    Serial.println("Boton presionado - Apagando rele");
    digitalWrite(salida, HIGH);
  }
  botonPresionadoAntes = botonPresionado;

  // Detectar botón presionado ON
  static bool botonPresionadoAntesOn = false;
  bool botonPresionadoOn = (digitalRead(huella) == HIGH);

  // Solo mostrar mensaje cuando se presiona (no mientras se mantiene)
  if (botonPresionadoOn && !botonPresionadoAntesOn) {
    Serial.println("Boton presionado - Encendiendo rele");
    digitalWrite(salida, LOW);
  }
  botonPresionadoAntesOn = botonPresionadoOn;
  
  // Procesar comandos seriales si hay datos disponibles
  if(Serial.available() > 0) {
    Serial.println("Probador de rele");
    Serial.println("Activar rele 1/apagar 0: ");
    
    // Leer el comando enviado
    int activar = Serial.parseInt();
    Serial.print("Comando recibido: ");
    Serial.println(activar);
    
    if(activar == 1) {
      digitalWrite(salida, LOW);
      Serial.println("Rele ACTIVADO");
    } else if(activar == 0) {
      digitalWrite(salida, HIGH);
      Serial.println("Rele DESACTIVADO");
    } else {
      Serial.println("Comando no valido. Use 1 o 0");
    }
    
    Serial.println(""); // Línea en blanco para separar
    
    // Limpiar buffer serial
    while(Serial.available() > 0) {
      Serial.read();
    }
  }
  
  delay(50); // Delay para estabilidad
}