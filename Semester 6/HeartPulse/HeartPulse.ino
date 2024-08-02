#include <PulseSensorPlayground.h> // Includes the PulseSensorPlayground Library.

const int PulseWire = A0;  // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
const int LED13 = 13;      // The on-board Arduino LED, close to PIN 13.
int Threshold = 410;       // Determine which Signal to "count as a beat" and which to ignore.

PulseSensorPlayground pulseSensor;  // Creates an instance of the PulseSensorPlayground object called "pulseSensor"

void setup() {
  // Initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  // Configure the PulseSensor object:
  pulseSensor.analogInput(PulseWire);
  pulseSensor.blinkOnPulse(LED13); // Automatically blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(Threshold);

  if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor Object!"); // This prints one time at Arduino power-up, or on Arduino reset.
  }
}

void loop() {
  int myBPM = pulseSensor.getBeatsPerMinute(); // Calls function on our pulseSensor object that returns BPM as an "int".

  if (pulseSensor.sawStartOfBeat()) { // Constantly test to see if "a beat happened".
    Serial.println("♥  A HeartBeat Happened! "); // If test is "true", print a message "a heartbeat happened".
    Serial.print("BPM: "); // Print phrase "BPM: "
    Serial.println(myBPM); // Print the value inside of myBPM.
  }

  //delay(20); // Wait for 20 milliseconds.
}
