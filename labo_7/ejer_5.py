import cv2
import os
import numpy as np

def convertir_a_gris(imagen_bgr):
    return cv2.cvtColor(imagen_bgr, cv2.COLOR_BGR2GRAY)

def main():
    imagenes = [f for f in os.listdir('colors') if f.endswith(('.png', '.jpg', '.jpeg'))]

    if len(imagenes) == 0:
        print("No hay imágenes en la carpeta 'colors'.")
        return

    for nombre in imagenes:
        ruta = os.path.join('colors', nombre)
        img = cv2.imread(ruta) 
        
        if img is not None:
            h, w = img.shape[:2]
            pixel_valor = img[h//2, w//2]
            
            print(f"\n>>> Imagen: {nombre}")
            print(f"Valor del píxel central (BGR): {pixel_valor}")
            
            img_gris = convertir_a_gris(img)
            
            cv2.imshow(f"Original: {nombre}", img)
            cv2.imshow(f"Escala de Grises: {nombre}", img_gris)
            
            print("Presiona cualquier tecla para continuar con la siguiente imagen...")
            cv2.waitKey(0)
            cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
