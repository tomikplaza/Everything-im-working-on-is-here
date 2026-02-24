import cv2
import os

face_haar = cv2.CascadeClassifier(cv2.data.haarcascades + "haarcascade_frontalface_default.xml")

for root, dir, file in os.walk("C:/Users/tomas/Desktop/Practice-Projects/Practice"):
    print(root)
    if "video_exp.mp4" in file:
        video_dir = os.path.join(root, "video_exp.mp4")
        print(video_dir)
        break

cap = cv2.VideoCapture(video_dir)

while True:
    ret, frame = cap.read()

    if not ret:
        break
    
    frame_gray = cv2.cvtColor(frame, cv2.COLOR_RGB2GRAY)

    for (x, y, w, h) in face_haar.detectMultiScale(frame_gray, scaleFactor=1.3, minNeighbors=6):
        cv2.rectangle(frame, (x, y), ((x+w), (y+h)), (255, 0, 0), 3)


    cv2.imshow("done", frame)

    cv2.waitKey(10)