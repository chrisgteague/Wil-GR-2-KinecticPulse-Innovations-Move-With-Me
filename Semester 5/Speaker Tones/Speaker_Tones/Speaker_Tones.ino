int Red = 6;
int Yellow = 5;
int Green = 4;
int touchInput = 2;
int inputState;
int lastInputState = 1;
int tapCount = 0;
unsigned long lastTapTime = 0;
int dt = 500;
int speakerPin = 9; //Change this to the PWM pin connected to the speaker

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(Green, OUTPUT);
  pinMode(Yellow,OUTPUT);
  pinMode(Red,OUTPUT);
  pinMode(touchInput, INPUT);
  pinMode(speakerPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  inputState = digitalRead(touchInput);
  if (lastInputState == LOW && inputState == HIGH) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastTapTime > 1000) {
      tapCount = 0;
    }
    lastTapTime = currentMillis;

    tapCount++;
    if(tapCount ==1){
      digitalWrite(Green, HIGH);
      digitalWrite(Yellow, LOW);
      digitalWrite(Red, LOW);
      tone(speakerPin, 200);
    }
    else if (tapCount == 2){
      digitalWrite(Green, LOW);
      digitalWrite(Yellow, HIGH);
      digitalWrite(Red, LOW);
      tone(speakerPin, 400);
      } 
      else if (tapCount == 3) {
        digitalWrite(Green, LOW);
        digitalWrite(Yellow, LOW);
        digitalWrite(Red, HIGH);
        tone(speakerPin, 600);
      }
      else if (tapCount == 4) {
        digitalWrite(Green, LOW);
        digitalWrite(Yellow, LOW);
        digitalWrite(Red, LOW);
        noTone(speakerPin);
      }
  }
  lastInputState = inputState;
}
