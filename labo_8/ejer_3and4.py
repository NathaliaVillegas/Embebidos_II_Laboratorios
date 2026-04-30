import cv2
import os
import numpy as np
from abc import ABC, abstractmethod

# Clase base abstracta siguiendo el estándar del laboratorio
class CaptureSystem_abs(ABC):
    @abstractmethod
    def start_webcam(self):
        pass
    
    @abstractmethod
    def post_process(self):
        pass

class ImageHandler(CaptureSystem_abs):
    def __init__(self):
        self.cap = cv2.VideoCapture(0)
        self.folder = "Captures"
        self.file_name = "image1.jpg"
        
        # Crear carpeta si no existe
        if not os.path.exists(self.folder):
            os.makedirs(self.folder)

    def start_webcam(self):
        print("Cámara iniciada. Presiona 'f' para capturar y 'Esc' para procesar y salir.")
        
        while self.cap.isOpened():
            ret, frame = self.cap.read()
            if not ret: break

            cv2.imshow("Webcam Live", frame)
            
            key = cv2.waitKey(1) & 0xFF
            
            # Ejercicio 4: Captura con tecla 'f'
            if key == ord('f'):
                path = os.path.join(self.folder, self.file_name)
                cv2.imwrite(path, frame)
                print(f"¡Imagen guardada en {path}!")
                
            elif key == 27: # Esc para terminar
                break
        
        self.cap.release()
        cv2.destroyAllWindows()
        self.post_process()

    def post_process(self):
        path = os.path.join(self.folder, self.file_name)
        
        # Verificar si la imagen existe antes de procesar
        if not os.path.exists(path):
            print("No se realizó ninguna captura.")
            return

        # Cargar y aplicar filtro de grises
        img = cv2.imread(path)
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        
        # Obtener dimensiones para los cuadrantes
        h, w = gray.shape
        mid_h, mid_w = h // 2, w // 2

        # División en cuadrantes mediante slicing de matrices
        q1 = gray[0:mid_h, 0:mid_w]
        q2 = gray[0:mid_h, mid_w:w]
        q3 = gray[mid_h:h, 0:mid_w]
        q4 = gray[mid_h:h, mid_w:w]

        # Mostrar resultados
        cv2.imshow("Cuadrante 1 - Superior Izq", q1)
        cv2.imshow("Cuadrante 2 - Superior Der", q2)
        cv2.imshow("Cuadrante 3 - Inferior Izq", q3)
        cv2.imshow("Cuadrante 4 - Inferior Der", q4)
        
        print("Cuadrantes generados. Presiona cualquier tecla para cerrar.")
        cv2.waitKey(0)
        cv2.destroyAllWindows()

if __name__ == "__main__":
    app = ImageHandler()
    app.start_webcam()