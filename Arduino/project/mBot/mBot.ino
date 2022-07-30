#include <Wire.h>
#include <MeMCore.h>
#include <MeRGBLed.h>
#include "Notes.h"
using ll = long long;

/********** Settings **********/
#define LEFTIR_PIN      A1
#define RIGHTIR_PIN     A2
#define SNDLOW_PIN      A3                      // 100-300
#define SNDHI_PIN       A0                      // >3000
#define LDR_PIN         A6
#define LED_PIN         7
#define MUSIC_PIN       8
MeDCMotor               leftWheel(M1);          // -255 to 255
MeDCMotor               rightWheel(M2);         // 255 to -255, ie reversed
MeLineFollower          lineFinder(PORT_2);     // PORT_3
MeUltrasonicSensor      ultraSensor(PORT_1);    // PORT_7
MeSoundSensor           highSound(PORT_6);
MeSoundSensor           lowSound(PORT_5);
MeRGBLed                led(LED_PIN);
MeBuzzer                buzzer;

// General
#define WAYPTDELAY      100                     // delay b4 decoding challenge
#define TIMEDELAY       20                      // delay b4 recheck position

// Movement
#define MOTORSPEED      220
#define TIMETURN        (72500/MOTORSPEED)      // time for 90deg turn
#define TIMEGRID        (180000/MOTORSPEED)     // time to travel 1 grid
#define DELAYGRID       (TIMEGRID / TIMEDELAY)
#define K_ERR           0.5
#define K_DIST          (255/2)                 // max correction to mvmt
#define LEFT_BIAS       0                       // 128
#define FRONT_BIAS      40                      // cm
#define K_LEFTIR        200                     // threshold for IR sensor values
#define K_RIGHTIR       200
#define TIMEMUL         5                       // time multiplier for IR adjustment

// Sound
#define K_SNDLOW        35                     // 80
#define K_SNDHI         8                     // 75

// Color
// retrieved from colourcal.ino file after calibration
#define MIN_DIST        5000                   // 10000
#define WHI_VAL         {375, 335, 380}         // from LDR b4 normalisation
//#define BLA_VAL         {318, 276, 313}         // {315, 265, 305}
//#define GRE_VAL         {102, 93, 109}          // {60,70,75}
#define BLA_VAL         {255, 217, 243}
#define GRE_VAL         {116, 108, 130}

#define RED_ARR         {185,35,35}             // normalised rgb vals
#define GRE_ARR         {45, 100, 60}
#define YEL_ARR         {255, 175, 100}
#define PUR_ARR         {115, 110, 175}
#define BLU_ARR         {140, 200, 230}
#define BLA_ARR         {0,0,0}
#define NUMCOL          6                       // black, red, green, yellow, purple, blue

// Calibration
#define CALLIBRATE_SEC  3                       // delay b4 calibration
#define CALIBRATE_NO    50                      // no of measurements
#define IR_WAIT         100                     // delay btw measurements. IMPT!
#define RGB_WAIT        200
#define LDR_WAIT        10
#define MIC_WAIT        100
#define LED_MAX         255
#define IR_MAX          1023



/********** Global Variables **********/
bool busy = true;

int IR_VALUES[2][2] = {{68,10},{81,19}}; // left-right, minmax
ll error = 0;

int blackArray[] = BLA_VAL;
int greyDiff[] = GRE_VAL;
static int allColourArray[6][3] = {BLA_ARR,RED_ARR,GRE_ARR,YEL_ARR,PUR_ARR,BLU_ARR};
  


/********** Main Program **********/
void setup() {
  pinMode(LEFTIR_PIN, INPUT);
  pinMode(RIGHTIR_PIN, INPUT);
  pinMode(SNDLOW_PIN, INPUT);
  pinMode(SNDHI_PIN, INPUT);
  Serial.begin(9600);

//   calibrateWB();
  // calibrateIR();
  // colorSensor.SensorInit();
  busy = false;
}

void stopMove(int i);
void loop() {
//  Serial.println(ultraSensor.distanceCm()); 
//  delay(500);
//  return;
//  forwardGrid();
//  return;
//  int right=analogRead(RIGHTIR_PIN),left=analogRead(LEFTIR_PIN);;
//  Serial.print("Right: "); Serial.println(right);
//  Serial.print("Left: "); Serial.println(left);
//  delay(500);
//  return;
  if (busy) return;

  // double dist = ultraSensor.distanceCm();
  // Serial.println(dist);
//   Serial.print("LOW: "); Serial.print(analogRead(SNDLOW_PIN)); //delay(MIC_WAIT);
//   Serial.print(" HIGH: "); Serial.println(analogRead(SNDHI_PIN)); delay(MIC_WAIT);
//  printColour(getColour());
//  Serial.println(getDist());
//  delay(100);
//  return;

  // double frontDistance = ultraSensor.distanceCm();
  if (lineFinder.readSensors() != S1_IN_S2_IN) { // both sensors not in black line
    moveForward(); // todo: front
    return;
  }
  
  // Waypoint detected!
  busy = true;
  stopMove(0);
  delay(WAYPTDELAY);

  // Color Challenge
  int colourRes;
  do {
    colourRes = getColour();
    printColour(colourRes);
  } while (colourRes == -1);
  if (colourRes > 0) { // is color challenge (not black)
    colorWaypoint(colourRes);
    busy = false;
    return;
  }

  // Sound Challenge
  int soundRes = getSound();
  if (soundRes > 0) {
    soundWaypoint(soundRes);
    busy = false;
    return;
  }

  // Finished!
  finishWaypoint();
}



