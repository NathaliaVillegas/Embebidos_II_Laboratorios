import serial
import RPi.GPIO as GPIO
import time
import sys
import select

BOTON_PIN = 25

GPIO.setmode(GPIO.BCM)
GPIO.setup(BOTON_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)

# Ajuste de puerto (asegúrate que sea ttyAMA0 o ttyS0 según tu config)
puerto = serial.Serial('/dev/ttyAMA0', 9600, timeout=0.01)

def boton_presionado(channel):
    # Solo enviamos el comando, evitamos prints pesados aquí
    puerto.write(b"buzzer\n")

GPIO.add_event_detect(
    BOTON_PIN,
    GPIO.FALLING,
    callback=boton_presionado,
    bouncetime=300 # Anti-rebote aumentado
)

print("Raspberry Lista. Ingrese velocidad (0-100) o 'stop':")

try:
    while True:
        # ===== LEER UART =====
        if puerto.in_waiting > 0:
            mensaje = puerto.readline().decode('utf-8', errors='ignore').strip()
            if mensaje:
                print("TIVA:", mensaje)

        # ===== INPUT USUARIO =====
        if sys.stdin in select.select([sys.stdin], [], [], 0)[0]:
            entrada = sys.stdin.readline().strip()
            if entrada.isdigit():
                duty = int(entrada)
                duty = max(0, min(100, duty))
                print(f"Enviando Velocidad: {duty}%")
                puerto.write(f"V{duty}\n".encode())
            elif entrada == "stop":
                print("STOP")
                puerto.write(b"V0\n")

        time.sleep(0.01)

except KeyboardInterrupt:
    GPIO.cleanup()
    puerto.close()