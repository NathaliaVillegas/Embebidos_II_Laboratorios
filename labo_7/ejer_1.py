import cv2

def main():
    img = cv2.imread("mario.jpg")
    
    if img is None:
        print("Error: No se encontró la imagen.")
        return

    resized_img = cv2.resize(img, (1000, 1000))
    
    cv2.imshow("Resolucion 1000x1000", resized_img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
