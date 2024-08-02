
// The dark variable determines when we turn the LEDs on or off. 
// Set higher or lower to adjust sensitivity.
const int darkLevel = 50;

// Create a variable to hold the readings from the light sensor.
int lightValue;

// Set which pin the Signal output from the light sensor is connected to
int sensorPin = A4;

// Set which pin the LED is connected to. 
int ledPin = 2;

void setup()
{
    // Set sensorPin as an INPUT
    pinMode(sensorPin, INPUT);

    // Set LED as outputs
    pinMode(ledPin, OUTPUT);

    // Set pin A5 to use as a power pin for the light sensor
    pinMode(A5, OUTPUT);
    digitalWrite(A5, HIGH);

    // Initialize Serial, set the baud rate to 9600 bps.
    Serial.begin(9600);
}

void loop()
{
    // Read the light sensor's value and store in 'lightValue'
    lightValue = analogRead(sensorPin);

    // Print some descriptive text and then the value from the sensor
    Serial.print("Light value is:");
    Serial.println(lightValue);

    // Compare "lightValue" to the "dark" variable
    if (darkLevel <= lightValue) // If the reading is more then 'darkLevel'
    {
        digitalWrite(ledPin, HIGH); // Turn on LED
    }
    else // Otherwise, if "lightValue" is less than "dark"
    {
        digitalWrite(ledPin, LOW);  // Turn off LED
    }


    delay(100);
}
