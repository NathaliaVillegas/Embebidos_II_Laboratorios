import cv2
from abc import ABC, abstractmethod

class video_Capture_abs(ABC):
    @abstractmethod
    def display_camera(self):
        pass

    @abstractmethod
    def stop_display(self):
        pass

    @abstractmethod
    def camera_visualization(self):
        pass

class video_Capture(video_Capture_abs):
    def __init__(self, camera) -> None:
        self.camera = camera 
        self.displayed = False
        self.modo_gris = False 

    def display_camera(self):
        self.displayed = True
        self.camera_visualization() 

    def stop_display(self):
        self.displayed = False 

    def camera_visualization(self):
        while self.displayed:
            check, frame = self.camera.read()
            if self.modo_gris:
                frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            
            cv2.imshow("Webcam", frame) 
            
            key = cv2.waitKey(1) 
            
            if key == ord('g'): # para Gris
                self.modo_gris = True
            elif key == ord('r'): # para RGB
                self.modo_gris = False
            elif key == 27: # 27 es 'Esc' [cite: 76]
                self.stop_display()

if __name__ == "__main__":
    camera = cv2.VideoCapture(0) 
    camera_object = video_Capture(camera) 
    camera_object.display_camera() 
    camera.release()
    cv2.destroyAllWindows()