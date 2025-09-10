from machine import Pin
import time

# Configuración de pines
led_left = Pin(4, Pin.OUT)
led_centro = Pin(16, Pin.OUT)
led_right = Pin(17, Pin.OUT)
boton_right = Pin(19, Pin.IN, Pin.PULL_UP)
boton_left = Pin(18, Pin.IN, Pin.PULL_UP)

# Variables de control
ctrl_right = 0
ctrl_left = 0

# Función para apagar todos los LEDs
def apagar_leds():
    led_right.off()
    led_left.off()
    led_centro.off()

# Inicializar LEDs apagados
apagar_leds()

print("Controlador de motor iniciado...")

while True:
    # Leer estado de los botones (0 = presionado, 1 = no presionado)
    estado_right = boton_right.value()
    estado_left = boton_left.value()
    
    # Giro a la derecha (mantener presionado botón derecho)
    if estado_right == 0 and estado_left == 1:  # Solo botón derecho presionado
        led_right.on()
        led_left.off()
        led_centro.off()
        ctrl_right = 1
        ctrl_left = 0
        
    # Giro a la izquierda (mantener presionado botón izquierdo)
    elif estado_right == 1 and estado_left == 0:  # Solo botón izquierdo presionado
        led_left.on()
        led_right.off()
        led_centro.off()
        ctrl_left = 1
        ctrl_right = 0
        
    # Corrección automática cuando se sueltan los botones
    elif estado_right == 1 and estado_left == 1:  # Ningún botón presionado
        
        # Si estaba girando a la izquierda, corregir hacia el centro
        if ctrl_left == 1:
            print("Corrigiendo giro izquierdo...")
            for i in range(4):  # 4 parpadeos de corrección
                led_right.on()
                led_centro.on()
                led_left.off()
                time.sleep(0.2)
                apagar_leds()
                time.sleep(0.1)
                ctrl_left = 0
            
        # Si estaba girando a la derecha, corregir hacia el centro
        elif ctrl_right == 1:
            print("Corrigiendo giro derecho...")
            for i in range(4):  # 4 parpadeos de corrección
                led_left.on()
                led_centro.on()
                led_right.off()
                time.sleep(0.2)
                apagar_leds()
                time.sleep(0.1)
                ctrl_right = 0
    
    # Si ambos botones están presionados, mantener posición central
    elif estado_right == 0 and estado_left == 0:
        led_centro.on()
        led_right.off()
        led_left.off()
        ctrl_right = 0
        ctrl_left = 0
    
    # Pequeña pausa para evitar lecturas excesivas
    time.sleep(0.05)