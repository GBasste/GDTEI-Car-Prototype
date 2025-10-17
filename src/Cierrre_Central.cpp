#include <Arduino.h>

//PINES
const int input = 21;
const int output1 = 22; //NPN
const int output2 = 23; //PNP

//VARIABLES DE CONTROL
int change = 0;

void CCPINCONFIG(){
    pinMode(input, INPUT);
    pinMode(output1, OUTPUT);
    pinMode(output2, OUTPUT);
}

void CCEXE(){
    digitalRead(input);
    //Direccion 1
    if(input == 1 && change == 0){
        digitalWrite(output1, HIGH); //NPN
        digitalWrite(output2, HIGH); //PNP
        delay(5000);  
        change = 1;
        digitalWrite(output1, LOW); //NPN
        digitalWrite(output2, HIGH); //PNP
    }
    //Direccion 2
    if(input == 1 && change == 1){
        digitalWrite(output1, LOW);  //NPN
        digitalWrite(output2, LOW); //PNP
        delay(5000);  
        change = 0;
        digitalWrite(output1, LOW); //NPN
        digitalWrite(output2, HIGH); //PNP
    }
}

