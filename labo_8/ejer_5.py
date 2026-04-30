import cv2
import numpy as np

def main():
  
    img = cv2.imread("dragon.jpg")
    if img is None:
        print("Error: No se pudo cargar la imagen.")
        return

    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    _, thresh = cv2.threshold(gray, 127, 255, cv2.THRESH_BINARY)

    contours, hierarchy = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)


    img_copy = img.copy()
    cv2.drawContours(img_copy, contours, -1, (0, 255, 0), 2)

    print(f"Se detectaron {len(contours)} contornos.")
    cv2.imshow("Imagen Binaria (Threshold)", thresh)
    cv2.imshow("Contornos Detectados", img_copy)

    cv2.waitKey(0)
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()