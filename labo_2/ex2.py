import RPi.GPIO as GPIO
import time

led0 = 26
led1 = 19
led2 = 13
led3 = 6
add = 5
res = 0

table = [
    [0,0,0,0],
    [1,0,0,0],
    [0,1,0,0],
    [1,1,0,0],
    [0,0,1,0],
    [1,0,1,0],
    [0,1,1,0],
    [1,1,1,0],
    [0,0,0,1],
    [1,0,0,1],
    [0,1,0,1],
    [1,1,0,1],
    [0,0,1,1],
    [1,0,1,1],
    [0,1,1,1],
    [1,1,1,1]
]

cont = 0

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)

GPIO.setup(led0, GPIO.OUT)
GPIO.setup(led1, GPIO.OUT)
GPIO.setup(led2, GPIO.OUT)
GPIO.setup(led3, GPIO.OUT)
    
GPIO.setup(add, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(res, GPIO.IN, pull_up_down=GPIO.PUD_UP)

def select(num):
    estado_actual = table[num]
    GPIO.output(led0, estado_actual[0])
    GPIO.output(led1, estado_actual[1])
    GPIO.output(led2, estado_actual[2])
    GPIO.output(led3, estado_actual[3])
    
def main():
    global cont
    while True:
        if GPIO.input(add) == GPIO.LOW:
            cont += 1
            if cont > 15:
                cont = 0
            select(cont)
            time.sleep(0.3)
        if GPIO.input(res) == GPIO.LOW:
            if cont > 0:
                cont -= 1
            select(cont)
            time.sleep(0.3)
        time.sleep(0.05)


try:
	select(cont)
	main()

except KeyboardInterrupt:
	GPIO.cleanup()
