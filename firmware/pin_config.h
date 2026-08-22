#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

// --- MQ-3 Alcohol Sensor ---
#define MQ3_ANALOG_PIN       34   // ADC1 pin for alcohol concentration reading
#define ALCOHOL_THRESHOLD    400  // Predefined threshold for intoxication lockout

// --- Ultrasonic Sensor Pins (Front, Rear, Left, Right) ---
#define FRONT_TRIG_PIN       12
#define FRONT_ECHO_PIN       14

#define REAR_TRIG_PIN        27
#define REAR_ECHO_PIN        26

#define LEFT_TRIG_PIN        25
#define LEFT_ECHO_PIN        33

#define RIGHT_TRIG_PIN       32
#define RIGHT_ECHO_PIN       35

// --- L298N DC Motor Driver PWM Pins ---
#define MOTOR_A_PWM          18   // Left motor speed control
#define MOTOR_A_IN1          19
#define MOTOR_A_IN2          21

#define MOTOR_B_PWM          22   // Right motor speed control
#define MOTOR_B_IN1          23
#define MOTOR_B_IN2          5

// --- I2C LCD & Status Indicators ---
#define LED_ALERT_PIN        2    // Visual alert LED
#define BUZZER_PIN           4    // Local audio alert

#endif // PIN_CONFIG_H
