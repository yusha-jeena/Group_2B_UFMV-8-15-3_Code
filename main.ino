#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <WiFi.h>
#include <WebServer.h>
#include <math.h>
#include "web_interface.h"

// Config section

const char* ssid = "AL5D_Robot_Controller";
const char* password = "12345678";

// Servo pin numbers on the PCA9685 board
#define PIN_BASE 0
#define PIN_SHOULDER 1
#define PIN_ELBOW 2
#define PIN_WRIST 3
#define PIN_WRIST_ROT 4
#define PIN_GRIPPER 5

// Calibration data
// These were measured manually for our exact robot

// Base Servo
#define BASE_MIN_PULSE 110
#define BASE_MAX_PULSE 525
#define BASE_MIN_DEG -103.6
#define BASE_MAX_DEG 100.9

// Shoulder Servo
#define SHLD_MIN_PULSE 138
#define SHLD_MAX_PULSE 520
#define SHLD_MIN_DEG -98.6
#define SHLD_MAX_DEG 100.5

// Elbow Servo
#define ELBW_MIN_PULSE 125
#define ELBW_MAX_PULSE 480
#define ELBW_MIN_DEG -7.2
#define ELBW_MAX_DEG 188.3

// Wrist Servo
#define WRST_MIN_PULSE 100
#define WRST_MAX_PULSE 550
#define WRST_MIN_DEG -78.5
#define WRST_MAX_DEG 129.2

// Wrist Rotation Servo
#define ROT_MIN_PULSE 130
#define ROT_MAX_PULSE 550
#define ROT_MIN_DEG -30.8
#define ROT_MAX_DEG 168.2

// Gripper Servo (note linear calibration)
#define GRIP_MIN_PULSE 200
#define GRIP_MAX_PULSE 375
#define GRIP_OPEN_MM 18.1
#define GRIP_CLOSE_MM 0.0

// Robot Arm Lengths in mm
#define LENGTH_A 146.5 // Shoulder to Elbow
#define LENGTH_B 180.5 // Elbow to Wrist
#define HEIGHT_OFFSET 92.5 // Base height

WebServer server(80);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Values coming from the Website
float input_rot = 0.0;
float input_reach = 150.0;
float input_z = 100.0;
float input_pitch = 0.0;
float input_roll = 0.0;
float input_grip = 18.1;
float input_speed = 0.05;

// Current angles the robot is actually at
float current_base = 0.0;
float current_shoulder = 0.0;
float current_elbow = 0.0;
float current_wrist = 0.0;
float current_rot = 0.0;
float current_grip = 18.1;

// Target angles calculated by the Math
float target_base = 0.0;
float target_shoulder = 0.0;
float target_elbow = 0.0;
float target_wrist = 0.0;

// Timer for the loop
long last_time = 0;


void setup() {
  Serial.begin(115200);
  
  // Servo driver
  Wire.begin(21, 22);
  pwm.begin();
  pwm.setPWMFreq(50);

  // Wifi
  WiFi.softAP(ssid, password);
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Setup web server
  server.on("/", []() {
    server.send(200, "text/html", index_html);
  });
  
  // When user moves a slider
  server.on("/set", handleCommand);
  
  server.begin();
  
  calculateAngles();
}


void loop() {
  // Check fro clicks
  server.handleClient();
  
  // Update the robot every 20 milliseconds
  long now = millis();
  if (now - last_time >= 20) {
    last_time = now;
    
    // Calculate where it should be
    calculateAngles();
    
    // Smoothly move current angles towards target angles
    // Formula: new_pos = (target * speed) + (old_pos * (1 - speed))
    current_base = (target_base * input_speed) + (current_base * (1.0 - input_speed));
    current_shoulder = (target_shoulder * input_speed) + (current_shoulder * (1.0 - input_speed));
    current_elbow = (target_elbow * input_speed) + (current_elbow * (1.0 - input_speed));
    current_wrist = (target_wrist * input_speed) + (current_wrist * (1.0 - input_speed));
    
    // Rotation and Grip are direct from sliders
    current_rot = (input_roll * input_speed) + (current_rot * (1.0 - input_speed));
    current_grip = (input_grip * input_speed) + (current_grip * (1.0 - input_speed));

    // Send pulse commands to the servos
    updateServos();
  }
}

