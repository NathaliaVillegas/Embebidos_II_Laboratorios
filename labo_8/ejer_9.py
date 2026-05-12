import cv2
import numpy as np
import serial
import time

class DetectorObjetos:

    def __init__(self, port='/dev/serial0', baud=9600):

        try:
            self.ser = serial.Serial(port, baudrate=baud, timeout=1)

        except Exception as e:
            print(f"Error UART: {e}")
            self.ser = None

        self.cap = cv2.VideoCapture(0)

        if not self.cap.isOpened():
            print("Errorcito")
            exit()

        self.fgbg = cv2.createBackgroundSubtractorMOG2(
            history=500,
            varThreshold=50,
            detectShadows=False
        )

        self.kernel = np.ones((5,5), np.uint8)

        self.estado_anterior = -1

    def enviar_a_tiva(self, conteo):

        if conteo == 0:
            estado = '0'

        elif conteo == 1:
            estado = '1'

        else:
            estado = '2'

        if estado != self.estado_anterior:

            print(f"Envio: {estado}")

            if self.ser and self.ser.is_open:
                self.ser.write(estado.encode())

            self.estado_anterior = estado


    def procesar(self):

        print("ESC para salir")

        while True:

            ret, frame = self.cap.read()

            if not ret:
                break

            frame = cv2.resize(frame, (640, 480))

            fgmask = self.fgbg.apply(frame)

            fgmask = cv2.morphologyEx(
                fgmask,
                cv2.MORPH_OPEN,
                self.kernel
            )

            fgmask = cv2.dilate(fgmask, self.kernel, iterations=2)

            contours, _ = cv2.findContours(
                fgmask,
                cv2.RETR_EXTERNAL,
                cv2.CHAIN_APPROX_SIMPLE
            )

            objetos_validos = 0

            for cnt in contours:

                area = cv2.contourArea(cnt)

                if area > 2000:

                    objetos_validos += 1

                    x, y, w, h = cv2.boundingRect(cnt)

                    cv2.rectangle(
                        frame,
                        (x, y),
                        (x+w, y+h),
                        (0,255,0),
                        2
                    )

                    cv2.drawContours(
                        frame,
                        [cnt],
                        -1,
                        (255,0,0),
                        2
                    )

            self.enviar_a_tiva(objetos_validos)

            cv2.putText(
                frame,
                f"Objetos: {objetos_validos}",
                (10,30),
                cv2.FONT_HERSHEY_SIMPLEX,
                1,
                (0,0,255),
                2
            )

            cv2.imshow("Deteccion Objetos", frame)
            cv2.imshow("Mascara", fgmask)

            if cv2.waitKey(1) & 0xFF == 27:
                break

        self.cap.release()

        if self.ser:
            self.ser.close()

        cv2.destroyAllWindows()


if __name__ == "__main__":

    detector = DetectorObjetos()

    detector.procesar()