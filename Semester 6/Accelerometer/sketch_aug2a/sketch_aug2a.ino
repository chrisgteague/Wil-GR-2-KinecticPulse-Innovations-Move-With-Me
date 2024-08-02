#include "BluetoothSerial.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

// Define I2C pins for ESP32
#define I2C_SDA 21
#define I2C_SCL 22

// DFPlayer Mini
#define RX_PIN 12
#define TX_PIN 13

// Accelerometer
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// DFPlayer Mini
SoftwareSerial mySoftwareSerial(RX_PIN, TX_PIN); // Create a software serial port
DFRobotDFPlayerMini myDFPlayer; // Create a DFPlayerMini object

// Bluetooth
BluetoothSerial SerialBT;

// Thresholds
const int jumpThreshold = 15;
const int fallThreshold = -15;

void setup() {
  Serial.begin(115200);
  Serial.begin(9600);
  SerialBT.begin("ESP32_BT"); // Bluetooth device name
  Serial.println("The device started, now you can pair it with Bluetooth!");

  // Initialize I2C communication
  Wire.begin(I2C_SDA, I2C_SCL);

  // Initialize accelerometer
  if (!accel.begin()) {
    Serial.println("Could not find a valid ADXL345 sensor, check wiring!");
  }

  // Initialize DFPlayer Mini
  mySoftwareSerial.begin(9600);
  if (!myDFPlayer.begin(mySoftwareSerial)) {  // Use softwareSerial to communicate with DFPlayer Mini
    Serial.println("Unable to begin:");
    Serial.println("1. Please recheck the connection!");
    Serial.println("2. Please insert the SD card!");
    while (true);
  }
  myDFPlayer.volume(30);  // Set volume to 30 (0~30).
  
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
    myDFPlayer.play(2); // Play the fourth track on the DFPlayer Mini
    delay(2000);
  }
  
  if (yValue > jumpThreshold) {
    SerialBT.println("Lateral- Right Movement Detected");
    Serial.println("Lateral- Right Movement Detected");
    myDFPlayer.play(2); // Play the second track on the DFPlayer Mini
    delay(2000);
  }
  if (yValue < fallThreshold) {
    SerialBT.println("Lateral-Left Movement Detected");
    Serial.println("Lateral Movement Detected");
    myDFPlayer.play(2); // Play the third track on the DFPlayer Mini
    delay(2000);
  }
  if (xValue > jumpThreshold) {
    SerialBT.println("Fowardsies? detected! Yahoo! ^^");
    Serial.println("Fowardsies? Yahoo! ^^");
    myDFPlayer.play(2); // Play the fifth track on the DFPlayer Mini
    delay(2000);
  }
  if (xValue < fallThreshold) {
    SerialBT.println("Backsies? detected! Yahoo! ^^");
    Serial.println("Backsies? Yahoo! ^^");
    myDFPlayer.play(2); // Play the sixth track on the DFPlayer Mini
    delay(2000);
  }

  delay(20); // Adjust delay as needed
}
