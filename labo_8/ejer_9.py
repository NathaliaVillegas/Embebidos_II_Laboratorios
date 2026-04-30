import cv2
import numpy as np
import serial
import time

class DetectorObjetos:
    def __init__(self, port='/dev/ttyS0', baud=115200):
        try:
            self.ser = serial.Serial(port, baudrate=baud, timeout=1)
            print(f"UART conectada en {port}")
        except Exception as e:
            print(f"Error al conectar UART: {e}")
            self.ser = None

        self.cap = cv2.VideoCapture(0)
        self.fgbg = cv2.createBackgroundSubtractorMOG2(history=500, varThreshold=50, detectShadows=True)
        
        self.kernel = np.ones((5,5), np.uint8)

    def enviar_a_tiva(self, conteo):
        """Envía comandos a la Tiva basados en el conteo de objetos"""
        if self.ser and self.ser.is_open:
            if conteo == 1:
                self.ser.write(b'1') 
            elif conteo > 1:
                self.ser.write(b'M') 
            else:
                self.ser.write(b'0') 

    def procesar(self):
        print("Iniciando detección. Presiona 'Esc' para salir.")
        
        while self.cap.isOpened():
            ret, frame = self.cap.read()
            if not ret: break

            frame = cv2.resize(frame, (400, 300))
            
            fgmask = self.fgbg.apply(frame)

            fgmask = cv2.morphologyEx(fgmask, cv2.MORPH_OPEN, self.kernel)

            contours, _ = cv2.findContours(fgmask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
            
            objetos_validos = 0
            for cnt in contours:
                if cv2.contourArea(cnt) > 2000:
                    objetos_validos += 1
                    cv2.drawContours(frame, [cnt], -1, (0, 255, 0), 2)
                    
                    x, y, w, h = cv2.boundingRect(cnt)
                    cv2.rectangle(frame, (x, y), (x+w, y+h), (255, 0, 0), 2)

            self.enviar_a_tiva(objetos_validos)

            texto = f"Objetos detectados: {objetos_validos}"
            cv2.putText(frame, texto, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)
            
            cv2.imshow('Deteccion de Objetos - Lab 8', frame)
            cv2.imshow('Mascara Binaria (Limpieza)', fgmask)

            if cv2.waitKey(1) & 0xFF == 27: # Esc para salir
                break

        self.cap.release()
        if self.ser: self.ser.close()
        cv2.destroyAllWindows()

if __name__ == "__main__":
    detector = DetectorObjetos()
    detector.procesar()