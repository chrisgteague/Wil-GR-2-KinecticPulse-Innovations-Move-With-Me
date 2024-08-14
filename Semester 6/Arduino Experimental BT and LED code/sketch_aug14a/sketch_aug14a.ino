#include "BluetoothSerial.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

// Define I2C pins for ESP32
#define I2C_SDA 21
#define I2C_SCL 22

// DFPlayer Mini
#define RX_PIN 12
#define TX_PIN 13

// Accelerometer
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// DFPlayer Mini
SoftwareSerial mySoftwareSerial(RX_PIN, TX_PIN);  // Create a software serial port
DFRobotDFPlayerMini myDFPlayer;                   // Create a DFPlayerMini object

// Bluetooth
BluetoothSerial SerialBT;

#define PIXEL_PIN 15      // Data Pin of Led strip
#define PIXEL_COUNT 10    // Number of LEDs in the strip
#define BRIGHTNESS 7      // Use 96 for medium brightness
#define SPEED 50          // Speed of each Color Transition (in ms)

// Thresholds
const int jumpThreshold = 15;
const int fallThreshold = -15;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

bool RedOld = LOW;
bool GreenOld = LOW;
bool offOld = HIGH;
int showType = 0;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_BT");  // Bluetooth device name
  Serial.println("The device started, now you can pair it with Bluetooth!");
  SerialBT.println("Connected to Arduino");

  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show();

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
    while (true)
      ;
  }
  myDFPlayer.volume(30);  // Set volume to 30 (0~30).
}

char a;
char b;
void startShow(int i) {
  switch (i) {
    case 0:
      colorWipe(strip.Color(0, 0, 0), SPEED);  // Black/off
      break;

    case 1:
      colorWipe(strip.Color(255, 0, 0), SPEED);  // Red
      break;

    case 2:
      colorWipe(strip.Color(0, 255, 0), SPEED);  // Green
      break;

    default:
      colorWipe(strip.Color(0, 0, 0), SPEED);  // Black/off
      break;
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// Color wheel function (not used but kept for completeness)
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void loop() {
  // Get accelerometer data
  sensors_event_t event;
  accel.getEvent(&event);

  // Read accelerometer values
  float xValue = event.acceleration.x;
  float yValue = event.acceleration.y;
  float zValue = event.acceleration.z;
  delay(200);

  bool Red = LOW;
  bool Green = LOW;
  bool off = LOW;


 a = (char)SerialBT.read();

  

    switch (a) {
        case 'r':
            Red = HIGH;
            SerialBT.println("CHANGING TO RED");
            Green = LOW;
            off = LOW;
            a = '0';
            break;
        case 'g':
            Green = HIGH;
            SerialBT.println("CHANGING TO GREEN");
            Red = LOW;
            off = LOW;
            a = '0';
            break;
        case 'o':
            off = HIGH;
            SerialBT.println("TURNING OFF LEDs..");
            Red = LOW;
            Green = LOW;
            a = '0';
            break;
        default:
            Red = LOW;
            Green = LOW;
            off = LOW;
            a = '0';
            break;
    }

    
    // Red LED animation
    if (Red == HIGH && RedOld == LOW) {
        delay(20);
        showType = 1;  // Red animation Type 1
        startShow(showType);
    }

    // Green LED animation
    if (Green == HIGH && GreenOld == LOW) {
        delay(20);
        showType = 2;  // Green animation Type 2
        startShow(showType);
    }

    // Turning off LEDs
    if (off == HIGH && offOld == LOW) {
        delay(20);
        showType = 0;  // Off animation Type 0
        startShow(showType);
    }

    // Update the old state
    RedOld = Red;
    GreenOld = Green;
    offOld = LOW;

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
    myDFPlayer.play(2);  // Play the fourth track on the DFPlayer Mini
    delay(2000);
    b = 'g';
  }

  if (yValue > jumpThreshold) {
    SerialBT.println("Lateral- Right Movement Detected");
    Serial.println("Lateral- Right Movement Detected");
    myDFPlayer.play(2);  // Play the second track on the DFPlayer Mini
    a = 'g';
    Green = HIGH;
    delay(2000);
    
  }
  if (yValue < fallThreshold) 
  {
    SerialBT.println("Lateral-Left Movement Detected");
    Serial.println("Lateral Movement Detected");
    myDFPlayer.play(2);  // Play the third track on the DFPlayer Mini
    a = 'g';
    Green = HIGH;
    delay(2000);
    
  }
  if (xValue > jumpThreshold) {
    SerialBT.println("Fowardsies? detected! Yahoo! ^^");
    Serial.println("Fowardsies? Yahoo! ^^");
    myDFPlayer.play(2);  // Play the fifth track on the DFPlayer Mini
    a = 'g';
    Green = HIGH;
    delay(2000);
    
  }
  if (xValue < fallThreshold) {
    

    SerialBT.println("Backsies? detected! Yahoo! ^^");
    Serial.println("Backsies? Yahoo! ^^");
    myDFPlayer.play(2);  // Play the sixth track on the DFPlayer Mini
    a = 'g';
    Green = HIGH;
    delay(2000);
    
  }
  
  delay(20);  // Adjust delay as needed
}
