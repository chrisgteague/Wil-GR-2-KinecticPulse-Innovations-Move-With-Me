#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

#define RX 10 // RX pin of DFPlayer Mini connected to pin 10 of Lilypad
#define TX 11 // TX pin of DFPlayer Mini connected to pin 11 of Lilypad
#define TOUCH_PIN 2

SoftwareSerial mySoftwareSerial(RX, TX); // Create a software serial port
DFRobotDFPlayerMini myDFPlayer; // Create a DFPlayerMini object

int tapCount = 0;

void setup() {
  // Initialize serial communication with DFPlayer Mini
  mySoftwareSerial.begin(9600);
  myDFPlayer.begin(mySoftwareSerial);

  myDFPlayer.volume(40);

  pinMode(TOUCH_PIN, INPUT);

  // Delay for 5 seconds
  delay(5000);
}

void loop() {
  // Check if touch sensor is triggered
  if (digitalRead(TOUCH_PIN) == HIGH) {
    delay(50); // Debouncing delay

    // Increment tap count
    tapCount++;

    // Play corresponding track based on tap count
    switch (tapCount) {
      case 1:
        myDFPlayer.play(1); // Play track 1
        break;
      case 2:
        myDFPlayer.play(2); // Play track 2
        break;
      case 3:
        myDFPlayer.play(3); // Play track 3
        break;
      case 4:
        myDFPlayer.stop(); // Stop playback
        tapCount = 0; // Reset tap count
        break;
      default:
        break;
    }

    // Reset tap count after 4 taps
    if (tapCount > 3) {
      tapCount = 0;
    }

    // Wait for touch release
    while (digitalRead(TOUCH_PIN) == HIGH) {
      delay(10);
    }
  }
}