/********** Movement **********/
void stopMove(int i = 10) {
  rightWheel.stop();
  leftWheel.stop();
  if (i) delay(TIMEDELAY * i);
}

void moveForward() {
  int dx = getDist();
    
  // Normalise to MOTORSPEED
  int maxx = MOTORSPEED + (dx >= 0 ? dx : -dx);
  leftWheel.run((ll)(-MOTORSPEED + dx) * MOTORSPEED / maxx);
  rightWheel.run((ll)(MOTORSPEED + dx) * MOTORSPEED / maxx);
    
  delay(TIMEDELAY*TIMEMUL);
  stopMove(0);
}

void forward() {
  leftWheel.run(-MOTORSPEED);
  rightWheel.run(MOTORSPEED);
  delay(TIMEDELAY);
}

void forwardGrid() {
  for (int i = 0; i < DELAYGRID; ++i) { 
    if (ultraSensor.distanceCm() < 7) break;
    forward();
  }
  stopMove();
}

void turnRight() {
  leftWheel.run(-MOTORSPEED);
  rightWheel.run(-MOTORSPEED);
  delay(TIMETURN);
  stopMove();
}

void turnLeft() {
  leftWheel.run(MOTORSPEED);
  rightWheel.run(MOTORSPEED);
  delay(TIMETURN);
  stopMove();
}

void doubleRight() {
  turnRight();
  forwardGrid();
  turnRight();
}

void doubleLeft() {
  turnLeft();
  forwardGrid();
  turnLeft();
}

void uTurn() {
  turnRight();
  turnRight();
}



/********** Sensors **********/
int getDist() {
//  
//   // METHOD 1
//   // Sweep left/right to find avail space
//   if (ultraSensor.distanceCm() > FRONT_BIAS) return 0;
//
//   // Jiggle and see left/right better
//   stopMove(0); busy = true;
//   unsigned long time = millis() + TIMETURN / 4;
//
//   // Jiggle Right
//   while (millis() < time || ultraSensor.distanceCm() < FRONT_BIAS) {
//     leftWheel.run(-MOTORSPEED); rightWheel.run(MOTORSPEED);
//   }
//   stopMove(0);
//   if (ultraSensor.distanceCm() > FRONT_BIAS) {
//     busy = false; return 0;
//   }
//
//   time = millis() + TIMETURN / 2;
//   // Jiggle Right
//   while (millis() < time || ultraSensor.distanceCm() < FRONT_BIAS) {
//     leftWheel.run(MOTORSPEED); rightWheel.run(-MOTORSPEED);
//   };
//   stopMove(0); busy = false;
//   return 0;
 
  // METHOD 2
  // Take raw value and threshold
  int ir = analogRead(LEFTIR_PIN);
  if (ir < K_LEFTIR) { // turn right
    return (ir - K_LEFTIR) * K_DIST / 100;
  }
  else if (ir = analogRead(RIGHTIR_PIN), ir < K_RIGHTIR) { // turn left
    return (K_RIGHTIR - ir) * K_DIST / 100;
  }
  return 0;

/*
  // METHOD 3
  // Calibrate, normalise and bias
  int left = analogRead(LEFTIR_PIN), right = analogRead(RIGHTIR_PIN);
  left = norm(left, IR_VALUES[0]) - LEFT_BIAS;
  right = norm(right, IR_VALUES[1]);
  // Only care if too close to either side
  Serial.print("LEFT: "); Serial.print(left);
  Serial.print("\tRIGHT: "); Serial.println(right);
  if (left > 877) left = 2; // so we know it's not 1
  if (right > 877) right = 2;
  // TODO: Check front
  // Ultrasonic
  ll curr = (ll)(left - right) * K_DIST / 1023;
  // return curr;
  curr -= error;
  error += curr;
  return curr + error * K_ERR;
  */
}

// Sound Sensor: Return first hz
int getSound() {
  for (int i = 0; i < CALIBRATE_NO; ++i) {
    int low = analogRead(SNDLOW_PIN);
    int hi = analogRead(SNDHI_PIN);
    if (low > K_SNDLOW) return 1;
    else if (hi > K_SNDHI) return 2;
    delay(MIC_WAIT);
  }
  return 0;
}

