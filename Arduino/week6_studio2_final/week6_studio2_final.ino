#define numPin 7
#define numLights 5

int arrLights[] = {A1, A2, A3, A4, A5};
int arrPin[] = {2, 3, 5, 6, 8, 9, 10};
int arrSeg[] = {1, 2, 5, 6, 8, 9, 10};

int adc_val, numDisp = 1, i;


void setup() {
  Serial.begin(9600);
  for (i = 0; i <= numPin; i = i + 1) {
    pinMode(arrPin[i], OUTPUT);
  }

  for (i = 0; i <= numLights; i = i + 1) {
    pinMode(arrLights[i], OUTPUT);
  }
}

void loop() {
  Serial.println(adc_val);

  for (numDisp = 1; numDisp <= 9; numDisp = numDisp + 1) {
    for (i = 0; i <= 4; i = i + 1) {
      adc_val = analogRead(A0);
      sseg_display(numDisp);
      delay(adc_val + 50);
      digitalWrite(arrLights[i], HIGH);
      delay(adc_val + 50);
      digitalWrite(arrLights[i], LOW);
    }

    for (i = 3; i > 0; i = i - 1) {
      adc_val = analogRead(A0);
      sseg_display(numDisp);
      delay(adc_val + 50);
      digitalWrite(arrLights[i], HIGH);
      delay(adc_val + 50);
      digitalWrite(arrLights[i], LOW);
    }
  }
}

void sseg_display(char num) {
  switch (num) {
    case 1:
      digitalWrite(segToPin(10), LOW);
      digitalWrite(segToPin(8), LOW);
      digitalWrite(segToPin(1), HIGH);
      digitalWrite(segToPin(2), HIGH);
      digitalWrite(segToPin(5), HIGH);
      digitalWrite(segToPin(6), HIGH);
      digitalWrite(segToPin(9), HIGH);
      break;
    case 2:
      digitalWrite(segToPin(1), LOW);
      digitalWrite(segToPin(10), LOW);
      digitalWrite(segToPin(9), LOW);
      digitalWrite(segToPin(5), LOW);
      digitalWrite(segToPin(6), LOW);
      digitalWrite(segToPin(2), HIGH);
      digitalWrite(segToPin(8), HIGH);
      break;
    case 3:
      digitalWrite(segToPin(1), LOW);
      digitalWrite(segToPin(10), LOW);
      digitalWrite(segToPin(9), LOW);
      digitalWrite(segToPin(8), LOW);
      digitalWrite(segToPin(6), LOW);
      digitalWrite(segToPin(2), HIGH);
      digitalWrite(segToPin(5), HIGH);
      break;
    case 4:
      digitalWrite(segToPin(2), LOW);
      digitalWrite(segToPin(9), LOW);
      digitalWrite(segToPin(10), LOW);
      digitalWrite(segToPin(8), LOW);
      digitalWrite(segToPin(1), HIGH);
      digitalWrite(segToPin(5), HIGH);
      digitalWrite(segToPin(6), HIGH);
      break;
    case 5:
      digitalWrite(segToPin(1), LOW);
      digitalWrite(segToPin(2), LOW);
      digitalWrite(segToPin(9), LOW);
      digitalWrite(segToPin(8), LOW);
      digitalWrite(segToPin(6), LOW);
      digitalWrite(segToPin(5), HIGH);
      digitalWrite(segToPin(10), HIGH);
      break;
    case 6:
      digitalWrite(segToPin(2), LOW);
      digitalWrite(segToPin(5), LOW);
      digitalWrite(segToPin(6), LOW);
      digitalWrite(segToPin(8), LOW);
      digitalWrite(segToPin(9), LOW);
      digitalWrite(segToPin(1), LOW);
      digitalWrite(segToPin(10), HIGH);
      break;
    case 7:
      digitalWrite(segToPin(1), LOW);
      digitalWrite(segToPin(10), LOW);
      digitalWrite(segToPin(8), LOW);
      digitalWrite(segToPin(2), HIGH);
      digitalWrite(segToPin(5), HIGH);
      digitalWrite(segToPin(6), HIGH);
      digitalWrite(segToPin(9), HIGH);
      break;
    case 8:
      digitalWrite(segToPin(1), LOW);
      digitalWrite(segToPin(2), LOW);
      digitalWrite(segToPin(5), LOW);
      digitalWrite(segToPin(6), LOW);
      digitalWrite(segToPin(8), LOW);
      digitalWrite(segToPin(9), LOW);
      digitalWrite(segToPin(10), LOW);
      break;
    case 9:
      digitalWrite(segToPin(9), LOW);
      digitalWrite(segToPin(2), LOW);
      digitalWrite(segToPin(1), LOW);
      digitalWrite(segToPin(10), LOW);
      digitalWrite(segToPin(8), LOW);
      digitalWrite(segToPin(5), HIGH);
      digitalWrite(segToPin(6), LOW);
      break;
  }
}

int segToPin(int num) {
  int i;
  for (i = 0; i < numPin; i = i + 1) {
    if (arrSeg[i] == num) {
      return arrPin[i];
    }
  }
}
