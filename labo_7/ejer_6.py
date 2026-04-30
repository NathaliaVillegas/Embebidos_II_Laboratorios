import cv2

class ImageColorManager:
    def __init__(self, path):
        self.original = cv2.imread(path)
        if self.original is None:
            print(f"Error: No se pudo encontrar la imagen en {path}")
            self.valid = False
        else:
            self.valid = True

    def show_rgb(self):
        if self.valid:
            cv2.imshow("Imagen Original (RGB/BGR)", self.original)

    def show_grayscale(self):
        if self.valid:
            gray = cv2.cvtColor(self.original, cv2.COLOR_BGR2GRAY)
            cv2.imshow("Imagen en Grayscale", gray)

    def show_hsv(self):
        if self.valid:
            hsv = cv2.cvtColor(self.original, cv2.COLOR_BGR2HSV)
            cv2.imshow("Imagen en HSV", hsv)

if __name__ == "__main__":
    ruta = "mario.jpg" 
    
    processor = ImageColorManager(ruta)
    
    if processor.valid:
        processor.show_rgb()
        processor.show_grayscale()
        processor.show_hsv()
        
        cv2.waitKey(0)
        cv2.destroyAllWindows()