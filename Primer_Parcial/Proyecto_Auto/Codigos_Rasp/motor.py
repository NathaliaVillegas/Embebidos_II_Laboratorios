import RPi.GPIO as GPIO
import time
import serial

# ================= CONFIGURACIÓN GPIO =================
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)

# ===== MOTOR 1 =====
pin_pwm1 = 18      # ENA
pin_adelante1 = 23 # IN1
pin_atras1 = 24    # IN2

# ===== MOTOR 2 =====
pin_pwm2 = 19      # ENB
pin_adelante2 = 5  # IN3
pin_atras2 = 6     # IN4

# Configurar pines como salida
GPIO.setup(pin_pwm1, GPIO.OUT)
GPIO.setup(pin_adelante1, GPIO.OUT)
GPIO.setup(pin_atras1, GPIO.OUT)

GPIO.setup(pin_pwm2, GPIO.OUT)
GPIO.setup(pin_adelante2, GPIO.OUT)
GPIO.setup(pin_atras2, GPIO.OUT)

# ================= PWM =================
pwm1 = GPIO.PWM(pin_pwm1, 1000)
pwm2 = GPIO.PWM(pin_pwm2, 1000)

pwm1.start(0)
pwm2.start(0)

# ================= UART =================
ser = serial.Serial(
    port='/dev/ttyAMA0',   # UART que estás usando
    baudrate=9600,
    timeout=1
)

# ================= FUNCIONES =================

GPIO.output(pin_atras1, GPIO.LOW)
GPIO.output(pin_atras2, GPIO.LOW)


def motor1_on():
    print("Motor 1 ON")
    GPIO.output(pin_adelante1, GPIO.HIGH)
    GPIO.output(pin_adelante2, GPIO.LOW)
    pwm1.ChangeDutyCycle(30)
    pwm2.ChangeDutyCycle(0)

def motor2_on():
    print("Motor 2 ON")
    GPIO.output(pin_adelante2, GPIO.HIGH)
    GPIO.output(pin_adelante1, GPIO.LOW)
    pwm2.ChangeDutyCycle(30)
    pwm1.ChangeDutyCycle(0)

def stop_motors():
    print("Motores OFF")
    pwm1.ChangeDutyCycle(0)
    pwm2.ChangeDutyCycle(0)

# ================= LOOP PRINCIPAL =================

try:
    while True:
        if ser.in_waiting > 0:
            mensaje = ser.readline().decode('utf-8').strip()
            print("Recibido:", mensaje)

            if mensaje == "motor1":
                motor1_on()

            elif mensaje == "motor2":
                motor2_on()

            else:
                stop_motors()

        time.sleep(0.1)

except KeyboardInterrupt:
    print("Programa detenido")

    pwm1.stop()
    pwm2.stop()
    GPIO.cleanup()
    ser.close()
