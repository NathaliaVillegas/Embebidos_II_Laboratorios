from gpiozero import LED
from time import sleep

led1 = LED(17)
led2 = LED(27)
led3 = LED(22)

stop = 1.0

while True:
    try:
        with open("time.txt","r") as tim:
            text = tim.read().strip()
            if text:
                stop = float(text)
    except Exception:
        pass
    led1.on()
    sleep(stop)
    led1.off()

    led2.on()
    sleep(stop)
    led2.off()

    led3.on()
    sleep(stop)
    led3.off()