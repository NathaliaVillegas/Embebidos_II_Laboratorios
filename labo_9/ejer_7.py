import cv2
import numpy as np
import serial
import time

# ================= UART =================
ser = serial.Serial('/dev/serial0', 9600, timeout=1)

time.sleep(2)

# ================= CAMARA =================
cap = cv2.VideoCapture(0)

# ================= BACKGROUND SUBTRACTOR =================
subtractor = cv2.createBackgroundSubtractorMOG2(
    history=500,
    varThreshold=50,
    detectShadows=True
)

last_command = ""

print("Robot Tracker iniciado")

while cap.isOpened():

    ret, frame = cap.read()

    if not ret:
        break

    frame = cv2.resize(frame, (640, 480))

    height, width, _ = frame.shape

    # =====================================================
    # MASCARA
    # =====================================================

    mask = subtractor.apply(frame)

    _, mask = cv2.threshold(
        mask,
        200,
        255,
        cv2.THRESH_BINARY
    )

    contours, _ = cv2.findContours(
        mask,
        cv2.RETR_EXTERNAL,
        cv2.CHAIN_APPROX_SIMPLE
    )

    valid_objects = []

    # =====================================================
    # FILTRAR OBJETOS VALIDOS
    # =====================================================

    for cnt in contours:

        area = cv2.contourArea(cnt)

        if area > 3000:

            valid_objects.append(cnt)

    # =====================================================
    # DECISIONES
    # =====================================================

    command = "STOP"

    # -----------------------------------------------------
    # 0 OBJETOS
    # -----------------------------------------------------

    if len(valid_objects) == 0:

        command = "SEARCH"

    # -----------------------------------------------------
    # 1 OBJETO
    # -----------------------------------------------------

    elif len(valid_objects) == 1:

        cnt = valid_objects[0]

        x, y, w, h = cv2.boundingRect(cnt)

        cx = x + w // 2
        cy = y + h // 2

        cv2.rectangle(
            frame,
            (x,y),
            (x+w,y+h),
            (0,255,0),
            2
        )

        cv2.circle(frame, (cx, cy), 5, (0,0,255), -1)

        if cx < width // 3:
            command = "LEFT"

        elif cx > 2 * width // 3:
            command = "RIGHT"

        else:
            command = "FORWARD"

    # -----------------------------------------------------
    # 2 O MÁS OBJETOS
    # -----------------------------------------------------

    else:

        command = "LEDS"

        for cnt in valid_objects:

            x, y, w, h = cv2.boundingRect(cnt)

            cv2.rectangle(
                frame,
                (x,y),
                (x+w,y+h),
                (255,0,0),
                2
            )

    # =====================================================
    # ENVIAR UART
    # =====================================================

    if command != last_command:

        ser.write((command + "\n").encode())

        last_command = command

    # =====================================================
    # UI
    # =====================================================

    cv2.line(frame,
             (width//3,0),
             (width//3,height),
             (255,0,0),
             2)

    cv2.line(frame,
             (2*width//3,0),
             (2*width//3,height),
             (255,0,0),
             2)

    cv2.putText(
        frame,
        command,
        (20,40),
        cv2.FONT_HERSHEY_SIMPLEX,
        1,
        (0,255,0),
        2
    )

    cv2.putText(
        frame,
        f"Objects: {len(valid_objects)}",
        (20,80),
        cv2.FONT_HERSHEY_SIMPLEX,
        1,
        (0,255,255),
        2
    )

    cv2.imshow("Robot Tracker", frame)

    cv2.imshow("Mask", mask)

    if cv2.waitKey(1) & 0xFF == 27:
        break

cap.release()

cv2.destroyAllWindows()

ser.close()