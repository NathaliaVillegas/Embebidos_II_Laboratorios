import cv2
import numpy as np

def main():
    cap = cv2.VideoCapture('bouncing.mp4.mp4')
    
    fgbg = cv2.createBackgroundSubtractorMOG2(history=500, varThreshold=50, detectShadows=True)

    print("Procesando video... Presiona 'q' para salir.")

    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            break

        frame = cv2.resize(frame, (600, 400))
        
        fgmask = fgbg.apply(frame)

        fgmask = cv2.medianBlur(fgmask, 5) 
        
        contours, _ = cv2.findContours(fgmask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

        for cnt in contours:
            if cv2.contourArea(cnt) > 500:
                cv2.drawContours(frame, [cnt], -1, (0, 255, 0), 2)
                
                M = cv2.moments(cnt)
                if M["m00"] != 0:
                    cx = int(M["m10"] / M["m00"])
                    cy = int(M["m01"] / M["m00"])
                    cv2.circle(frame, (cx, cy), 5, (0, 0, 255), -1)

        cv2.imshow('Contornos en Movimiento - Julian', frame)
        cv2.imshow('Mascara de Movimiento', fgmask)

        if cv2.waitKey(1) & 0xFF == 27:
            break

    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()