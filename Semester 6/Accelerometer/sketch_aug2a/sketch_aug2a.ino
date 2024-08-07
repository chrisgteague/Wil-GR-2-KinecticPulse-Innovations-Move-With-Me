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
SoftwareSerial mySoftwareSerial(RX_PIN, TX_PIN); // Create a software serial port
DFRobotDFPlayerMini myDFPlayer; // Create a DFPlayerMini object

// Bluetooth
BluetoothSerial SerialBT;

#define PIXEL_PIN      15    // Data Pin of Led strip 
#define PIXEL_COUNT    10   // Number of LEDs in the strip
#define BRIGHTNESS     7    // use 96 for medium brightness
#define SPEED          50    // Speed of each Color Transition (in ms)
#define IMMEDIATELY    0    // Transition happen instantly
#define RAINBOW_SPEED  50    // Rainbow Transition speed

// Thresholds
const int jumpThreshold = 15;
const int fallThreshold = -15;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

bool offOld = LOW;
bool WhiteOld = LOW;
bool RedOld = LOW;
bool GreenOld = LOW;
bool BlueOld = LOW;
bool TopazOld = LOW;
bool LilacOld = LOW;
bool RainbowOld = LOW;
bool rgbOld = LOW;
int  showType = 0;

void setup() {
  Serial.begin(115200);
  Serial.begin(9600);
  SerialBT.begin("ESP32_BT"); // Bluetooth device name
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
    while (true);
  }
  myDFPlayer.volume(30);  // Set volume to 30 (0~30).
  
  SerialBT.println("Testing Connection");
}
char a;

void startShow(int i) {
  switch(i){

    case 0: colorWipe(strip.Color(0, 0, 0), SPEED);    // Black/off
            break;

    case 1: strip.setBrightness(255);                            // Changes the Brightness to MAX
            colorWipe(strip.Color(255, 255, 255), IMMEDIATELY);  // White
            strip.setBrightness(BRIGHTNESS);                     // Reset the Brightness to Default value
            break;  

    case 2: colorWipe(strip.Color(255, 0, 0), SPEED);  // Red
            break;

    case 3: colorWipe(strip.Color(0, 255, 0), SPEED);  // Green
            break;

    case 4: colorWipe(strip.Color(0, 0, 255), SPEED);  // Blue
            break;

    case 5: colorWipe(strip.Color(255, 192, 103), SPEED);  // Topaz
            break;            

    case 6: colorWipe(strip.Color(221, 130, 255), SPEED);  // Lilac
            break;            
    
    case 7: colorWipe(strip.Color(255, 0, 0), SPEED);  // Red
            colorWipe(strip.Color(0, 255, 0), SPEED);  // Green
            colorWipe(strip.Color(0, 0, 255), SPEED);  // Blue
            theaterChase(strip.Color(  0,   0, 127), SPEED); // Blue
            theaterChase(strip.Color(127,   0,   0), SPEED); // Red
            theaterChase(strip.Color(0,   127,   0), SPEED); // Green
            break;

    case 8: rainbowCycle(25);
            break;
    default:colorWipe(strip.Color(0, 0, 0), SPEED);    // Black/off
            break;

  }
}



//  ANIMATION #1 - COLOR WIPE

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}


//  ANIMATION #2 - RAINBOW COLORS

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*10; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//  ANIMATION #3 - THEATER CHASE

void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//  COLOR WHEEL


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
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
  bool off = LOW;
  bool White = LOW;
  bool Blue = LOW;
  bool Red = LOW;
  bool Green = LOW;
  bool Topaz = LOW;
  bool Lilac = LOW;
  bool Rainbow = LOW;
  bool rgb = LOW;
  bool ende;

    a= (char)SerialBT.read();




    if(a=='o')
    {
      off = HIGH;
          SerialBT.println("TURNING OFF LEDs..");

    }else{
          off = LOW;
    }
    