// Color Sensor: Return nearest colour
ll square(ll x) { return x * x; }
int getColour() { // returns index of best color
  float colourArray[3] = {0};
  for (int i = 0; i < 3; ++i) {
    led.setColor( // one-hot encoding
      ((1<<i)   &1) * LED_MAX,
      ((1<<i>>1)&1) * LED_MAX,
      ((1<<i>>2)&1) * LED_MAX
    ); led.show();
    delay(RGB_WAIT);

    for (int j = 0; j < CALIBRATE_NO; ++j) {
      colourArray[i] += analogRead(LDR_PIN);
      delay(LDR_WAIT);
    }
    colourArray[i] /= CALIBRATE_NO;
    colourArray[i] = (colourArray[i] - blackArray[i]) * 255 / greyDiff[i];
    Serial.println(colourArray[i]);
  }
  led.setColor(0,0,0); led.show();

  int idx = -1, min_dist = MIN_DIST;
  for (int i = 0; i < 6; ++i) {
    long long curr_dist = 0;
    for (int j = 0; j < 3; ++j) {
      curr_dist += square(allColourArray[i][j] - colourArray[j]);
    }
//    Serial.print(i); Serial.print(curr_dist); Serial.print(", ");
    if (min_dist > curr_dist && curr_dist > 0) {
      idx = i; min_dist = curr_dist;
    }
  }
  Serial.println();

  return idx;
}

// void getColours(uint16_t colorvalues[]) {
//   colorvalues[0] = colorSensor.Returnresult();
//   colorvalues[1] = colorSensor.ReturnRedData();
//   colorvalues[2] = colorSensor.ReturnGreenData();
//   colorvalues[3] = colorSensor.ReturnBlueData();
//   colorvalues[4] = colorSensor.ReturnColorData();
// }

// void colorPrint() {
//   uint16_t colorvalues[5];
//   getColours(colorvalues);
//   long colorcode = colorSensor.ReturnColorCode();//RGB24code
//   uint8_t grayscale = colorSensor.ReturnGrayscale();

//   Serial.print("R:"); Serial.print(colorvalues[1]); Serial.print("\t");
//   Serial.print("G:"); Serial.print(colorvalues[2]); Serial.print("\t");
//   Serial.print("B:"); Serial.print(colorvalues[3]); Serial.print("\t");
//   Serial.print("C:"); Serial.print(colorvalues[4]); Serial.print("\t");
//   Serial.print("color:");
//   switch (colorvalues[0])
//   {
//     case BLACK: Serial.print("BLACK"); break;
//     case BLUE: Serial.print("BLUE"); break;
//     case YELLOW: Serial.print("YELLOW"); break;
//     case GREEN: Serial.print("GREEN"); break;
//     case RED: Serial.print("RED"); break;
//     case WHITE: Serial.print("WHITE"); break;
//   }
//   Serial.print("\t");
//   Serial.print("code:"); Serial.print(colorcode, HEX); Serial.print("\t");
//   Serial.print("grayscale:"); Serial.println(grayscale);
// }



/********** Waypoints **********/
void colorWaypoint(int colourRes) {
  // red : left
  // green : right
  // yellow : 180deg within grid
  // purple : 2 left
  // light blue : 2 right
//  {BLA_ARR,RED_ARR,GRE_ARR,YEL_ARR,PUR_ARR,BLU_ARR}
  switch (colourRes) {
    case 1: turnLeft(); break;
    case 2: turnRight(); break;
    case 3: uTurn(); break;
    case 4: doubleLeft(); break;
    case 5: doubleRight(); break;
  }
}

void soundWaypoint(int soundRes) {
  // 1  low (100-300) : left
  // 2  right (>3000) : right
  switch (soundRes) {
    case 1: turnLeft(); break;
    case 2: turnRight(); break;
  }
}

void finishWaypoint() {
  // stop moving, play sound
  for (int i = 0; i < sizeof(music_key) / sizeof(int); ++i) {
    // quarter note = 1000 / 4, eighth note = 1000/8, etc. (Assuming 1 beat per sec)
    int duration = 1000 / music_duration[i];
    buzzer.tone(MUSIC_PIN, music_key[i], duration);

    // to distinguish notes
    delay(duration * 1.30);
    buzzer.noTone(MUSIC_PIN);
  }
}



/********** Calibration **********/
int norm(const int val, const int *low_mult) {
  int tmp = 1023LL * (val - *low_mult) / *(low_mult+1); // int will overflow
  if (tmp < 0) tmp = 1;
  else if (tmp > 1023) tmp = 1023;
  return tmp;
}

