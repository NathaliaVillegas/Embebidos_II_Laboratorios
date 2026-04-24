import cv2
import os
from abc import ABC, abstractmethod

class video_Capture_abs(ABC):
    @abstractmethod
    def display_camera(self): pass
    @abstractmethod
    def camera_visualization(self): pass

class video_Capture(video_Capture_abs):
    def __init__(self, camera) -> None:
        self.camera = camera
        self.displayed = False
        if not os.path.exists('Captures'): os.makedirs('Captures')

    def display_camera(self):
        self.displayed = True
        self.camera_visualization()

    def camera_visualization(self):
        print("Presiona 'c' para capturar, 'Esc' para salir y procesar.")
        while self.displayed:
            check, frame = self.camera.read()
            cv2.imshow("Camara en vivo", frame)
            
            key = cv2.waitKey(1)
            if key == ord('c'): # 
                cv2.imwrite("Captures/image1.jpg", frame)
                print("Imagen guardada en Captures/image1.jpg")
            elif key == 27:
                self.displayed = False

    def procesar_cuadrantes(self):
        # Al terminar el proceso, aplicamos filtro y dividimos 
        img = cv2.imread("Captures/image1.jpg")
        if img is not None:
            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY) 
            h, w = gray.shape
            m_h, m_w = h // 2, w // 2
            
            cv2.imshow("Quadrant 1", gray[0:m_h, 0:m_w])
            cv2.imshow("Quadrant 2", gray[0:m_h, m_w:w])
            cv2.imshow("Quadrant 3", gray[m_h:h, 0:m_w])
            cv2.imshow("Quadrant 4", gray[m_h:h, m_w:w])
            cv2.waitKey(0)

if __name__ == "__main__":
    camera = cv2.VideoCapture(0)
    obj = video_Capture(camera)
    obj.display_camera()
    obj.procesar_cuadrantes() 
    camera.release()
    cv2.destroyAllWindows()