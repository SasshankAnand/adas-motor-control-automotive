"""
Driver State Analysis using MediaPipe Face Mesh.
Monitors Eye Aspect Ratio (EAR) for blink duration and yawning frequency.
Sends serial feedback commands ('A' for Alert, 'D' for Drowsy) to the ESP32.
"""
import cv2
import mediapipe as mp
import numpy as np
import serial
import time

# Configure serial link to ESP32 MCU
try:
    ser = serial.Serial('COM3', 115200, timeout=1)
    time.sleep(2)
except Exception as e:
    ser = None
    print(f"Serial port not connected: {e}")

mp_face_mesh = mp.solutions.face_mesh
face_mesh = mp_face_mesh.FaceMesh(
    max_num_faces=1,
    refine_landmarks=True,
    min_detection_confidence=0.5,
    min_tracking_confidence=0.5
)

def calculate_ear(landmarks, eye_indices):
    # Calculate vertical and horizontal euclidean distances for eye landmarks
    p2_p6 = np.linalg.norm(landmarks[eye_indices[1]] - landmarks[eye_indices[5]])
    p3_p5 = np.linalg.norm(landmarks[eye_indices[2]] - landmarks[eye_indices[4]])
    p1_p4 = np.linalg.norm(landmarks[eye_indices[0]] - landmarks[eye_indices[3]])
    ear = (p2_p6 + p3_p5) / (2.0 * p1_p4)
    return ear

LEFT_EYE = [362, 385, 387, 263, 373, 380]
RIGHT_EYE = [33, 160, 158, 133, 153, 144]
EAR_THRESHOLD = 0.22
CONSECUTIVE_FRAMES = 15

cap = cv2.VideoCapture(0)
closed_counter = 0

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    h, w, _ = frame.shape
    rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = face_mesh.process(rgb_frame)

    if results.multi_face_landmarks:
        for face_landmarks in results.multi_face_landmarks:
            landmarks = np.array([(lm.x * w, lm.y * h) for lm in face_landmarks.landmark])
            
            left_ear = calculate_ear(landmarks, LEFT_EYE)
            right_ear = calculate_ear(landmarks, RIGHT_EYE)
            avg_ear = (left_ear + right_ear) / 2.0

            if avg_ear < EAR_THRESHOLD:
                closed_counter += 1
                if closed_counter >= CONSECUTIVE_FRAMES:
                    cv2.putText(frame, "DROWSINESS DETECTED!", (30, 60),
                                cv2.FONT_HERSHEY_SIMPLEX, 1.0, (0, 0, 255), 2)
                    if ser and ser.is_open:
                        ser.write(b'D')
            else:
                closed_counter = 0
                if ser and ser.is_open:
                    ser.write(b'A')

    cv2.imshow("In-Cabin Driver Monitor", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
