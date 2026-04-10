import serial
import RPi.GPIO as GPIO
import time
import sys
import select

BOTON_PIN = 25
GPIO.setmode(GPIO.BCM)
GPIO.setup(BOTON_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)

# Intentar abrir puerto
try:
    puerto = serial.Serial('/dev/ttyAMA0', 9600, timeout=0.01)
except:
    puerto = serial.Serial('/dev/ttyS0', 9600, timeout=0.01)

def boton_presionado(channel):
    puerto.write(b"buzzer\n")

GPIO.add_event_detect(BOTON_PIN, GPIO.FALLING, callback=boton_presionado, bouncetime=300)

print("Sistema Listo. Comandos: Numero(0-100), L30, R45, o 'stop'")

try:
    while True:
        # Escuchar Telemetría
        if puerto.in_waiting > 0:
            mensaje = puerto.readline().decode('utf-8', errors='ignore').strip()
            if mensaje: print(f"Tiva: {mensaje}")

        # Input de usuario
        if sys.stdin in select.select([sys.stdin], [], [], 0)[0]:
            entrada = sys.stdin.readline().strip().upper()
            
            if entrada == "STOP":
                print("Enviando Frenado Total...")
                puerto.write(b"V0\n")
            elif entrada.isdigit():
                v = max(0, min(100, int(entrada)))
                puerto.write(f"V{v}\n".encode())
                print(f"Velocidad seteada al {v}%")
            elif entrada.startswith(('L', 'R')):
                print(f"Orden de Giro: {entrada}")
                puerto.write(f"{entrada}\n".encode())

        time.sleep(0.01)

except KeyboardInterrupt:
    GPIO.cleanup()
    puerto.close()