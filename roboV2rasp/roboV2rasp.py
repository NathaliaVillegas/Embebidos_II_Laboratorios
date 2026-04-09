import serial
import RPi.GPIO as GPIO
import time
import threading

BOTON_PIN = 25

GPIO.setmode(GPIO.BCM)
GPIO.setup(BOTON_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)

puerto = serial.Serial('/dev/ttyAMA0', 9600, timeout=0.1)

def leer_uart():
    while True:
        try:
            if puerto.in_waiting > 0:
                mensaje = puerto.readline().decode('utf-8').rstrip()
                print(f"TIVA: {mensaje}")
                
                if mensaje == "motor1":
                    puerto.write(b"A35\n")
                elif mensaje == "motor2":
                    puerto.write(b"B35\n")
                elif mensaje == "stop":
                    puerto.write(b"S0\n")
        except:
            pass

def leer_boton():
    estado_anterior = GPIO.HIGH
    while True:
        estado_actual = GPIO.input(BOTON_PIN)
        if estado_actual == GPIO.LOW and estado_anterior == GPIO.HIGH:
            puerto.write(b"buzzer\n")
            time.sleep(0.1)
        estado_anterior = estado_actual
        time.sleep(0.01)

hilo_uart = threading.Thread(target=leer_uart)
hilo_uart.daemon = True
hilo_uart.start()

hilo_boton = threading.Thread(target=leer_boton)
hilo_boton.daemon = True
hilo_boton.start()

try:
    while True:
        time.sleep(1)

except KeyboardInterrupt:
    GPIO.cleanup()
    puerto.close()