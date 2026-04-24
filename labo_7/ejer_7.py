import cv2
import numpy as np
from abc import ABC, abstractmethod

class video_Capture_abs(ABC):
    @abstractmethod
    def camera_visualization(self):
        pass

class ColorDetector(video_Capture_abs):
    def __init__(self):
        # RANGOS AMPLIADOS para captar más tonalidades (Saturación y Valor desde 40)
        self.colores = {
            "Rojo": [
                (np.array([0, 70, 50]), np.array([15, 255, 255])),
                (np.array([165, 70, 50]), np.array([180, 255, 255]))
            ],
            "Verde": [(np.array([35, 40, 40]), np.array([90, 255, 255]))],
            "Azul": [(np.array([90, 40, 40]), np.array([130, 255, 255]))]
        }
        self.kernel = np.ones((5,5), np.uint8)

    def procesar_y_dibujar(self, frame, hsv, nombre, rangos, color_bgr):
        # Crear máscara
        mask = cv2.inRange(hsv, rangos[0][0], rangos[0][1])
        if len(rangos) > 1:
            mask2 = cv2.inRange(hsv, rangos[1][0], rangos[1][1])
            mask = cv2.bitwise_or(mask, mask2)
        
        # Limpieza de ruido (Morfología)
        mask = cv2.dilate(mask, self.kernel, iterations=1) # Engrosar para cámaras de baja resolución
        
        contornos, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        
        # Lógica de ÁREA MAYOR
        ar = []
        for cnt in contornos:
            ar.append(cv2.contourArea(cnt))
        
        if len(ar) > 0:
            maxi = max(ar)
            if maxi > 300: # Umbral más bajo por si la cámara es de baja resolución
                for cnt in contornos:
                    if cv2.contourArea(cnt) == maxi:
                        x, y, w, h = cv2.boundingRect(cnt)
                        cv2.rectangle(frame, (x, y), (x+w, y+h), color_bgr, 2)
                        cv2.putText(frame, f"{nombre}", (x, y-10), 
                                    cv2.FONT_HERSHEY_SIMPLEX, 0.6, color_bgr, 2)

    def camera_visualization(self):
        cap = cv2.VideoCapture(0)
        # Ajuste de resolución estándar para Raspberry
        cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
        cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

        while True:
            ret, frame = cap.read()
            if not ret: break

            hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

            # Llamadas a la función de procesamiento
            self.procesar_y_dibujar(frame, hsv, "Rojo", self.colores["Rojo"], (0, 0, 255))
            self.procesar_y_dibujar(frame, hsv, "Verde", self.colores["Verde"], (0, 255, 0))
            self.procesar_y_dibujar(frame, hsv, "Azul", self.colores["Azul"], (255, 0, 0))

            cv2.imshow("Webcam Pico - RGB Detector", frame)

            if cv2.waitKey(1) & 0xFF == 27:
                break

        cap.release()
        cv2.destroyAllWindows()

if __name__ == "__main__":
    detector = ColorDetector()
    detector.camera_visualization()