#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "pin_config.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Motor State Variables
int baseSpeed = 180; // Default PWM duty cycle (0-255)
bool isIntoxicated = false;
bool isDriverDrowsy = false;

long readDistanceCM(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    long duration = pulseIn(echoPin, HIGH, 25000); // 25ms timeout (~400cm max)
    if (duration == 0) return 400;
    return duration * 0.034 / 2;
}

void setMotorSpeed(int speedA, int speedB) {
    speedA = constrain(speedA, 0, 255);
    speedB = constrain(speedB, 0, 255);

    analogWrite(MOTOR_A_PWM, speedA);
    analogWrite(MOTOR_B_PWM, speedB);

    digitalWrite(MOTOR_A_IN1, HIGH);
    digitalWrite(MOTOR_A_IN2, LOW);
    digitalWrite(MOTOR_B_IN1, HIGH);
    digitalWrite(MOTOR_B_IN2, LOW);
}

void emergencyStop() {
    analogWrite(MOTOR_A_PWM, 0);
    analogWrite(MOTOR_B_PWM, 0);
    digitalWrite(LED_ALERT_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
}

void setup() {
    Serial.begin(115200);

    // Initialize Ultrasonic Pins
    pinMode(FRONT_TRIG_PIN, OUTPUT); pinMode(FRONT_ECHO_PIN, INPUT);
    pinMode(REAR_TRIG_PIN, OUTPUT);  pinMode(REAR_ECHO_PIN, INPUT);
    pinMode(LEFT_TRIG_PIN, OUTPUT);  pinMode(LEFT_ECHO_PIN, INPUT);
    pinMode(RIGHT_TRIG_PIN, OUTPUT); pinMode(RIGHT_ECHO_PIN, INPUT);

    // Initialize Motor Driver Pins
    pinMode(MOTOR_A_PWM, OUTPUT); pinMode(MOTOR_A_IN1, OUTPUT); pinMode(MOTOR_A_IN2, OUTPUT);
    pinMode(MOTOR_B_PWM, OUTPUT); pinMode(MOTOR_B_IN1, OUTPUT); pinMode(MOTOR_B_IN2, OUTPUT);
    pinMode(LED_ALERT_PIN, OUTPUT); pinMode(BUZZER_PIN, OUTPUT);

    // Initialize LCD
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("ADAS Motor Ctrl");
    lcd.setCursor(0, 1);
    lcd.print("Initializing...");
    delay(1500);
}

void loop() {
    // 1. Intoxication Check (MQ-3 Sensor)
    int alcoholVal = analogRead(MQ3_ANALOG_PIN);
    if (alcoholVal > ALCOHOL_THRESHOLD) {
        isIntoxicated = true;
        emergencyStop();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ALCOHOL DETECTED");
        lcd.setCursor(0, 1);
        lcd.print("IGNITION LOCKED");
        Serial.println(">>> [SAFETY INTERLOCK] Alcohol threshold exceeded. Motor locked.");
        delay(1000);
        return;
    }

    // 2. Parse serial data from In-Cabin Vision Driver Monitor (MediaPipe)
    if (Serial.available() > 0) {
        char cmd = Serial.read();
        if (cmd == 'D') isDriverDrowsy = true;   // Drowsiness flag
        else if (cmd == 'A') isDriverDrowsy = false; // Alert flag
    }

    // 3. Multi-Directional Obstacle Range Sensing
    long frontDist = readDistanceCM(FRONT_TRIG_PIN, FRONT_ECHO_PIN);
    long leftDist  = readDistanceCM(LEFT_TRIG_PIN, LEFT_ECHO_PIN);
    long rightDist = readDistanceCM(RIGHT_TRIG_PIN, RIGHT_ECHO_PIN);

    // 4. Adaptive Motor Actuation Logic (Collision Avoidance & Fatigue Response)
    if (frontDist < 15) {
        // Critical collision hazard: Immediate Stop
        emergencyStop();
        lcd.clear();
        lcd.print("EMERGENCY STOP");
        lcd.setCursor(0, 1);
        lcd.print("Front Dist: "); lcd.print(frontDist); lcd.print("cm");
    } else if (frontDist < 40 || isDriverDrowsy) {
        // Adaptive deceleration zone / Driver fatigue limit
        int reducedSpeed = map(frontDist, 15, 40, 60, 120);
        if (isDriverDrowsy) reducedSpeed = min(reducedSpeed, 80);
        setMotorSpeed(reducedSpeed, reducedSpeed);
        digitalWrite(LED_ALERT_PIN, HIGH);
        digitalWrite(BUZZER_PIN, LOW);
        
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(isDriverDrowsy ? "DROWSY WARNING" : "ADAPTIVE SLOW");
        lcd.setCursor(0, 1);
        lcd.print("Dist: "); lcd.print(frontDist); lcd.print("cm");
    } else {
        // Normal cruise conditions
        setMotorSpeed(baseSpeed, baseSpeed);
        digitalWrite(LED_ALERT_PIN, LOW);
        digitalWrite(BUZZER_PIN, LOW);
        
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("STATUS: NORMAL");
        lcd.setCursor(0, 1);
        lcd.print("Front: "); lcd.print(frontDist); lcd.print("cm");
    }

    delay(100);
}
