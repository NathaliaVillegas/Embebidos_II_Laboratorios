import serial
import RPi.GPIO as GPIO
import time

pinboton = 17

GPIO.setmode(GPIO.BCM)
GPIO.setup(pinboton, GPIO.IN, pull_up_down=GPIO.PUD_UP)

port = serial.Serial('/dev/ttyAMA0', 9600, timeout=1)

try:
	while True:
		if GPIO.input(pinboton) == GPIO.LOW:
			port.write(b"buzzer\n")
			time.sleep(0.5)
		time.sleep(0.01)
except KeyboardInterrupt:
	GPIO.cleanup()
	port.close()



