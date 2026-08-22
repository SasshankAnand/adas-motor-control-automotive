# ADAS-Inspired Motor Control for Automotive Applications

An intelligent, context-aware motor control system that integrates **Advanced Driver Assistance Systems (ADAS)** concepts with embedded control logic on an **ESP32 MCU**. The architecture fuses in-cabin driver monitoring (intoxication & drowsiness) with multi-directional ultrasonic obstacle sensing to dynamically regulate vehicle speed and torque using **Model Predictive Control (MPC)** and adaptive PWM.

## Authors & Affiliation
- **G. Siddharth** (23BEC0003, VIT Vellore)
- **Hrithika Muraleedharan** (23BEC0106, VIT Vellore)
- **Sasshank Anand** (23BEC0099, VIT Vellore)
- **Anagha S** (23BEC0173, VIT Vellore)
- **Vaibhav Nangia** (23BEC0207, VIT Vellore)
- **Dr. Nisha J. S.** (Faculty Coordinator, School of Electronics Engineering, VIT Vellore)

## System Architecture

```text
       +-------------------------+
       |   MQ-3 Alcohol Sensor   | ---> Vehicle Startup Lockout / Ignition Inhibit
       +-------------------------+
       +-------------------------+
       |  4x Ultrasonic Sensors  | ---> 2-400 cm Multi-Directional Collision Avoidance
       +-------------------------+
       +-------------------------+
       |  Vision (MediaPipe/CV)  | ---> Eye Blink & Yawn Driver Drowsiness Detection
       +-------------------------+
                    │
                    ▼
         ┌─────────────────────┐
         │      ESP32 MCU      │ <--- Model Predictive Control (MPC) Algorithm
         └─────────────────────┘
                    │
                    ▼
   +---------------------------------+
   |   Dual DC Motor PWM Drivers     | ---> Adaptive Acceleration, Settling & Braking
   +---------------------------------+
```

## Key Hardware & Software Features
- **Microcontroller Unit (MCU):** ESP32 30-Pin Dev Module coordinating sensor fusion and PWM actuation.
- **Intoxication Prevention:** MQ-3 gas sensor module detects breath alcohol content ($0.05\text{ mg/L} - 10\text{ mg/L}$); locks motor startup when threshold is exceeded.
- **Surround Sensing:** 4x HC-SR04 ultrasonic sensors (Front, Rear, Left, Right) covering $2\text{ cm} - 400\text{ cm}$ range for adaptive cruise control and automated emergency stopping.
- **Driver State Vision Pipeline:** OpenCV and MediaPipe Face Mesh track Eye Aspect Ratio (EAR) and yawning frequency to detect fatigue/distraction
- **Actuation Layer:** Dual DC motors driven via L298N H-Bridge with MPC-based speed profile adjustments.

## Repository Structure
```text
├── firmware/
│   ├── adas_motor_controller.ino     # Main ESP32 embedded control & sensor fusion firmware
│   └── pin_config.h                  # Hardware pin mappings (Sensors, PWM, LCD)
├── vision/
│   ├── driver_monitor.py             # OpenCV & MediaPipe facial fatigue detection
│   └── mpc_controller.py             # Model Predictive Control simulation script
├── requirements.txt
└── README.md
```
