import cv2
import numpy as np
import serial
import time

# ================= UART =================
# AJUSTA EL PUERTO
ser = serial.Serial('/dev/serial0', 9600, timeout=1)
time.sleep(2)

# ================= VIDEO / CAMARA =================
cap = cv2.VideoCapture(0)

# ================= BACKGROUND SUBTRACTORS =================
subtractor_mog2 = cv2.createBackgroundSubtractorMOG2(
    history=500,
    varThreshold=50,
    detectShadows=True
)

print("Iniciando control del robot... ESC para salir")

# último comando (evita spam UART)
last_command = ""

while cap.isOpened():

    ret, frame = cap.read()
    if not ret:
        break

    frame = cv2.resize(frame, (640, 480))

    height, width, _ = frame.shape

    # ================= MASK =================
    mask = subtractor_mog2.apply(frame)

    _, mask = cv2.threshold(mask, 200, 255, cv2.THRESH_BINARY)

    contours, _ = cv2.findContours(
        mask,
        cv2.RETR_EXTERNAL,
        cv2.CHAIN_APPROX_SIMPLE
    )

    command = "STOP"
    object_found = False

    for cnt in contours:

        area = cv2.contourArea(cnt)

        if area > 3000:   # filtro ruido

            x, y, w, h = cv2.boundingRect(cnt)

            cx = x + w // 2
            cy = y + h // 2

            # dibujar objeto
            cv2.rectangle(frame, (x,y), (x+w,y+h), (0,255,0), 2)
            cv2.circle(frame, (cx, cy), 5, (0,0,255), -1)

            # ================= DECISIÓN =================
            if cx < width // 3:
                command = "LEFT"

            elif cx > 2 * width // 3:
                command = "RIGHT"

            else:
                command = "FORWARD"

            object_found = True
            break

    if not object_found:
        command = "STOP"

    # ================= ENVIAR SOLO SI CAMBIA =================
    if command != last_command:
        ser.write((command + "\n").encode())
        last_command = command

    # ================= UI =================
    cv2.line(frame, (width//3, 0), (width//3, height), (255,0,0), 2)
    cv2.line(frame, (2*width//3, 0), (2*width//3, height), (255,0,0), 2)

    cv2.putText(frame, command, (20, 40),
                cv2.FONT_HERSHEY_SIMPLEX, 1,
                (0,255,0), 2)

    cv2.imshow("Robot Tracker", frame)
    cv2.imshow("Mask", mask)

    if cv2.waitKey(1) & 0xFF == 27:
        break

cap.release()
cv2.destroyAllWindows()
ser.close()