import serial
import RPi.GPIO as GPIO
import time
import threading

MOTOR1_PWM = 23
MOTOR2_PWM = 24
BOTON_PIN = 25

GPIO.setmode(GPIO.BCM)

GPIO.setup(MOTOR1_PWM, GPIO.OUT, initial=GPIO.LOW)
GPIO.setup(MOTOR2_PWM, GPIO.OUT, initial=GPIO.LOW)

GPIO.setup(BOTON_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)

# (Ejercicio 3: PWM base)
pwm_m1 = GPIO.PWM(MOTOR1_PWM, 1000)
pwm_m2 = GPIO.PWM(MOTOR2_PWM, 1000)

pwm_m1.start(0)
pwm_m2.start(0)

puerto = serial.Serial('/dev/ttyAMA0', 9600, timeout=0.1)

def leer_uart():
    while True:
        try:
            if puerto.in_waiting > 0:
                mensaje = puerto.readline().decode('utf-8').rstrip()
                print(f"TIVA: {mensaje}")
                
                # (Ejercicio 2 y 3: Motores al 50%)
                if mensaje == "motor1":
                    pwm_m1.ChangeDutyCycle(50)
                    pwm_m2.ChangeDutyCycle(0)
                elif mensaje == "motor2":
                    pwm_m2.ChangeDutyCycle(50)
                    pwm_m1.ChangeDutyCycle(0)
                # (Ejercicio 7: Detener motores si choca)
                elif mensaje == "stop":
                    pwm_m1.ChangeDutyCycle(0)
                    pwm_m2.ChangeDutyCycle(0)
        except:
            pass

hilo_uart = threading.Thread(target=leer_uart)
hilo_uart.daemon = True
hilo_uart.start()

try:
    estado_anterior = GPIO.HIGH
    while True:
        estado_actual = GPIO.input(BOTON_PIN)
        # (Ejercicio 5: Botón envía "buzzer")
        if estado_actual == GPIO.LOW and estado_anterior == GPIO.HIGH:
            puerto.write(b"buzzer\n")
            time.sleep(0.1)
        estado_anterior = estado_actual
        time.sleep(0.01)

except KeyboardInterrupt:
    pwm_m1.stop()
    pwm_m2.stop()
    GPIO.cleanup()
    puerto.close()