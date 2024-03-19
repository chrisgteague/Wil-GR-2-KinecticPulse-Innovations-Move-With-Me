
//--------------------------------------------------------------
#define xPin A2
#define yPin A3
#define zPin A4
//--------------------------------------------------------------


// Take multiple samples to reduce noise
const int samples = 10;
//--------------------------------------------------------------

void setup() 
{
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  pinMode(3 , OUTPUT);
  pinMode(9 , OUTPUT);
}


void loop() 
{
  //--------------------------------------------------------------
  //Read raw values
  int xRaw=0,yRaw=0,zRaw=0;
  for(int i=0;i<samples;i++) //this constantly loops through and reads the raw values recieved from the sensor
  {
    xRaw+=analogRead(xPin);
    yRaw+=analogRead(yPin);
    zRaw+=analogRead(zPin);
  }
  xRaw/=samples;
  yRaw/=samples;
  zRaw/=samples;
  //--------------------------------------------------------------
  Serial.print(xRaw);  //This prints to the serial monitor which you can access by going to "Tools > Serial Monitor"
  Serial.print("\t");
  Serial.print(yRaw);
  Serial.print("\t");
  Serial.print(zRaw);
  Serial.println();
  //--------------------------------------------------------------
  delay(200);

//For this i have only used the y axis for turning on the lights

  if(yRaw < 420) //This turns on the red light when the y axis is less than 420
  {
    digitalWrite(2 ,HIGH);
    delay(200);
    digitalWrite(3 ,LOW);
    delay(200);
    digitalWrite(9 ,LOW);
    delay(200);
  }
  if(yRaw > 421&& yRaw <599)//This turns on the yellow light when the y axis is greater than 421 and less than 599
  {
    
    digitalWrite(2 ,LOW);
    delay(200);
    digitalWrite(9 ,LOW);
    delay(200);
    digitalWrite(3,HIGH);
    delay(200);
  }
  if(yRaw > 600)//This turns on the green light when the y axis is greater than 600
  {
    digitalWrite(9 ,HIGH);
    delay(200);
    digitalWrite(2 ,LOW);
    delay(200);
    digitalWrite(3 ,LOW);
    delay(200);
  }
}
