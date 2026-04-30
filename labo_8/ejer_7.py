import cv2
import numpy as np
import RPi.GPIO as GPIO
import time

MOTOR_PIN = 18
GPIO.setmode(GPIO.BCM)
GPIO.setup(MOTOR_PIN, GPIO.OUT)
pwm = GPIO.PWM(MOTOR_PIN, 100)
pwm.start(0)

def main():
    cap = cv2.VideoCapture(0)
    

    rojo = ([0, 150, 50], [10, 255, 255])
    amarillo = ([20, 100, 100], [35, 255, 255])
    verde = ([40, 100, 100], [80, 255, 255])

    ultimo_estado = ""
    contador_confirmacion = 0

    print("Sistema iniciado. Analizando semáforo de la TIVA...")

    while True:
        ret, frame = cap.read()
        if not ret: break

        frame = cv2.resize(frame, (400, 300))
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

        m_r = cv2.inRange(hsv, np.array(rojo[0]), np.array(rojo[1]))
        m_a = cv2.inRange(hsv, np.array(amarillo[0]), np.array(amarillo[1]))
        m_v = cv2.inRange(hsv, np.array(verde[0]), np.array(verde[1]))

        conteos = {"ROJO": cv2.countNonZero(m_r), 
                   "AMARILLO": cv2.countNonZero(m_a), 
                   "VERDE": cv2.countNonZero(m_v)}
        
        estado_actual = max(conteos, key=conteos.get)
        
        if conteos[estado_actual] < 1000:
            estado_actual = "APAGADO"

        if estado_actual == ultimo_estado:
            contador_confirmacion += 1
        else:
            contador_confirmacion = 0
            ultimo_estado = estado_actual

        if contador_confirmacion == 5:
            if estado_actual == "VERDE":
                pwm.ChangeDutyCycle(100)
            elif estado_actual == "AMARILLO":
                pwm.ChangeDutyCycle(25)  
            elif estado_actual == "ROJO":
                pwm.ChangeDutyCycle(0) 
            
            print(f"Estado Semáforo Detectado: {estado_actual}")

        cv2.imshow("Monitor de Semáforo", frame)
        if cv2.waitKey(1) == 27: break

    cap.release()
    pwm.stop()
    GPIO.cleanup()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()