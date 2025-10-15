#include <Arduino.h>

//PINES
const int input = 21;
const int output1 = 22;
const int output2 = 23;

//VARIABLES DE CONTROL
int change = 0;

void CCPINCONFIG(){
    pinMode(input, INPUT);
    pinMode(output1, OUTPUT);
    pinMode(output2, OUTPUT);
}

void CCEXE(){
    digitalRead(input);
    if(input == 1 && change == 0){
        digitalWrite(output1, HIGH);
        digitalWrite(output2, LOW); 
        change = 1;
        delay(3000);  
    }
    if(input == 1 && change == 1){
        digitalWrite(output1, LOW);
        digitalWrite(output2, HIGH); 
        change = 0;
        delay(3000);  
    }
}

