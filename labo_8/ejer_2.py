import cv2
import numpy as np
from abc import ABC, abstractmethod

class CameraApp_abs(ABC):
    @abstractmethod
    def apply_filter(self, frame):
        pass
    
    @abstractmethod
    def run(self):
        pass

class FilterSystem(CameraApp_abs):
    def __init__(self):
        self.cap = cv2.VideoCapture(0)
        self.filter_mode = '1' 
        
    def apply_filter(self, frame):
        
        if self.filter_mode == '1':
            return cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            
        elif self.filter_mode == '2':
            gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            return cv2.Canny(gray, 50, 150)
            
        elif self.filter_mode == '3':
            gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            _, thresh = cv2.threshold(gray, 127, 255, cv2.THRESH_BINARY)
            return thresh
            
        return frame 

    def run(self):
        print("Filtros")
        print("Presiona: '1' Gris, '2' Bordes, '3' Binario, 'o' Original, 'Esc' Salir")
        
        while self.cap.isOpened():
            ret, frame = self.cap.read()
            if not ret: break

            processed_frame = self.apply_filter(frame)
            
            cv2.imshow("Camara con Filtros - Julian", processed_frame)
            
            key = cv2.waitKey(1) & 0xFF
            
            if key == ord('1'): self.filter_mode = '1'
            elif key == ord('2'): self.filter_mode = '2'
            elif key == ord('3'): self.filter_mode = '3'
            elif key == ord('o'): self.filter_mode = 'original'
            elif key == 27: # Esc
                break
                
        self.cap.release()
        cv2.destroyAllWindows()

if __name__ == "__main__":
    app = FilterSystem()
    app.run()