#define INPUTPIN A0
#define PERIOD 200
#define SAMPLES 50
int set_number=1, i=0, input[SAMPLES]={0}, millis_ref, j=0;

void setup() {
  Serial.begin(9600);
  millis_ref = millis() - PERIOD;
}

void loop() {
  while(i<SAMPLES) {
    if(millis() - millis_ref >= PERIOD) {
      input[i] = analogRead(INPUTPIN);
      millis_ref += PERIOD;
      ++i;
    }
  }

  Serial.print("Sampling Results: Set ");
  Serial.println(set_number);

  for(j=0;j<SAMPLES;j++) {
    Serial.println(input[j]);
  }
}
