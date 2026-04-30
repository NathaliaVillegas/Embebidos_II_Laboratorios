import cv2
import numpy as np
from abc import ABC, abstractmethod

class ContourApp_abs(ABC):
    @abstractmethod
    def start_detection(self):
        pass

class RealTimeContours(ContourApp_abs):
    def __init__(self):
        self.cap = cv2.VideoCapture(0) 
        self.active = True

    def start_detection(self):
        print("Iniciando detección de contornos en tiempo real")
        print("Presiona 'Esc' para salir.")

        while self.active:
            ret, frame = self.cap.read() #
            if not ret: break

            frame = cv2.resize(frame, (400, 600))
            
            gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            
            blurred = cv2.GaussianBlur(gray, (5, 5), 0)
            
            _, thresh = cv2.threshold(blurred, 100, 255, cv2.THRESH_BINARY)

            contours, _ = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

            cv2.drawContours(frame, contours, -1, (0, 255, 0), 2)

            cv2.imshow("Deteccion en Tiempo Real - Julian", frame)
            cv2.imshow("Mascara Binaria", thresh)

            if cv2.waitKey(1) == 27: # Esc
                self.active = False

        self.cap.release()
        cv2.destroyAllWindows()

if __name__ == "__main__":
    app = RealTimeContours()
    app.start_detection()