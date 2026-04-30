import cv2
import numpy as np
from abc import ABC, abstractmethod

class VideoProcessor_abs(ABC):
    @abstractmethod
    def process_video(self):
        pass

class VideoExercise(VideoProcessor_abs):
    def __init__(self, video_path):
        self.cap = cv2.VideoCapture(video_path)
        if not self.cap.isOpened():
            print("Error: No se pudo abrir el video.")

    def process_video(self):
        while self.cap.isOpened():
            ret, frame = self.cap.read()
            if not ret:
                break

            frame = cv2.resize(frame, (400, 600))
            h, w = frame.shape[:2]

            gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            edges = cv2.Canny(gray, 100, 200)

            mitad_izq = frame[:, :w//2]
            mitad_der = frame[:, w//2:]

            q1 = frame[0:h//2, 0:w//2]
            q2 = frame[0:h//2, w//2:w]
            q3 = frame[h//2:h, 0:w//2]
            q4 = frame[h//2:h, w//2:w]

            cv2.imshow("Video Original Resized", frame)
            cv2.imshow("Bordes (Edge Detector)", edges)
            
            mitades = np.hstack((mitad_izq, mitad_der))
            cv2.imshow("Mitades", mitades)

            # Mostramos cuadrantes
            cv2.imshow("Q1", q1)
            cv2.imshow("Q2", q2)
            cv2.imshow("Q3", q3)
            cv2.imshow("Q4", q4)

            if cv2.waitKey(25) & 0xFF == 27: 
                break

        self.cap.release()
        cv2.destroyAllWindows()

if __name__ == "__main__":
    path_video = "ejemplo2.mp4" 
    lab = VideoExercise(path_video)
    lab.process_video()