// Read values from the URL
void handleCommand() {
  if (server.hasArg("rot")) input_rot = server.arg("rot").toFloat();
  if (server.hasArg("rch")) input_reach = server.arg("rch").toFloat();
  if (server.hasArg("z")) input_z = server.arg("z").toFloat();
  if (server.hasArg("p")) input_pitch = server.arg("p").toFloat();
  if (server.hasArg("r")) input_roll = server.arg("r").toFloat();
  if (server.hasArg("g")) input_grip = server.arg("g").toFloat();
  
  // Calculate speed based on the slider (converts 1 to 100 to smaller values)
  if (server.hasArg("s")) {
    float s = server.arg("s").toFloat();
    input_speed = s / 500.0; 
    if (input_speed < 0.01) input_speed = 0.01;
  }
  
  server.send(200, "text/plain", "OK");
}

// Inverse Kinematics Math
// Calculates the angles needed to reach X, Y, Z
void calculateAngles() {
  // Base rotation is just input
  target_base = input_rot;

  // Calculate height relative to the shoulder joint
  float z_adjusted = input_z - HEIGHT_OFFSET;
  
  // Calculate the distance from shoulder to wrist
  float dist = sqrt((input_reach * input_reach) + (z_adjusted * z_adjusted));

  // Limit the reach to avoid damage
  float max_dist = LENGTH_A + LENGTH_B;
  if (dist > max_dist) dist = max_dist;
  if (dist < 50) dist = 50; 

  // Law of Cosines to find the Shoulder Angle
  float top_a = (LENGTH_A * LENGTH_A) + (dist * dist) - (LENGTH_B * LENGTH_B);
  float bottom_a = 2 * LENGTH_A * dist;
  float alpha_rad = acos(top_a / bottom_a);

  // Law of Cosines to find the Elbow Angle
  float top_b = (LENGTH_A * LENGTH_A) + (LENGTH_B * LENGTH_B) - (dist * dist);
  float bottom_b = 2 * LENGTH_A * LENGTH_B;
  float beta_rad = acos(top_b / bottom_b);

  // Calculate the global angle of the arm
  float theta = atan2(z_adjusted, input_reach);
  
  // Convert Radians to Degrees
  float shoulder_deg = ((theta + alpha_rad) * 180.0 / PI) - 90.0;
  float elbow_deg = 180.0 - (beta_rad * 180.0 / PI);
  
  // Calculate Wrist Angle to keep the gripper level
  // Formula: Pitch - (Shoulder Angle - Elbow Angle + 90)
  float forearm_angle = (shoulder_deg + 90.0) - elbow_deg;
  float wrist_deg = input_pitch - forearm_angle;

  // Save the targets
  target_shoulder = shoulder_deg;
  target_elbow = elbow_deg;
  target_wrist = wrist_deg;
}

// Convert angles to pulses and send to PCA9685
void updateServos() {
  moveServo(PIN_BASE, current_base, BASE_MIN_DEG, BASE_MAX_DEG, BASE_MIN_PULSE, BASE_MAX_PULSE);
  moveServo(PIN_SHOULDER, current_shoulder, SHLD_MIN_DEG, SHLD_MAX_DEG, SHLD_MIN_PULSE, SHLD_MAX_PULSE);
  moveServo(PIN_ELBOW, current_elbow, ELBW_MIN_DEG, ELBW_MAX_DEG, ELBW_MIN_PULSE, ELBW_MAX_PULSE);
  moveServo(PIN_WRIST, current_wrist, WRST_MIN_DEG, WRST_MAX_DEG, WRST_MIN_PULSE, WRST_MAX_PULSE);
  moveServo(PIN_WRIST_ROT, current_rot, ROT_MIN_DEG, ROT_MAX_DEG, ROT_MIN_PULSE, ROT_MAX_PULSE);

  // Gripper uses linear mapping
  // We flipped mapping as 18 is open
  float g = current_grip;
  if (g > GRIP_OPEN_MM) g = GRIP_OPEN_MM;
  if (g < GRIP_CLOSE_MM) g = GRIP_CLOSE_MM;
  
  // Map logic for float
  float range_mm = GRIP_CLOSE_MM - GRIP_OPEN_MM;
  float range_pulse = GRIP_MAX_PULSE - GRIP_MIN_PULSE;
  float percent = (g - GRIP_OPEN_MM) / range_mm;
  int grip_pulse = GRIP_MIN_PULSE + (percent * range_pulse);

  pwm.setPWM(PIN_GRIPPER, 0, grip_pulse);
}

// Function to move a single servo
void moveServo(int pin, float angle, float minAng, float maxAng, int minPulse, int maxPulse) {
  // CHeck to not go past limits
  if (angle < minAng) angle = minAng;
  if (angle > maxAng) angle = maxAng;

  // Map the angle to a pulse width
  // (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin
  float pulse = (angle - minAng) * (maxPulse - minPulse) / (maxAng - minAng) + minPulse;

  // Send to hardware
  pwm.setPWM(pin, 0, (int)pulse);
}