#define xPin A1
#define yPin A2
#define zPin A3

const int samples = 10;

int previousXRaw = 0;
int previousYRaw = 0;
int previousZRaw = 0;
  
void setup() {
}

void loop() {

 int xRaw=0;
 int zRaw=0;
 int yRaw=0;

 for (int i=0;i<samples;i++) 
 {
  xRaw+=analogRead(xPin);
  zRaw+=analogRead(zPin);
  yRaw+=analogRead(yPin);
 }
  xRaw/=samples; 
  yRaw/=samples;
  zRaw/=samples;

if (xRaw != previousXRaw){  // if axis reading is new 
  if (xRaw < 450){ //if over threshold display
    Serial.println("Tipped over x");  
  }
  }
  previousXRaw = xRaw; //set previousxraw
}
if (yRaw != previousYRaw){
  if (yRaw < 450 ){
    Serial.println("Tipped over y");
  }
  }
  previousYRaw = yRaw;
}
if (zRaw != previousZRaw){
  if (zRaw < 450){
    Serial.println("Tipped over z");
  }
}
  previousZRaw = zRaw;
}
delay(1000);
}