void calibrateIR() {
  Serial.println("===== CALIBRATING IR SENSORS (SIDE) =====");

  // Min values
  Serial.print("COVER SENSORS. Calibrating MIN in ");
  for (int i = CALLIBRATE_SEC; i > 0; --i) {
    Serial.print(i); Serial.print(".. "); delay(1000);
  }
  IR_VALUES[0][0] = IR_VALUES[1][0] = 0;
  for (int i = 0; i < CALIBRATE_NO; ++i) {
    IR_VALUES[0][0] += analogRead(LEFTIR_PIN);
    IR_VALUES[1][0] += analogRead(RIGHTIR_PIN);
    delay(IR_WAIT);
  }
  IR_VALUES[0][0] /= CALIBRATE_NO;
  IR_VALUES[1][0] /= CALIBRATE_NO;
  Serial.println("done.");

  // Max values
  Serial.print("UNCOVER SENSORS. Calibrating MAX in ");
  for (int i = CALLIBRATE_SEC; i > 0; --i) {
    Serial.print(i); Serial.print(".. "); delay(1000);
  }
  IR_VALUES[0][1] = IR_VALUES[1][1] = 0;
  for (int i = 0; i < CALIBRATE_NO; ++i) {
    IR_VALUES[0][1] += analogRead(LEFTIR_PIN);
    IR_VALUES[1][1] += analogRead(RIGHTIR_PIN);
    delay(IR_WAIT);
  }
  IR_VALUES[0][1] /= CALIBRATE_NO;
  IR_VALUES[1][1] /= CALIBRATE_NO;
  Serial.println("done.\n");

  // Save range
  // Serial.println(IR_VALUES[0][0]);
  // Serial.println(IR_VALUES[0][1]);
  // Serial.println(IR_VALUES[1][0]);
  // Serial.println(IR_VALUES[1][1]);
  IR_VALUES[0][1] -= IR_VALUES[0][0]; // left range
  IR_VALUES[1][1] -= IR_VALUES[1][0]; // right range

  // Output calibrated
  Serial.print("int IR_VALUES[2][2] = {{");
  Serial.print(IR_VALUES[0][0]); Serial.print(",");
  Serial.print(IR_VALUES[0][1]); Serial.print("},{");
  Serial.print(IR_VALUES[1][0]); Serial.print(",");
  Serial.print(IR_VALUES[1][1]); Serial.println("}};");
}

void calibrateWB() {
  Serial.println("===== CALIBRATING COLOR SENSORS (TOP) =====");
  int whiteArray[3] = {0};

  // Max values
  Serial.print("Put WHITE sample. Calibrating MAX in ");
  for (int i = CALLIBRATE_SEC; i > 0; --i) {
    Serial.print(i); Serial.print(".. "); delay(1000);
  }
  for (int i = 0; i < 3; ++i) {
    led.setColor( // one-hot encoding
      ((1<<i)   &1) * LED_MAX,
      ((1<<i>>1)&1) * LED_MAX,
      ((1<<i>>2)&1) * LED_MAX
    ); led.show();
    delay(RGB_WAIT);

    for (int j = 0; j < CALIBRATE_NO; ++j) {
      whiteArray[i] += analogRead(LDR_PIN);
      delay(LDR_WAIT);
    }
    whiteArray[i] /= CALIBRATE_NO;
  }
  led.setColor(0,0,0); led.show();
  Serial.println("done.");

  // Min values
  Serial.print("Put BLACK sample. Calibrating MIN in ");
  for (int i = CALLIBRATE_SEC; i > 0; --i) {
    Serial.print(i); Serial.print(".. "); delay(1000);
  }
  for (int i = 0; i < 3; ++i) {
    led.setColor( // one-hot encoding
      ((1<<i)   &1) * LED_MAX,
      ((1<<i>>1)&1) * LED_MAX,
      ((1<<i>>2)&1) * LED_MAX
    ); led.show();
    delay(RGB_WAIT);
    
    blackArray[i] = 0;
    for (int j = 0; j < CALIBRATE_NO; ++j) {
      blackArray[i] += analogRead(LDR_PIN);
      delay(LDR_WAIT);
    }
    blackArray[i] /= CALIBRATE_NO;
    greyDiff[i] = whiteArray[i] - blackArray[i];
  }
  led.setColor(0,0,0); led.show();
  Serial.println("done.");

  // Output calibrated
  Serial.print("#define BLA_VAL         {");
  Serial.print(blackArray[0]); Serial.print(", ");
  Serial.print(blackArray[1]); Serial.print(", ");
  Serial.print(blackArray[2]); Serial.println("}");
  Serial.print("#define GRE_VAL         {");
  Serial.print(greyDiff[0]); Serial.print(", ");
  Serial.print(greyDiff[1]); Serial.print(", ");
  Serial.print(greyDiff[2]); Serial.println("}");
}
