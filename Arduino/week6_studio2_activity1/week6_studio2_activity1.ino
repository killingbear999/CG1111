#define numLights 5

int arrLights[] = {A1,A2,A3,A4,A5};

void setup() {
  int i;
  for(i=0;i<=numLights;i=i+1) {
    pinMode(arrLights[i],OUTPUT);
  }
}

void loop() {
  int i;
  for(i=0;i<=numLights;i=i+1) {
  digitalWrite(arrLights[i], HIGH);
  delay(1000);
  digitalWrite(arrLights[i], LOW);
  delay(1000);
  }
}
 
