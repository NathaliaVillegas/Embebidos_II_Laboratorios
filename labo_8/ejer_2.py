import cv2
import numpy as np
from abc import ABC, abstractmethod

# Clase Abstracta según el estándar del laboratorio[cite: 1, 2]
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
        self.filter_mode = 'o' 
        
        self.kernel_ridge = np.array([[-1, -1, -1], 
                                      [-1,  8, -1], 
                                      [-1, -1, -1]])
        
        self.kernel_sharpen = np.array([[ 0, -1,  0], 
                                        [-1,  5, -1], 
                                        [ 0, -1,  0]])
        
        self.kernel_blur = np.array([[1, 1, 1], 
                                     [1, 1, 1], 
                                     [1, 1, 1]]) / 9.0

    def apply_filter(self, frame):
        frame_small = cv2.resize(frame, (400, 300))
        
        if self.filter_mode == '1':
            return cv2.filter2D(frame_small, -1, self.kernel_ridge)
            
        elif self.filter_mode == '2':
            return cv2.filter2D(frame_small, -1, self.kernel_sharpen)
            
        elif self.filter_mode == '3':
            return cv2.filter2D(frame_small, -1, self.kernel_blur)
            
        return frame_small 

    def run(self):
        print("--- Sistema de Filtros por Kernels (Lab 8) ---")
        print("Presiona: '1' Ridge, '2' Sharpen, '3' Blur, 'o' Original, 'Esc' Salir")
        
        while self.cap.isOpened():
            ret, frame = self.cap.read()
            if not ret: break

            processed_frame = self.apply_filter(frame)
            
            cv2.imshow("Filtros Kernels - Julian", processed_frame)
            
            key = cv2.waitKey(1) & 0xFF
            
            if key == ord('1'): self.filter_mode = '1'
            elif key == ord('2'): self.filter_mode = '2'
            elif key == ord('3'): self.filter_mode = '3'
            elif key == ord('o'): self.filter_mode = 'o'
            elif key == 27: # Esc
                break
                
        self.cap.release()
        cv2.destroyAllWindows()

if __name__ == "__main__":
    app = FilterSystem()
    app.run()