from gpiozero import Button, Buzzer
import time

#Configuración de Hardware (gpiozero)
btn_1 = Button(17)
bzr_1 = Buzzer(27)

try:
    while True:
        if btn_1.is_pressed:
            bzr_1.on()
        else:
            bzr_1.off()
        time.sleep(0.1) 

except KeyboardInterrupt:
    bzr_1.off()