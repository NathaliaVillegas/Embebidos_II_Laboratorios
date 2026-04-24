import cv2

def main():
    img = cv2.imread("mario.jpg")
    if img is None: return

    print("Presiona cualquier tecla para rotar 90°. Presiona 'Esc' para salir.")
    
    while True:
        cv2.imshow("Rotacion 90 grados", img)
        
        key = cv2.waitKey(0)
        if key == 27: 
            break
        
        img = cv2.rotate(img, cv2.ROTATE_90_CLOCKWISE)

    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
