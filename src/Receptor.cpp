#define RXD2 16
#define TXD2 17

// Pines conectados a la tarjeta de relé (para movimientos adelante/atrás)
const int RELAY_ADELANTEP = 18;
const int RELAY_ADELANTEN = 19;
const int RELAY_ATRASP    = 21;
const int RELAY_ATRASN   = 22;
const int RELAY_IZQP      = 33;
const int RELAY_IZQN      = 32;
const int RELAY_DERP      = 25;
const int RELAY_DERN      = 26;

// Pines para motor L298N (giros de 90°)
const int MOTOR_L298N_IN1 = 2;   // Control dirección motor L298N
const int MOTOR_L298N_IN2 = 15;   // Control dirección motor L298N  
const int MOTOR_L298N_ENA = 14;   // Control velocidad PWM motor L298N

// Variables para control de movimientos de 90°
unsigned long tiempoMovimiento = 1000; // Tiempo en ms para girar 90°
int velocidadMotor = 255; // Velocidad del motor L298N (0-255)
int posicionActual = 0; // Variable para tracking de posición: 0° = posición inicial, -90 = izquierda, 90 = derecha

void sendCmd(String cmd) {
  Serial2.println(cmd);
  delay(50);
  while (Serial2.available()) {
    Serial.print(char(Serial2.read()));
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  delay(30);

  sendCmd("AT+ADDRESS=2");
  sendCmd("AT+NETWORKID=5");
  sendCmd("AT+RX");

  pinMode(RELAY_ADELANTEP, OUTPUT);
  pinMode(RELAY_ADELANTEN, OUTPUT);
  pinMode(RELAY_ATRASP, OUTPUT);
  pinMode(RELAY_ATRASN, OUTPUT);
  pinMode(RELAY_IZQP, OUTPUT);
  pinMode(RELAY_IZQN, OUTPUT);
  pinMode(RELAY_DERP, OUTPUT);
  pinMode(RELAY_DERN, OUTPUT);

  // Configurar pines del motor L298N
  pinMode(MOTOR_L298N_IN1, OUTPUT);
  pinMode(MOTOR_L298N_IN2, OUTPUT);
  pinMode(MOTOR_L298N_ENA, OUTPUT);

  // Apagar todos los relés al inicio (HIGH = desactivado para relés)
  apagarTodo();
  // Detener motor L298N al inicio
  detenerMotorL298N();
  
  Serial.println("Sistema iniciado - Comandos disponibles:");
  Serial.println("- adelante, atras (relés)");
  Serial.println("- izquierda: gira 90° izquierda y mantiene posición");
  Serial.println("- derecha: gira 90° derecha y mantiene posición");
  Serial.println("- stop: vuelve a posición inicial");
}

void apagarTodo() {
  digitalWrite(RELAY_ADELANTEP, HIGH);
  digitalWrite(RELAY_ADELANTEN, HIGH);
  digitalWrite(RELAY_ATRASP, HIGH);
  digitalWrite(RELAY_ATRASN, HIGH);
  digitalWrite(RELAY_IZQP, HIGH);
  digitalWrite(RELAY_IZQN, HIGH);
  digitalWrite(RELAY_DERP, HIGH);
  digitalWrite(RELAY_DERN, HIGH);
}

// Funciones para control del motor L298N
void detenerMotorL298N() {
  digitalWrite(MOTOR_L298N_IN1, LOW);
  digitalWrite(MOTOR_L298N_IN2, LOW);
  analogWrite(MOTOR_L298N_ENA, 0);
}

void aceleracionPWM(int pin, int velocidadInicial, int velocidadFinal, int tiempoMs) {
  int pasos = abs(velocidadFinal - velocidadInicial);
  int direccion = (velocidadFinal > velocidadInicial) ? 1 : -1;
  int delayPaso = tiempoMs / pasos;
  
  for (int i = 0; i <= pasos; i++) {
    int velocidadActual = velocidadInicial + (i * direccion);
    analogWrite(pin, velocidadActual);
    delay(delayPaso);
  }
}

void girarMotorDerechaL298N() {
  digitalWrite(MOTOR_L298N_IN1, HIGH);
  digitalWrite(MOTOR_L298N_IN2, LOW);
  analogWrite(MOTOR_L298N_ENA, velocidadMotor);
}

void girarMotorIzquierdaL298N() {
  digitalWrite(MOTOR_L298N_IN1, LOW);
  digitalWrite(MOTOR_L298N_IN2, HIGH);
  analogWrite(MOTOR_L298N_ENA, velocidadMotor);
}

void activarReles(bool adp, bool adn, bool atp, bool atn, bool izqp, bool izqn, bool derp, bool dern) {
  digitalWrite(RELAY_ADELANTEP, adp ? LOW : HIGH);
  digitalWrite(RELAY_ADELANTEN, adn ? LOW : HIGH);
  digitalWrite(RELAY_ATRASP,    atp ? LOW : HIGH);
  digitalWrite(RELAY_ATRASN,    atn ? LOW : HIGH);
  digitalWrite(RELAY_IZQP,      izqp ? LOW: HIGH);
  digitalWrite(RELAY_IZQN,      izqn ? LOW : HIGH);
  digitalWrite(RELAY_DERP,      derp ? LOW: HIGH);
  digitalWrite(RELAY_DERN,      dern ? LOW : HIGH);
}

// Función para ir a derecha (mantiene posición)
void comandoDerecha() {
  if (posicionActual == 20) {
    Serial.println("Ya está en posición derecha");
    return;
  }
  
  Serial.println("Comando DERECHA: Girando 90° a la derecha...");
  
  if (posicionActual == 0) {
    // Desde centro a derecha
    girarMotorDerechaL298N();
    delay(tiempoMovimiento);
    detenerMotorL298N();
    posicionActual = 20;
  } else if (posicionActual == -20) {
    // Desde izquierda a derecha (180°)
    girarMotorDerechaL298N();
    delay(tiempoMovimiento * 2);
    detenerMotorL298N();
    posicionActual = 20;
  }
  
  Serial.println("En posición derecha - Manteniendo posición");
}

// Función para ir a izquierda (mantiene posición)
void comandoIzquierda() {
  if (posicionActual == -20) {
    Serial.println("Ya está en posición izquierda");
    return;
  }
  
  Serial.println("Comando IZQUIERDA: Girando 90° a la izquierda...");
  
  if (posicionActual == 0) {
    // Desde centro a izquierda
    girarMotorIzquierdaL298N();
    delay(tiempoMovimiento);
    detenerMotorL298N();
    posicionActual = -20;
  } else if (posicionActual == 20) {
    // Desde derecha a izquierda (180°)
    girarMotorIzquierdaL298N();
    delay(tiempoMovimiento * 2);
    detenerMotorL298N();
    posicionActual = -20;
  }
  
  Serial.println("En posición izquierda - Manteniendo posición");
}

// Función para volver al centro
void volverACentro() {
  if (posicionActual == 0) {
    Serial.println("Ya está en posición central");
    return;
  }
  
  Serial.println("Volviendo a posición central...");
  
  if (posicionActual == 20) {
    // Desde derecha al centro
    girarMotorIzquierdaL298N();
    delay(tiempoMovimiento/2.5);   //calibrar el tiempo de giro de las ruedas
    detenerMotorL298N();
  } else if (posicionActual == -20) {
    // Desde izquierda al centro
    girarMotorDerechaL298N();
    delay(tiempoMovimiento/2.3);
    detenerMotorL298N();
  }
  
  posicionActual = 0;
  Serial.println("En posición central");
}

void ejecutarComando(String cmd) {
  cmd.trim();  // Elimina espacios y saltos de línea

  if (cmd == "adelante") {
    aceleracionPWM(27,2,50,200);
    activarReles(true, true, false, false, false, false, false, false);
  } else if (cmd == "atras") {
    aceleracionPWM(27,2,50,200);
    activarReles(false, false, true, true, false, false, false, false);
  } else if (cmd == "izquierda") {
    // Nuevo comportamiento: gira 90° izquierda y mantiene posición
    comandoIzquierda();
    return; // No ejecutar apagarTodo() para este comando
  } else if (cmd == "derecha") {
    // Nuevo comportamiento: gira 90° derecha y mantiene posición
    comandoDerecha();
    return; // No ejecutar apagarTodo() para este comando
  } else if (cmd == "adelante_izquierda") {
    activarReles(true, true, false, false, true, true, false, false);
  } else if (cmd == "adelante_derecha") {
    activarReles(true, true, false, false, false, false, true, true);
  } else if (cmd == "atras_izquierda") {
    activarReles(false, false, true, true, true, true, false, false);
  } else if (cmd == "atras_derecha") {
    activarReles(false, false, true, true, false, false, true, true);
  } else {
    // comando "stop" u otro no reconocido
    apagarTodo();
    detenerMotorL298N();
    volverACentro(); // Volver a posición central cuando se suelta el botón
  }

  Serial.println("Ejecutado: " + cmd);
}

void loop() {
  if (Serial2.available()) {
    String mensaje = Serial2.readStringUntil('\n');
    Serial.println("Recibido: " + mensaje);

    if (mensaje.startsWith("+RCV=")) {
      // Formato: +RCV=1,8,adelante,-25,10
      int pos1 = mensaje.indexOf(',');                      // después de address
      int pos2 = mensaje.indexOf(',', pos1 + 1);            // después de length
      int pos3 = mensaje.indexOf(',', pos2 + 1);            // después del dato
      if (pos1 != -1 && pos2 != -1 && pos3 != -1) {
        String data = mensaje.substring(pos2 + 1, pos3);    // extrae el comando
        ejecutarComando(data);
      }
    }
  }
  
  // Control manual por Serial para pruebas (opcional)
  if (Serial.available()) {
    String comandoSerial = Serial.readStringUntil('\n');
    Serial.println("Comando manual: " + comandoSerial);
    ejecutarComando(comandoSerial);
  }
}
