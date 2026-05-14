import cv2
import numpy as np

video_path = 'bouncing.mp4.mp4'
cap = cv2.VideoCapture(video_path)

subtractor_mog2 = cv2.createBackgroundSubtractorMOG2(history=500, varThreshold=50, detectShadows=True)
subtractor_knn = cv2.createBackgroundSubtractorKNN(history=500, dist2Threshold=400, detectShadows=True)

print("Reproduciendo comparativa... Presiona 'q' para salir.")

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    frame = cv2.resize(frame, (400, 300))

    mask_mog2 = subtractor_mog2.apply(frame)
    mask_knn = subtractor_knn.apply(frame)

    cv2.imshow('Original', frame)
    cv2.imshow('Sustractor MOG2', mask_mog2)
    cv2.imshow('Sustractor KNN', mask_knn)

    if cv2.waitKey(1) & 0xFF == 27:
        break

cap.release()
cv2.destroyAllWindows()