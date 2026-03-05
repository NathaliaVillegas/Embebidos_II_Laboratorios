import RPi.GPIO as GPIO
import time

led1 = 26
led2 = 19
led3 = 13
led4 = 6
select = 5
seconds = 21

leds = [led1, led2, led3, led4]
selector = 0
time_adder = 0
controller = False

def cambiar_estado(channel):
	global selector, time_adder, controller
	time_adder = 0	
	controller = True
	selector += 1
	
	if selector > 3: 
    		selector = 0
	print("Presionaste selector")
    	

def cambiar_tiempo(channel):
	global time_adder
	time_adder += 1
	print("Presionaste time")
    

try:
	GPIO.setwarnings(False)
	GPIO.setmode(GPIO.BCM)

	GPIO.setup(led1, GPIO.OUT)
	GPIO.setup(led2, GPIO.OUT)
	GPIO.setup(led3, GPIO.OUT)
	GPIO.setup(led4, GPIO.OUT)
	GPIO.setup(select, GPIO.IN, pull_up_down=GPIO.PUD_UP)
	GPIO.setup(seconds, GPIO.IN, pull_up_down=GPIO.PUD_UP)
	GPIO.add_event_detect(select, GPIO.FALLING, callback=cambiar_estado, bouncetime=300) 
	GPIO.add_event_detect(seconds, GPIO.FALLING, callback=cambiar_tiempo, bouncetime=300) 

	while True:
		GPIO.output(led1, GPIO.LOW)
		GPIO.output(led2, GPIO.LOW)
		GPIO.output(led3, GPIO.LOW)
		GPIO.output(led4, GPIO.LOW)

		if controller == True:
			GPIO.output(leds[selector], GPIO.HIGH)
			time.sleep(1 + time_adder)
			controller = False
			print("Entraste a High")

except KeyboardInterrupt:
	GPIO.cleanup()
	
	
'''
^Camber@raspberrypi:~/Documentos $ python3 labo_2/ex4.py 
Presionaste selector
Presionaste time
Entraste a High
Presionaste selector
Presionaste time
Entraste a High
Presionaste time
Presionaste selector
Presionaste time
Entraste a High
Presionaste time
Presionaste selector
Presionaste time
Entraste a High
Presionaste time
Presionaste selector
Presionaste time
Entraste a High
Presionaste selector
Presionaste time
Entraste a High
Presionaste time
Presionaste selector
Presionaste time
Entraste a High
Presionaste time
Presionaste selector
Presionaste time
Entraste a High
Presionaste time
Presionaste selector
Entraste a High
Presionaste time
^Camber@raspberrypi:~/Documentos $ 
'''
