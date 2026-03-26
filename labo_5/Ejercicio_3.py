import RPi.GPIO as GPIO
import time

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)

def configuracion_pines():
	pin = 18
	pin_adelante = 23
	pin_atras = 24

	pwm = GPIO.PWM(pin, 1000)
	GPIO.setup(pin_adelante, GPIO.OUT)
	GPIO.setup(pin_atras, GPIO.OUT)


try: 
	configuracion_pines();
	GPIO.output(pin_adelante, GPIO.HIGH)
	GPIO.output(pin_atras, GPIO.LOW)
	while True:
		pwm.ChangeDutyCycle(75)
		time.sleep(5)
		pwm.ChangeDutyCycle(50)
		time.sleep(5)
		pwm.ChangeDutyCycle(45)
		time.sleep(5)
		pwm.ChangeDutyCycle(20)
		time.sleep(5)
except KeyboardInterrupt:
	pwm.stop()
	GPIO.cleanup()
