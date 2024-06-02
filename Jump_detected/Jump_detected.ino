const int xPin = A13;  // Pin connected to X output of BY-61
const int yPin = A14;  // Pin connected to Y output of BY-61
const int zPin = A15;  // Pin connected to Z output of BY-61

const int jumpThreshold = 450;  // Adjust this value based on your calibration
const int fallThreshold = 450;  // Adjust this value based on your calibration

bool inAir = false;  // To keep track of whether the jump is in progress


void setup() {

Serial.begin(9600);  // Initialize serial communication at 9600 baud

}

void loop() {

  int zValue = analogRead(zPin);

  if (!inAir && zValue > jumpThreshold) {
    // Detected the start of the jump
    inAir = true;
    Serial.println("Jump detected!");
  } else if (inAir && zValue < fallThreshold) {
    // Detected the free fall part of the jump
    inAir = false;
    Serial.println("Landed!");
  }

  delay(100);

}
