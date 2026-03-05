import RPi.GPIO as GPIO
import time
import random

PIN_CALENTADOR = 26
PIN_VENTILADOR = 19

def configurar_sistema():
    GPIO.setwarnings(False)
    GPIO.setmode(GPIO.BCM)
    
    GPIO.setup(PIN_CALENTADOR, GPIO.OUT)
    GPIO.setup(PIN_VENTILADOR, GPIO.OUT)
    
    GPIO.output(PIN_CALENTADOR, GPIO.LOW)
    GPIO.output(PIN_VENTILADOR, GPIO.LOW)

def controlar_clima(temperatura):
    print(f"Temperatura: {temperatura}°C")

    if temperatura < 12:
        GPIO.output(PIN_CALENTADOR, GPIO.HIGH)
        GPIO.output(PIN_VENTILADOR, GPIO.LOW)
        
    elif temperatura > 20:
        GPIO.output(PIN_CALENTADOR, GPIO.LOW)
        GPIO.output(PIN_VENTILADOR, GPIO.HIGH)
        
    else:
        GPIO.output(PIN_CALENTADOR, GPIO.LOW)
        GPIO.output(PIN_VENTILADOR, GPIO.LOW)

def programa_principal():
    configurar_sistema()
    
    while True:
        temperatura_falsa = random.randint(5, 30)
        
        controlar_clima(temperatura_falsa)
        time.sleep(2.0)

if __name__ == '__main__':
    try:
        programa_principal()
    except KeyboardInterrupt:
        GPIO.cleanup()
