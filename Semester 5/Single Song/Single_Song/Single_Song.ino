#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

#define RX 10 // RX pin of DFPlayer Mini connected to pin 10 of Lilypad
#define TX 11 // TX pin of DFPlayer Mini connected to pin 11 of Lilypad

SoftwareSerial mySoftwareSerial(RX, TX); // Create a software serial port

DFRobotDFPlayerMini myDFPlayer; // Create a DFPlayerMini object

void setup() {
  // Initialize serial communication with DFPlayer Mini
  mySoftwareSerial.begin(9600);
  myDFPlayer.begin(mySoftwareSerial);

  myDFPlayer.volume(100);

  // Delay for 5 seconds
  delay(5000);

  // Play the sound file on the micro SD card
  myDFPlayer.play(1); // Replace "1" with the index of your sound file on the SD card
}

void loop() {
  // Nothing to do here as we only want to play the sound once
}