#define RXD2 16
#define TXD2 17

const int AD = 14;   // Adelante
const int AT = 25;   // Atrás
const int IZQ = 5;  // Izquierda
const int DER = 22;  // Derecha

void sendCmd(String cmd)
{
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

  sendCmd("AT+ADDRESS=1");
  sendCmd("AT+NETWORKID=5");
  sendCmd("AT+RX");

  pinMode(AD, INPUT_PULLUP);
  pinMode(AT, INPUT_PULLUP);
  pinMode(IZQ, INPUT_PULLUP);
  pinMode(DER, INPUT_PULLUP);
}

void enviarComando(String comando) {
  int len = comando.length();
  String mensaje = "AT+SEND=2," + String(len) + "," + comando;
  Serial.println("Enviando: " + comando);
  sendCmd(mensaje);
}

void loop() {
  bool ad = digitalRead(AD) == LOW;
  bool at = digitalRead(AT) == LOW;
  bool izq = digitalRead(IZQ) == LOW;
  bool der = digitalRead(DER) == LOW;

  String comando = "";

  if (ad && izq) comando = "adelante_izquierda";
  else if (ad && der) comando = "adelante_derecha";
  else if (at && izq) comando = "atras_izquierda";
  else if (at && der) comando = "atras_derecha";
  else if (ad) comando = "adelante";
  else if (at) comando = "atras";
  else if (izq) comando = "izquierda";
  else if (der) comando = "derecha";
  else comando = "stop";

  enviarComando(comando);
  delay(200);  // evita saturar la red LoRa
}
