import cv2
import numpy as np
import RPi.GPIO as GPIO
import time
import threading

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

MOTOR_PIN = 18

GPIO.setup(MOTOR_PIN, GPIO.OUT)

pwm = GPIO.PWM(MOTOR_PIN, 100)
pwm.start(0)

frame = None
lock = threading.Lock()
running = True

def capture_thread():
    global frame, running

    cap = cv2.VideoCapture(0, cv2.CAP_V4L2)

    if not cap.isOpened():
        print("No se pudo abrir la cámara")
        running = False
        return

    print("Cámara iniciada correctamente")

    while running:
        ret, img = cap.read()
        if not ret:
            continue

        img = cv2.resize(img, (400, 300))

        with lock:
            frame = img.copy()

    cap.release()


def processing_thread():
    global frame, running

    rojo1 = ([0,150,150],[10,255,255])
    rojo2 = ([170,150,150],[180,255,255])
    amarillo = ([15,20,180],[40,120,255])
    verde = ([40,100,100],[80,255,255])

    estado_confirmado = ""
    ultimo_cambio = time.time()

    kernel = np.ones((5,5), np.uint8)

    while running:
        if frame is None:
            continue

        with lock:
            img = frame.copy()

        roi = img[50:250, 150:300]

        hsv = cv2.cvtColor(roi, cv2.COLOR_BGR2HSV)

        m_r1 = cv2.inRange(hsv, np.array(rojo1[0]), np.array(rojo1[1]))
        m_r2 = cv2.inRange(hsv, np.array(rojo2[0]), np.array(rojo2[1]))
        m_r = m_r1 + m_r2

        m_a = cv2.inRange(hsv, np.array(amarillo[0]), np.array(amarillo[1]))

        m_v = cv2.inRange(hsv, np.array(verde[0]), np.array(verde[1]))

        m_r = cv2.morphologyEx(m_r, cv2.MORPH_OPEN, kernel)
        m_a = cv2.morphologyEx(m_a, cv2.MORPH_OPEN, kernel)
        m_v = cv2.morphologyEx(m_v, cv2.MORPH_OPEN, kernel)

        conteos = {
            "ROJO": cv2.countNonZero(m_r),
            "AMARILLO": cv2.countNonZero(m_a),
            "VERDE": cv2.countNonZero(m_v)
        }

        estado_actual = max(conteos, key=conteos.get)

        area_total = roi.shape[0] * roi.shape[1]
        if conteos[estado_actual] < 0.01 * area_total:
            estado_actual = "APAGADO"

        if estado_actual != estado_confirmado:
            if time.time() - ultimo_cambio > 0.5:
                estado_confirmado = estado_actual
                ultimo_cambio = time.time()

                if estado_confirmado == "VERDE":
                    pwm.ChangeDutyCycle(100)
                    print("VERDE")

                elif estado_confirmado == "AMARILLO":
                    pwm.ChangeDutyCycle(25)
                    print("AMARILLO")

                elif estado_confirmado == "ROJO":
                    pwm.ChangeDutyCycle(0)
                    print("ROJO")

                else:
                    pwm.ChangeDutyCycle(0)
                    print("SIN DETECCIÓN")

        cv2.imshow("Frame", img)
        cv2.imshow("ROI", roi)
        cv2.imshow("Rojo", m_r)
        cv2.imshow("Amarillo", m_a)
        cv2.imshow("Verde", m_v)

        if cv2.waitKey(1) == 27:
            running = False

t1 = threading.Thread(target=capture_thread)
t2 = threading.Thread(target=processing_thread)

t1.start()
t2.start()

t1.join()
t2.join()

pwm.stop()
GPIO.cleanup()
cv2.destroyAllWindows()