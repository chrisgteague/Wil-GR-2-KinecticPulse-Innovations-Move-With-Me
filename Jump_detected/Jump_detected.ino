#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

// Accelerometer pins
const int xPin = A3;
const int yPin = A4;
const int zPin = A5;

const int jumpThreshold = 450;  // Adjust this value based on your calibration
const int fallThreshold = 450;  // Adjust this value based on your calibration

bool inAir = false;  // To keep track of whether the jump is in progress

// DFPlayer pins
#define RX 10 // RX pin of DFPlayer Mini connected to pin 10 of Arduino Mega
#define TX 11 // TX pin of DFPlayer Mini connected to pin 11 of Arduino Mega

SoftwareSerial mySoftwareSerial(RX, TX); // Create a software serial port
DFRobotDFPlayerMini myDFPlayer; // Create a DFPlayerMini object

void setup() {
  // Initialize serial communications
  Serial.begin(9600);
  mySoftwareSerial.begin(9600);

  // Initialize DFPlayer
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println("Unable to begin:");
    Serial.println("1. Please recheck the connection!");
    Serial.println("2. Insert the SD card!");
    while (true); // Stop execution if DFPlayer fails to initialize
  }
  Serial.println("DFPlayer Mini online.");

  // Set volume
  myDFPlayer.volume(30);  // Volume level between 0 and 30

  // Delay to ensure everything is set up correctly
  delay(1000);
}

void loop() {
  int zValue = analogRead(zPin);

  if (!inAir && zValue > jumpThreshold) {
    // Detected the start of the jump
    inAir = true;
    Serial.println("Jump detected!");
    myDFPlayer.play(4);
  }
  }