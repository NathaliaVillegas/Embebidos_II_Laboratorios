import cv2

def main():
    img = cv2.imread("mario.jpg")

    print("Seleccione el tamaño de la imagen:")
    print("1. Original\n2. Pequeño (Small)\n3. Mediano (Medium)\n4. Grande (Big)")
    opcion = input("Ingrese el número de opción: ")

    if opcion == "1":
        output = img
    elif opcion == "2":
        output = cv2.resize(img, (0,0), fx=0.3, fy=0.3) 
    elif opcion == "3":
        output = cv2.resize(img, (0,0), fx=0.6, fy=0.6) 
    elif opcion == "4":
        output = cv2.resize(img, (0,0), fx=1.5, fy=1.5) 
    else:
        print("Opción no válida")
        return

    cv2.imshow("Imagen Re-escalada", output)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
