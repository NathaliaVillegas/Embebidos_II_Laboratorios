import cv2
#import RPi.GPIO as GPIO

BUZZER_PIN = 23
#GPIO.setmode(GPIO.BCM)
#GPIO.setup(BUZZER_PIN, GPIO.OUT)

def main():
    cap = cv2.VideoCapture(0)
    fgbg = cv2.createBackgroundSubtractorMOG2(history=500, varThreshold=50, detectShadows=True)

    while True:
        ret, frame = cap.read()
        if not ret: break

        fgmask = fgbg.apply(frame)

        kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3, 3))
        fgmask = cv2.morphologyEx(fgmask, cv2.MORPH_OPEN, kernel)

        movimiento = cv2.countNonZero(fgmask)

        #if movimiento > 5000: 
            #print("¡MOVIMIENTO DETECTADO! Activando Alarma...")
            #GPIO.output(BUZZER_PIN, GPIO.HIGH)
        #else:
            #GPIO.output(BUZZER_PIN, GPIO.LOW)

        cv2.imshow('Frame Original', frame)
        cv2.imshow('Mascara de Movimiento', fgmask)

        if cv2.waitKey(30) == 27: break

    cap.release()
    #GPIO.cleanup()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()