// ===========================================================================================

    if(a=='w')
    {
      White = HIGH;
          SerialBT.println("TURNING LEDs WHITE");

    }else{
          White = LOW;
    }
    
// ===========================================================================================

    if(a=='b')
    {
      Blue = HIGH;
          SerialBT.println("CHANGING TO BLUE");      
          
    }else{
          Blue = LOW;  
    }

// ===========================================================================================

if(a=='r')
    {
      Red = HIGH;
          SerialBT.println("CHANGING TO RED");            
    }else{
          Red = LOW;  
    }

// ===========================================================================================

if(a=='g')
    {
      Green = HIGH;
          SerialBT.println("CHANGING TO GREEN");      
          
    }else{
          Green = LOW;
    }

// ===========================================================================================

if(a=='t')
    {
      Topaz = HIGH;
          SerialBT.println("CHANGING TO TOPAZ");      
          
    }else{
          Topaz = LOW;
    }

// ===========================================================================================

if(a=='l')
    {
      Lilac = HIGH;
          SerialBT.println("CHANGING TO LILAC");      
          
    }else{
          Lilac = LOW;
    }

// ===========================================================================================

    if(a=='a')
    {
      Rainbow = HIGH;
          SerialBT.println("RAINBOW ANIMATION");      
          
    }else{
          Rainbow = LOW;  
    }
    
// ===========================================================================================

     if(a=='m')
    {
      rgb = HIGH;
          SerialBT.println("MIX COLORS");      
          
    }else{
          rgb = LOW;  
    }

    if (off == LOW && offOld == HIGH) {
    delay(20);
   
    
    if (off == LOW) {
       showType = 0  ;                            // Off animation Type 0
     
      startShow(showType);
    }
  }

// ===========================================================================================

if (White == LOW && WhiteOld == HIGH) {
    delay(20);
   
    
    if (White == LOW) {
       showType = 1  ;                            // White animation Type 1
     
      startShow(showType);
    }
  }

  
// ===========================================================================================  
  
  if (Red == LOW && RedOld == HIGH) {
    delay(20);
   
    
    if (Red == LOW) {
       showType = 2  ;                            // Red animation Type 2
     
      startShow(showType);
    }
  }

// ===========================================================================================

if (Green == LOW && GreenOld == HIGH) {
    delay(20);
   
    
    if (Green == LOW) {
       showType = 3  ;                            // Green animation Type 3
     
      startShow(showType);
    }
  }
  
// ===========================================================================================

if (Blue == LOW && BlueOld == HIGH) {
    delay(20);
   
    
    if (Blue == LOW) {
       showType = 4  ;                            // Blue animation Type 4
     
      startShow(showType);
    }
  }

// ===========================================================================================

if (Topaz == LOW && TopazOld == HIGH) {
    delay(20);
   
    
    if (Topaz == LOW) {
       showType = 5  ;                            // Topaz animation Type 5
     
      startShow(showType);
    }
  }

// ===========================================================================================

if (Lilac == LOW && LilacOld == HIGH) {
    delay(20);
   
    
    if (Lilac == LOW) {
       showType = 6  ;                            // Topaz animation Type 6
     
      startShow(showType);
    }
  }
    
// ===========================================================================================


    if (Rainbow == LOW && RainbowOld == HIGH) {
    delay(20);

    if (Rainbow == LOW) {
      showType = 8;                            // Rainbow animation Type 8
      startShow(showType);
    }
  }

// ===========================================================================================

      if (rgb == LOW && rgbOld == HIGH) {
    delay(20);

    if (rgb == LOW) {
   showType = 7;                            // Mix animation Type 7
     rgb = HIGH;

      startShow(showType);
    }
  }

  WhiteOld = White;
  RedOld = Red;
  BlueOld = Blue;
  GreenOld = Green;
  TopazOld = Topaz;
  LilacOld = Lilac;
  offOld = off;
  RainbowOld = Rainbow;
  rgbOld = rgb;
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


