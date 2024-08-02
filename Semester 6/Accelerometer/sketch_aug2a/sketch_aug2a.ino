#include "BluetoothSerial.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>


// Define I2C pins for ESP32
#define I2C_SDA 21
#define I2C_SCL 22
//miniplayer
#define RX 10 // RX pin of DFPlayer Mini connected to pin 10 of Lilypad
#define TX 11 // TX pin of DFPlayer Mini connected to pin 11 of Lilypad
#define TOUCH_PIN 2

// Accelerometer
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
//miniplayer
SoftwareSerial mySoftwareSerial(RX, TX); // Create a software serial port
DFRobotDFPlayerMini myDFPlayer; // Create a DFPlayerMini object


// Bluetooth
BluetoothSerial SerialBT;

// Thresholds
const int jumpThreshold = 15;
const int fallThreshold = -15;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_BT"); // Bluetooth device name
  Serial.println("The device started, now you can pair it with Bluetooth!");

  // Initialize I2C communication
  Wire.begin(I2C_SDA, I2C_SCL);

  // Initialize accelerometer
  if (!accel.begin()) {
    Serial.println("Could not find a valid ADXL345 sensor, check wiring!");
    
  }

  
  SerialBT.println("Testing Connection");
}

void loop() {
  // Get accelerometer data
  sensors_event_t event;
  accel.getEvent(&event);

  // Read accelerometer values
  float xValue = event.acceleration.x;
  float yValue = event.acceleration.y;
  float zValue = event.acceleration.z;

  // Bluetooth communication
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }

  // Print axis values to Bluetooth and Serial
  // SerialBT.print("X-Value: ");
  // SerialBT.println(xValue);
  // SerialBT.println("----------------------------------");
  // SerialBT.print("Y-Value: ");
  // SerialBT.println(yValue);
  // SerialBT.println("----------------------------------");
  // SerialBT.print("Z-Value: ");
  // SerialBT.println(zValue);
  // SerialBT.println("----------------------------------");

  Serial.print("X-axis: ");
  Serial.println(xValue);
  Serial.print("Y-axis: ");
  Serial.println(yValue);
  Serial.print("Z-axis: ");
  Serial.println(zValue);
  delay(200);
  // Detect jump based on z-axis acceleration
  if (zValue > jumpThreshold) {
    SerialBT.println("Jump detected! Yahoo! ^^");
    Serial.println("Jump detected! Yahoo! ^^");
    delay(2000);
  }
  
  if (yValue > jumpThreshold) {
    SerialBT.println("Lateral- Right Movement Detected");
    Serial.println("Lateral- Right Movement Detected");
    delay(2000);
  }
  if (yValue < fallThreshold) {
    SerialBT.println("Lateral-Left Movement Detected");
    Serial.println("Lateral Movement Detected");
    delay(2000);
  }
  if (xValue > jumpThreshold) {
    SerialBT.println("Fowardsies? detected! Yahoo! ^^");
    Serial.println("Fowardsies? Yahoo! ^^");
    delay(2000);
  }
  if (xValue < fallThreshold) {
    SerialBT.println("Backsies? detected! Yahoo! ^^");
    Serial.println("Backsies? Yahoo! ^^");
    delay(2000);
  }
  delay(20);
   // Adjust delay as needed
}
