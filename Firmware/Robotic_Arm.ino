
#include <Servo.h>

// =====================================
// Mini Robotic Arm
// Arduino Uno Rev3 CH340
// =====================================

// Joystick 1
const int JOY1_X = A0;  // Base rotation
const int JOY1_Y = A1;  // Elbow joint 1

// Joystick 2
const int JOY2_X = A2;  // Gripper rotation
const int JOY2_Y = A3;  // Elbow joint 2 / arm height

// Push buttons
const int BUTTON_OPEN  = 2;
const int BUTTON_CLOSE = 3;

// Servo signal pins
const int SERVO_BASE       = 4;
const int SERVO_ELBOW1     = 5;
const int SERVO_ELBOW2     = 6;
const int SERVO_ROTATION   = 7;
const int SERVO_GRIPPER    = 8;

// Servo objects
Servo baseServo;
Servo elbow1Servo;
Servo elbow2Servo;
Servo rotationServo;
Servo gripperServo;

// Servo angle limits
// Adjust these after testing the real mechanism.
const int BASE_MIN = 0;
const int BASE_MAX = 180;

const int ELBOW1_MIN = 20;
const int ELBOW1_MAX = 160;

const int ELBOW2_MIN = 20;
const int ELBOW2_MAX = 160;

const int ROTATION_MIN = 0;
const int ROTATION_MAX = 180;

const int GRIPPER_OPEN_ANGLE = 90;
const int GRIPPER_CLOSE_ANGLE = 20;

// Joystick calibration
const int JOY_CENTER = 512;
const int JOY_DEAD_ZONE = 35;

// Current servo positions
int baseAngle = 90;
int elbow1Angle = 90;
int elbow2Angle = 90;
int rotationAngle = 90;
int gripperAngle = GRIPPER_OPEN_ANGLE;

// Timing for smooth servo movement
unsigned long lastMoveTime = 0;
const unsigned long MOVE_INTERVAL_MS = 15;

// =====================================
// Helper: map joystick movement to angle
// =====================================
int joystickToAngle(
  int reading,
  int currentAngle,
  int minAngle,
  int maxAngle
) {
  int offset = reading - JOY_CENTER;

  // Ignore small joystick fluctuations.
  if (abs(offset) < JOY_DEAD_ZONE) {
    return currentAngle;
  }

  // Convert the joystick's 0-1023 reading
  // into the permitted servo angle range.
  int targetAngle = map(
    reading,
    0, 1023,
    minAngle, maxAngle
  );

  return constrain(targetAngle, minAngle, maxAngle);
}

// =====================================
// Setup
// =====================================
void setup() {
  Serial.begin(9600);

  pinMode(BUTTON_OPEN, INPUT_PULLUP);
  pinMode(BUTTON_CLOSE, INPUT_PULLUP);

  baseServo.attach(SERVO_BASE);
  elbow1Servo.attach(SERVO_ELBOW1);
  elbow2Servo.attach(SERVO_ELBOW2);
  rotationServo.attach(SERVO_ROTATION);
  gripperServo.attach(SERVO_GRIPPER);

  // Start all joints at their centre positions.
  baseServo.write(baseAngle);
  elbow1Servo.write(elbow1Angle);
  elbow2Servo.write(elbow2Angle);
  rotationServo.write(rotationAngle);
  gripperServo.write(gripperAngle);

  Serial.println("Mini Robotic Arm Ready!");
}

// =====================================
// Main control loop
// =====================================
void loop() {
  // Read both joystick modules.
  int joy1X = analogRead(JOY1_X);
  int joy1Y = analogRead(JOY1_Y);
  int joy2X = analogRead(JOY2_X);
  int joy2Y = analogRead(JOY2_Y);

  // Update movement at regular intervals.
  if (millis() - lastMoveTime >= MOVE_INTERVAL_MS) {
    lastMoveTime = millis();

    // Servo 1: base left/right
    baseAngle = joystickToAngle(
      joy1X, baseAngle, BASE_MIN, BASE_MAX
    );

    // Servo 2: elbow joint 1
    elbow1Angle = joystickToAngle(
      joy1Y, elbow1Angle, ELBOW1_MIN, ELBOW1_MAX
    );

    // Servo 3: elbow joint 2 / arm height
    elbow2Angle = joystickToAngle(
      joy2Y, elbow2Angle, ELBOW2_MIN, ELBOW2_MAX
    );

    // Servo 4: rotate the gripper
    rotationAngle = joystickToAngle(
      joy2X, rotationAngle, ROTATION_MIN, ROTATION_MAX
    );

    // Apply joystick commands.
    baseServo.write(baseAngle);
    elbow1Servo.write(elbow1Angle);
    elbow2Servo.write(elbow2Angle);
    rotationServo.write(rotationAngle);
  }

  bool openPressed = (digitalRead(BUTTON_OPEN) == LOW);
  bool closePressed = (digitalRead(BUTTON_CLOSE) == LOW);

  if (openPressed && !closePressed) {
    gripperAngle = GRIPPER_OPEN_ANGLE;
    gripperServo.write(gripperAngle);
  }
  else if (closePressed && !openPressed) {
    gripperAngle = GRIPPER_CLOSE_ANGLE;
    gripperServo.write(gripperAngle);
  }

  delay(5);
}
