#define SAMPLE 10
#define THRESHOLDLOW 40
#define THRESHOLDHIGH 10

int i;
double /*valueLow[SAMPLE], valueHigh[SAMPLE], */valueLow, valueHigh;

void setup() {
  pinMode(A3, INPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  Serial.begin(9600);
}

void loop() {
  /*for(i=0;i<SAMPLE;i=i+1) {
    valueLow[i] = analogRead(A3)/204.6;
    delay(100);
    valueHigh[i] = analogRead(A0)/204.6;
    delay(100);
  }

  for(i=0;i<SAMPLE;i=i+1) {
    if(valueLow[i]>THRESHOLDLOW) {
      turnLeft();
    } else if(valueHigh[i]>THRESHOLDHIGH) {
      turnRight();
    }
  }*/
  valueLow= analogRead(A1);
  delay(100);
  valueHigh = analogRead(A2);
  delay(100);
  Serial.print(valueLow);
  Serial.print(",");
  Serial.println(valueHigh);
}
