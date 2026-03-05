import RPi.GPIO as GPIO
import time

LED1 = 14
LED2 = 15
BOTON = 18

estado_actual = 1

def cambiar_estado(channel):
    global estado_actual
    estado_actual += 1
    if estado_actual > 4:
        estado_actual = 1


def estado_1():
    GPIO.output(LED1, GPIO.HIGH)
    GPIO.output(LED2, GPIO.LOW)
    time.sleep(1)
    if estado_actual != 1: return 
    GPIO.output(LED1, GPIO.LOW)
    GPIO.output(LED2, GPIO.HIGH)
    time.sleep(1)

def estado_2():
    GPIO.output(LED1, GPIO.HIGH)
    GPIO.output(LED2, GPIO.HIGH)
    time.sleep(2)
    if estado_actual != 2: return
    GPIO.output(LED1, GPIO.LOW)
    GPIO.output(LED2, GPIO.LOW)
    time.sleep(2)

def estado_3():
    GPIO.output(LED1, GPIO.HIGH)
    GPIO.output(LED2, GPIO.HIGH)
    time.sleep(0.1) 

def estado_4():
    GPIO.output(LED1, GPIO.LOW)
    GPIO.output(LED2, GPIO.LOW)
    time.sleep(0.1)



configurar_pines()
try:
	GPIO.setwarnings(False)
	GPIO.setmode(GPIO.BCM)

	GPIO.setup(LED1, GPIO.OUT)
	GPIO.setup(LED2, GPIO.OUT)
	GPIO.setup(BOTON, GPIO.IN, pull_up_down=GPIO.PUD_UP)

	GPIO.add_event_detect(BOTON, GPIO.FALLING, callback=cambiar_estado, bouncetime=300) #Interrupción
	while True:
		if estado_actual == 1:
		    estado_1()
		elif estado_actual == 2:
		    estado_2()
		elif estado_actual == 3:
		    estado_3()
		elif estado_actual == 4:
		    estado_4()

except KeyboardInterrupt:
	GPIO.cleanup()
