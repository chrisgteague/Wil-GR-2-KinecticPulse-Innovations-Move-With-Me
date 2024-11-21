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
#define PIXEL_COUNT 50   // Number of LEDs in the strip
#define BRIGHTNESS 7      // Use 96 for medium brightness
#define SPEED 50          // Speed of each Color Transition (in ms)
#define IMMEDIATELY 0
#define RAINBOW_SPEED  50 
//unsigned long previousMillis = 0;

// Thresholds
const int jumpThreshold = -5;
const int yjumpThreshold = 5;  

// const int jumpThreshold = 5;
// const int yjumpThreshold = -6;
//const int fallThreshold = -10;
//const int yThreshold = -12;

const int fallThreshold = -10;
const int yThreshold = 5;

//value storage
int jump = 0;
bool bigJump = false;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

bool RedOld = LOW;
bool GreenOld = LOW;
bool offOld = HIGH;
bool jumping = LOW;
int showType = 0;

//for parallel operaions
unsigned long previousMillis = 0;
int myVariable = 0;
const unsigned long interval = 5000; //5000 millis make 5 sec :)


void setup() {
  Serial.begin(115200);
  SerialBT.begin("Kinetic_Cape");  // Bluetooth device name
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
bool lighting = LOW;
char a;
char b;
char temp;
void startShow(int i) {
  switch(i){

    case 0: colorWipe(strip.Color(0, 0, 0), SPEED);    // Black/off
            break;

    case 3: strip.setBrightness(255);                            // Changes the Brightness to MAX
            colorWipe(strip.Color(255, 255, 255), IMMEDIATELY);  // White
            strip.setBrightness(BRIGHTNESS);                     // Reset the Brightness to Default value
            break;  

    case 1: colorWipe(strip.Color(255, 0, 0), SPEED);  // Red
            break;

    case 2: colorWipe(strip.Color(0, 255, 0), SPEED);  // Green
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

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*1; j++) { // 1 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      if (Serial.available() > 0) {
        // Read the input to clear the buffer
        char btInput = Serial.read();
        Serial.print("Interrupting due to input: ");
        Serial.println(btInput);

        // Break out of both loops
        return; 
      }
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
  bigJump = false;
  // Get accelerometer data
  sensors_event_t event;
  accel.getEvent(&event);

  // Read accelerometer values
  float xValue = event.acceleration.x;
  float yValue = event.acceleration.y;
  float zValue = event.acceleration.z;
  //parallelogram
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // Update the last time
    myVariable++; // Change the variable
    Serial.println("Variable updated: " + String(myVariable));
  }
  delay(200);
  bool offOld = LOW;
  bool WhiteOld = LOW;
  bool RedOld = LOW;
  bool GreenOld = LOW;
  bool BlueOld = LOW;
  bool TopazOld = LOW;
  bool LilacOld = LOW;
  bool RainbowOld = LOW;
  bool rgbOld = LOW;
  
  bool off = LOW;
  bool White = LOW;
  bool Blue = LOW;
  bool Red = LOW;
  bool Green = LOW;
  bool Topaz = LOW;
  bool Lilac = LOW;
  bool Rainbow = LOW;
  bool rgb = LOW;
  bool ende;//intersting
  temp = (char)SerialBT.read();
  
  if (isDigit(temp)){
     jump = temp;
     SerialBT.println("jump variable in if:");  //conflict cause both change to the most recent one   
     SerialBT.print(jump);
     jumping = HIGH; 
  }
  else if (isAlpha(temp)){
      a = temp;
      jumping = LOW;
      lighting = HIGH;
  }

    if(lighting==HIGH)
    {
    switch (a) {
        case 'r':
            Red = HIGH;
            SerialBT.println("CHANGING TO RED");
            Green = LOW;
            off = LOW;
            rgb = LOW;
            Rainbow = LOW;
            Topaz = LOW;
            Lilac = LOW;
            White = LOW;
            a = '0';
            lighting = LOW;
            break;
        case 'g':
            Green = HIGH;
            SerialBT.println("CHANGING TO GREEN");
            Red = LOW;
            off = LOW;
            rgb = LOW;
            Rainbow = LOW;
            Topaz = LOW;
            Lilac = LOW;
            White = LOW;
            a = '0';
            lighting = LOW;
            break;
        case 'o':
            off = HIGH;
            SerialBT.println("TURNING OFF LEDs..");
            Red = LOW;
            Green = LOW;
            rgb = LOW;
            Rainbow = LOW;
            Topaz = LOW;
            Lilac = LOW;
            White = LOW;
            a = '0';
            lighting = LOW;
            break;
        case 'a':
            Rainbow = HIGH;
            SerialBT.println("RAINBOW GO BRRRRR");
            Red = LOW;
            Green = LOW;
            rgb = LOW;
            off = LOW;
            Topaz = LOW;
            Lilac = LOW;
            White = LOW;
            a = '0';
            lighting = LOW;
            break;
        case 'm':
            rgb = HIGH;
            SerialBT.println("SEIZURE MODE");
            Red = LOW;
            Green = LOW;
            Rainbow = LOW;
            off = LOW;
            Topaz = LOW;
            Lilac = LOW;
            White = LOW;
            a = '0';
            lighting = LOW;
            break;
          case 'w':
            White = HIGH;
            SerialBT.println("Racist MODE");
            Red = LOW;
            Green = LOW;
            Rainbow = LOW;
            off = LOW;
            Topaz = LOW;
            Lilac = LOW;
            rgb = LOW;
            a = '0';
            lighting = LOW;
            break;
          case 't':
            Topaz = HIGH;
            SerialBT.println("Topaz MODE");
            Red = LOW;
            Green = LOW;
            Rainbow = LOW;
            off = LOW;
            rgb = LOW;
            Lilac = LOW;
            White = LOW;
            a = '0';
            lighting = LOW;
            break;
          case 'l':
            Lilac = HIGH;
            SerialBT.println("Lilac MODE");
            Red = LOW;
            Green = LOW;
            Rainbow = LOW;
            off = LOW;
            Topaz = LOW;
            rgb = LOW;
            White = LOW;
            a = '0';
            lighting = LOW;
            break;
        default:
            Red = LOW;
            Green = LOW;
            off = LOW;
            a = '0';
            break;
    }}

    
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
    if (Blue == HIGH && BlueOld == LOW) {
        delay(20);
        showType = 4;  // Off animation Type 0
        startShow(showType);
    }
    if (Rainbow == HIGH && RainbowOld == LOW) {
        delay(20);
        showType = 8;  // Green animation Type 2
        startShow(showType);
    }
    if (rgb == HIGH && rgbOld == LOW) {
        delay(20);
        showType = 7;  // Green animation Type 2
        startShow(showType);
    }
    if (Lilac == HIGH && LilacOld == LOW) {
        delay(20);
        showType = 6;  // Green animation Type 2
        startShow(showType);
    }
    if (Topaz == HIGH && TopazOld == LOW) {
        delay(20);
        showType = 5;  // Off animation Type 0
        startShow(showType);
    }
    if (White == HIGH && WhiteOld == LOW) {
        delay(20);
        showType = 3;  // Off animation Type 0
        startShow(showType);
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
  // Serial.print("X-axis: ");
  // Serial.println(xValue);
  // Serial.print("Y-axis: ");
  // Serial.println(yValue);
  // Serial.print("Z-axis: ");
  // Serial.println(zValue);
  // SerialBT.print("X-axis: ");
  // SerialBT.println(xValue);                           //for test values
  // SerialBT.println("Y-axis: ");
  // SerialBT.println(yValue);
  // SerialBT.print("Z-axis: ");
  // SerialBT.println(zValue);
  // SerialBT.println("---------------------------------");
  // delay(1000);

  if (jumping == HIGH)
  {
  switch (jump) {
        case '1':
          while(!bigJump){
          sensors_event_t event;
          accel.getEvent(&event);
          float xValue = event.acceleration.x;
          float yValue = event.acceleration.y;
          float zValue = event.acceleration.z;
          SerialBT.print("X-axis: ");
          SerialBT.println(xValue);
          SerialBT.print("Y-axis: ");
          SerialBT.println(yValue);
          SerialBT.print("Z-axis: ");
          SerialBT.println(zValue);
          SerialBT.println("---------------------------------");
          delay(1000);
  
            if (yValue < yThreshold) 
            {
              SerialBT.println("BIG CHANGE: yValue:");
              SerialBT.println(yValue); 
              SerialBT.println("Lateral-Left Movement Detected");
              Serial.println("Lateral Movement Detected");
              myDFPlayer.play(3);  // Play the third track on the DFPlayer Mini
              a = 'g';
              Green = HIGH;
              lighting = HIGH;
              bigJump = true;
              delay(2000);
              
            }}
            break;
        case '2':
          while(!bigJump){
          sensors_event_t event;
          accel.getEvent(&event);
          float xValue = event.acceleration.x;
          float yValue = event.acceleration.y;
          float zValue = event.acceleration.z;
          SerialBT.print("X-axis: ");          
          SerialBT.println(xValue);
          SerialBT.print("Y-axis: ");
          SerialBT.println(yValue);
          SerialBT.print("Z-axis: ");
          SerialBT.println(zValue);
          SerialBT.println("---------------------------------");
          delay(1000);
            if (zValue > jumpThreshold &&(yValue <= jumpThreshold || xValue <= jumpThreshold)) {
              SerialBT.println("BIG CHANGE: zValue:");
              SerialBT.println(zValue);
              SerialBT.println("Jump detected! Yahoo! ^^");
              Serial.println("Jump detected! Yahoo! ^^");
              myDFPlayer.play(2);  // Play the fourth track on the DFPlayer Mini
              a = 'g';
              Green = HIGH;
              lighting = HIGH;
              bigJump = true;
              delay(2000);
              b = 'g';
            }}
            break;
        case '3':
          while(!bigJump){
          sensors_event_t event;
          accel.getEvent(&event);
          float xValue = event.acceleration.x;
          float yValue = event.acceleration.y;
          float zValue = event.acceleration.z;
          SerialBT.print("X-axis: ");
          SerialBT.println(xValue);
          SerialBT.print("Y-axis: ");
          SerialBT.println(yValue);
          SerialBT.print("Z-axis: ");
          SerialBT.println(zValue);
          SerialBT.println("---------------------------------");
          delay(1000);
            if (yValue > yjumpThreshold) {
              SerialBT.println("BIG CHANGE: yValue:");
              SerialBT.println(yValue);
              SerialBT.println("Lateral- Right Movement Detected");
              Serial.println("Lateral- Right Movement Detected");
              myDFPlayer.play(1);  // Play the second track on the DFPlayer Mini
              a = 'g';
              Green = HIGH;
              lighting = HIGH;
              bigJump = true;
              jumping = HIGH;
              delay(2000);
              
            }
          }
            break;
        case '4':
          while(!bigJump){
          sensors_event_t event;
          accel.getEvent(&event);
          float xValue = event.acceleration.x;
          float yValue = event.acceleration.y;
          float zValue = event.acceleration.z;
          SerialBT.print("X-axis: ");          SerialBT.println(xValue);
          SerialBT.print("Y-axis: ");
          SerialBT.println(yValue);
          SerialBT.print("Z-axis: ");
          SerialBT.println(zValue);
          SerialBT.println("---------------------------------");
          delay(1000);
            if (xValue > jumpThreshold) {
              SerialBT.println("BIG CHANGE: xValue:");
              SerialBT.println(xValue); 
              SerialBT.println("Fowardsies? detected! Yahoo! ^^");
              Serial.println("Fowardsies? Yahoo! ^^");
              myDFPlayer.play(2);  // Play the fifth track on the DFPlayer Mini
              a = 'g';
              Green = HIGH;
              lighting = HIGH;
              bigJump = true;
              delay(2000);
              
            }
            if (xValue < fallThreshold) {
              
              SerialBT.println("BIG CHANGE: xValue:");
              SerialBT.println(xValue);    
              SerialBT.println("Backsies? detected! Yahoo! ^^");
              Serial.println("Backsies? Yahoo! ^^");
              myDFPlayer.play(2);  // Play the sixth track on the DFPlayer Mini
              a = 'g';
              Green = HIGH;
              lighting = HIGH;
              bigJump = true;
              delay(2000);
              
            }}
            break;
        default:
            SerialBT.println("Do jump please");
            SerialBT.println("BT Char:");
            SerialBT.println(jump);
            break;
  }}

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
  delay(20);  
}
}
