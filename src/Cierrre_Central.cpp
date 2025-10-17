const int ledPin2 = 3;
const int ledPin1 = 1;
const int BotonFisico=23;

bool lastButtonState = HIGH;
bool currentButtonState = HIGH;
bool ledState = false;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup() {
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  int reading = digitalRead(buttonPin);
  
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != currentButtonState) {
      currentButtonState = reading;
      
      if (currentButtonState == LOW) {
        ledState = !ledState;
        
        if (ledState) {
          digitalWrite(ledPin1, HIGH);
          digitalWrite(ledPin2, LOW);
          delay(2000);
          digitalWrite(ledPin1, LOW);
          digitalWrite(ledPin2, LOW);
        } else {
          digitalWrite(ledPin1, LOW);
          digitalWrite(ledPin2, HIGH);
          delay(2000);
          digitalWrite(ledPin1, LOW);
          digitalWrite(ledPin2, LOW);
        }
      }
    }
  }
  
  lastButtonState = reading;
} 
