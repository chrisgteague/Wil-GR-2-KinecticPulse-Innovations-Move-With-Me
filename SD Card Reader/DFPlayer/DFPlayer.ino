#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#define RX 11 // RX of DFPlayer Mini connected to pin 11
#define TX 10 // TX of DFPlayer Mini connected to pin 10

int Red = 6;
int Yellow = 5;
int Green = 4;
int touchInput = 2;
int inputState;
int lastInputState = 1;
int tapCount = 0;
unsigned long lastTapTime = 0;
int dt = 500;
int speakerPin = 9; // Change this to the PWM pin connected to the speaker

SoftwareSerial mySerial(RX, TX); // Create a Software object

DFRobotDFPlayerMini myDFPlayer;

bool isPlaying = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(Green, OUTPUT);
  pinMode(Yellow, OUTPUT);
  pinMode(Red, OUTPUT);
  pinMode(touchInput, INPUT);
  pinMode(speakerPin, OUTPUT);

  // Initialize LEDs to a known state
  digitalWrite(Green, LOW);
  digitalWrite(Yellow, LOW);
  digitalWrite(Red, LOW);

  if (!myDFPlayer.begin(mySerial)) {
    Serial.println("Unable to begin:");
    Serial.println("1. Please recheck the connection!");
    Serial.println("2. Please insert the SD card!");
    while (true); // If DFPlayer Mini failed to initialize, stop the program
  }

  Serial.println("DFPlayer Mini online.");
}

void loop() {
  // put your main code here, to run repeatedly:
  inputState = digitalRead(touchInput);

  if (inputState == HIGH) {
    Serial.println("It works!!!");
  }

  if (lastInputState == LOW && inputState == HIGH) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastTapTime > 1000) {
      tapCount = 0;
    }
    lastTapTime = currentMillis;

    tapCount++;
    if (tapCount == 1) {
      digitalWrite(Green, HIGH);
      digitalWrite(Yellow, LOW);
      digitalWrite(Red, LOW);
      playSound(1);
    } else if (tapCount == 2) {
      digitalWrite(Green, LOW);
      digitalWrite(Yellow, HIGH);
      digitalWrite(Red, LOW);
      playSound(1);
    } else if (tapCount == 3) {
      digitalWrite(Green, LOW);
      digitalWrite(Yellow, LOW);
      digitalWrite(Red, HIGH);
      playSound(1);
    } else if (tapCount == 4) {
      digitalWrite(Green, LOW);
      digitalWrite(Yellow, LOW);
      digitalWrite(Red, LOW);
    }
  }
  lastInputState = inputState;
}

void playSound(uint8_t track) {
  if (!isPlaying) {
    isPlaying = true;
    myDFPlayer.play(track);
    while (myDFPlayer.readType() != DFPlayerPlayFinished) {
      delay(100);
    }
    isPlaying = false;
  }
}
