#include "BluetoothSerial.h"
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

//Accelerometer pins
const int xPin = A3;
const int yPin = A4;
const int zPin = A5;

const int jumpThreshold = 90;  // Adjust this value based on your calibration
const int fallThreshold = 70;  // Adjust this value based on your calibration

bool inAir = false;  // To keep track of whether the jump is in progress

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_BT"); // Bluetooth device name
  Serial.println("The device started, now you can pair it with Bluetooth!");
  delay(20000);
  SerialBT.println("Testing Connection");
}

void loop() {
  int zValue = analogRead(zPin);
inAir = false;
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }
  delay(20);
   

  if (!inAir && zValue > jumpThreshold) {
    // Detected the start of the jump
    inAir = true;
    SerialBT.println("Jump detected!");
    
  }
Serial.println(zValue);
}