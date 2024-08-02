#define BLYNK_TEMPLATE_ID           "TMPL2wzr_En22"
#define BLYNK_TEMPLATE_NAME         "Quickstart Device"
#define BLYNK_AUTH_TOKEN            "6KeSrHMi7OO9gGKAo9weDzmnmivq9IgA"

#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

const char* ssid = "uFi_F8F0AA";      // SSID (network name)
const char* password = "bubblegum";    // Password for the AP

#define LED_PIN    15       // Digital pin connected to the LED strip
#define LED_COUNT  144      // Number of LEDs in the strip 

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

BlynkTimer timer;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update LED strip color based on the switch state
  if (value == 1) {
    // Turn on the LED strip with green color
    setColor(strip.Color(0, 255, 0));  // Green color

  }
  else if (value == 2) {
    // Turn on the LED strip with green color
    setColor(strip.Color(255, 0, 0));  // Green color
  }
  else if (value == 3) {
    // Turn on the LED strip with green color
    setColor(strip.Color(0, 0, 255));  // Green color
  } 
  else {
    // Turn off the LED strip
    setColor(strip.Color(0, 0, 0));  // Turn off LEDs
  }
}

// Function to set the color of the entire LED strip
void setColor(uint32_t color) {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, color);
  }
  strip.show(); // Update the LED strip with the new color
}

void setup() {
  Serial.begin(115200);
  strip.begin();     // Initialize the LED strip
  strip.show();      // Turn off all LEDs initially
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password, "blynk.cloud", 80);
  timer.setInterval(1000L, []() { Blynk.virtualWrite(V2, millis() / 1000); });  // Send uptime to Virtual Pin 2 every second
}

void loop() {
  Blynk.run();
  timer.run();
  if (Serial.available()) {
    String message = Serial.readStringUntil('\n');
    Serial.println("Received from Arduino Mega: " + message);
    }

  // Example of sending data to Arduino Mega
  delay(1000); // Send data every second
  //Serial.println("Hello from ESP32");
}