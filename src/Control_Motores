from machine import Pin
import time
import math

# Pines
in1 = Pin(0, Pin.OUT)
in2 = Pin(2, Pin.OUT)
rele1=Pin(4,Pin.OUT)
rele2=Pin(13,Pin.OUT)

def cerrar_puertas():
 print("cerrar puertas")
 rele2.on()
 time.sleep_ms(200)
 in1.on()
 in2.off()
 time.sleep_ms(200)
 rele2.off()


def abrir_puertas():
 print("abrir puertas")
 rele2.on()
 time.sleep_ms(200)
 in1.off()
 in2.on()
 time.sleep_ms(200)
 rele2.off()
 
def apagar_motor():
  rele1.on()

def encender_motor():
   rele1.off()

def apagar():
    in1.off()
    in2.off()
  


abrir_puertas()