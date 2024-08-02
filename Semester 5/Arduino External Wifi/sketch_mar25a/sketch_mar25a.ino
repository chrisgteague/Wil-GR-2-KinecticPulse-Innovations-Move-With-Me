//This is for the Arduino mega

#include <SoftwareSerial.h>

SoftwareSerial espSerial(18, 19); // RX, TX

void setup() {
 Serial.begin(9600); // Start serial communication with Arduino Serial Monitor
 espSerial.begin(115200); // Start serial communication with ESP-01S module
  
 delay(1000); // Delay to let the module settle
  
 Serial.println("Initializing ESP8266...");
 espSerial.println("AT+RST"); // Reset ESP-01S
 delay(2000);
  
 Serial.println("Configuring ESP8266 as Access Point...");
 espSerial.println("AT+CWMODE=2"); // Set ESP-01S to Access Point mode
 delay(1000);
  
 espSerial.println("AT+CWSAP=\"ArdChris\",\"1234567890\",1,3"); // Set up Access Point with SSID, Password, Channel, and Encryption
 delay(1000);
}

void loop() {
 // Send AT+CWLIF command to list all connected clients
 espSerial.println("AT+CWLIF");
 delay(2000); // Increase delay to ensure we get the full response

 // Check if there's any data available to read
 if (espSerial.available()) {
    String response = "";
    while (espSerial.available()) {
      response += (char)espSerial.read(); // Read the response
    }
    // Check if the response contains "+CWLIF:" followed by an IP address
    if (response.indexOf("+CWLIF:") != -1 && response.indexOf(".") != -1) {
      Serial.println("A device has connected to the Access Point");
    }
 }

 delay(1000); // Wait for 1 second before checking again
}
