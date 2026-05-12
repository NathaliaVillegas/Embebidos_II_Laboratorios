import cv2
import numpy as np

def main():
    img = cv2.imread('monedas_2.jpg')
    if img is None:
        print("Error al cargar monedas_2.jpg")
        return

    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    blurred = cv2.GaussianBlur(gray, (11, 11), 0)

    edges = cv2.Canny(blurred, 30, 150)

    kernel = np.ones((3, 3), np.uint8)
    edges = cv2.morphologyEx(edges, cv2.MORPH_CLOSE, kernel)

    contours, _ = cv2.findContours(edges.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    conteo = 0
    for cnt in contours:
        if cv2.contourArea(cnt) > 1000:
            conteo += 1
           

    cv2.putText(img, f"Monedas detectadas: {conteo}", (50, 50), 
                cv2.FONT_HERSHEY_SIMPLEX, 1.5, (0, 0, 255), 3)

    print(f"Número de monedas encontradas: {conteo}")
    cv2.imshow('Conteo de Monedas', img)
    cv2.imshow('gray', edges)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()