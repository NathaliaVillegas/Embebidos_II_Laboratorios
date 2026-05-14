import cv2
import numpy as np

def main():
    cap = cv2.VideoCapture(0)
    fgbg = cv2.createBackgroundSubtractorMOG2(history=500, varThreshold=50, detectShadows=True)

    while True:
        ret, frame = cap.read()
        if not ret:
            break

        frame = cv2.resize(frame, (640, 480))
        h, w = frame.shape[:2]
        mitad_x = w // 2

        fgmask = fgbg.apply(frame)
        kernel = np.ones((5,5), np.uint8)
        fgmask = cv2.morphologyEx(fgmask, cv2.MORPH_OPEN, kernel)

        contours, _ = cv2.findContours(fgmask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

        if contours:
            max_cnt = max(contours, key=cv2.contourArea)
            
            if cv2.contourArea(max_cnt) > 2000:
                x, y, obj_w, obj_h = cv2.boundingRect(max_cnt)
                cx = x + (obj_w // 2)
                cv2.rectangle(frame, (x, y), (x + obj_w, y + obj_h), (0, 255, 0), 2)

                if abs(cx - mitad_x) < 20:
                    cv2.putText(frame, "Object Detected", (w - 250, h - 20), 
                                cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)

        cv2.line(frame, (mitad_x, 0), (mitad_x, h), (255, 255, 0), 1)
        cv2.imshow('Deteccion de paso - Julian', frame)
        cv2.imshow('Mascara MOG2', fgmask)

        if cv2.waitKey(1) & 0xFF == 27:
            break

    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()