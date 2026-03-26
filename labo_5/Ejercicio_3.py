import RPi.GPIO as GPIO
import time

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)

pin_pwm = 18
pin_adelante = 23
pin_atras = 24

GPIO.setup(pin_adelante, GPIO.OUT)
GPIO.setup(pin_atras, GPIO.OUT)
GPIO.setup(pin_pwm, GPIO.OUT)
pwm = GPIO.PWM(pin_pwm, 1000)
pwm.start(0)

try: 
	GPIO.output(pin_adelante, GPIO.HIGH)
	GPIO.output(pin_atras, GPIO.LOW)
	while True:
		print("DutyCycle: 75")
		pwm.ChangeDutyCycle(75)
		time.sleep(5)
		print("DutyCycle: 50")
		pwm.ChangeDutyCycle(50)
		time.sleep(5)
		print("DutyCycle: 45")
		pwm.ChangeDutyCycle(45)
		time.sleep(5)
		print("DutyCycle: 25")
		pwm.ChangeDutyCycle(25)
		time.sleep(5)
		
except KeyboardInterrupt:
	pwm.stop()
	GPIO.cleanup()
