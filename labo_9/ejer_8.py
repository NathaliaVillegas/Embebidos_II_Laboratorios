import cv2
import numpy as np

def clasificar_figura(vertices):
    if vertices == 3:
        return "Triangulo"
    elif vertices == 4:
        return "Cuadrilatero"
    elif vertices == 5:
        return "Pentagono"
    elif vertices > 5:
        return "Circulo"
    return "Desconocido"

def detectar_color(hsv_val):
    h = hsv_val[0]
    s = hsv_val[1]
    v = hsv_val[2]
    
    if s < 50 or v < 50:
        if v < 50: return "Negro"
        if v > 200 and s < 50: return "Blanco"
        return "Gris"
    
    if h < 10 or h > 165:
        return "Rojo"
    elif 10 <= h <= 35:
        return "Amarillo"
    elif 35 < h <= 85:
        return "Verde"
    elif 85 < h <= 130:
        return "Azul"
    elif 130 < h <= 165:
        return "Morado"
    return "Desconocido"

def main():
    img = cv2.imread('figuras.png')
    if img is None:
        print("Error: No se encontro la imagen.")
        return

    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    gris = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    
    blur = cv2.GaussianBlur(gris, (1, 1), 0)
    bordes = cv2.Canny(blur, 65, 14)
    
    contornos, _ = cv2.findContours(bordes, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    
    for cnt in contornos:
        area = cv2.contourArea(cnt)
        if area > 500:
            perimetro = cv2.arcLength(cnt, True)
            approx = cv2.approxPolyDP(cnt, 0.005 * perimetro, True)
            
            figura = clasificar_figura(len(approx))
            
            M = cv2.moments(cnt)
            if M["m00"] != 0:
                cx = int(M["m10"] / M["m00"])
                cy = int(M["m01"] / M["m00"])
                
                color_hsv = hsv[cy, cx]
                nombre_color = detectar_color(color_hsv)
                
                cv2.drawContours(img, [approx], -1, (0, 0, 0), 3)
                texto = f"{figura} {nombre_color}"
                cv2.putText(img, texto, (cx - 40, cy), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 2)
    
    cv2.imshow('Deteccion de Geometria y Color', img)
    while True:
        if cv2.waitKey(0) & 0xFF == 27:
            break
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()