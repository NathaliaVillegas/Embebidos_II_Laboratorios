import cv2

def main():
    img = cv2.imread("mario.jpg")
    if img is None: return
    
    img = cv2.resize(img, (400, 600))
    h, w = img.shape[:2] 

    mitad_h = h // 2
    superior = img[0:mitad_h, 0:w]
    inferior = img[mitad_h:h, 0:w]
    cv2.imshow("Mitad Superior", superior)
    cv2.imshow("Mitad Inferior", inferior)

    mitad_v = w // 2
    izquierda = img[0:h, 0:mitad_v]
    derecha = img[0:h, mitad_v:w]
    cv2.imshow("Mitad Izquierda", izquierda)
    cv2.imshow("Mitad Derecha", derecha)

    q1 = img[0:mitad_h, 0:mitad_v]
    q2 = img[0:mitad_h, mitad_v:w]
    q3 = img[mitad_h:h, 0:mitad_v]
    q4 = img[mitad_h:h, mitad_v:w]

    cv2.imshow("Quadrant 1", q1)
    cv2.imshow("Quadrant 2", q2)
    cv2.imshow("Quadrant 3", q3)
    cv2.imshow("Quadrant 4", q4)

    cv2.waitKey(0)
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
