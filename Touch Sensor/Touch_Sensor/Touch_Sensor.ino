int LEDState=0;
int Red=6;
int Yellow=5;
int Green=4;
int touchInput=2;
int inputState;
int lastInputState=1;
int dt=500;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(Green, OUTPUT);
  pinMode(Yellow,OUTPUT);
  pinMode(Red,OUTPUT);
  pinMode(touchInput, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  inputState=digitalRead(touchInput);
  if(lastInputState==0 && inputState==1){
    if(LEDState==0){
      digitalWrite(Green,HIGH);
      delay(dt);
      digitalWrite(Yellow,HIGH);
      delay(dt);
      digitalWrite(Red,HIGH);
      delay(dt);
      LEDState=1;
    }
    else{
      digitalWrite(Green,LOW);
      delay(dt);
      digitalWrite(Yellow,LOW);
      delay(dt);
      digitalWrite(Red,LOW);
      LEDState=0;
    }
  }
  lastInputState=inputState;
  delay(dt);
}
