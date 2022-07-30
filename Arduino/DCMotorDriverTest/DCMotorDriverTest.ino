/**
 * \par Copyright (C), 2012-2016, MakeBlock
 * @file    DCMotorDriverTest.ino
 * @author  MakeBlock
 * @version V1.0.0
 * @date    2015/09/09
 * @brief   Description: this file is sample code for Me DC motor device.
 *
 * Function List:
 *    1. void MeDCMotor::run(int16_t speed)
 *    2. void MeDCMotor::stop(void)
 *
 * \par History:
 * <pre>
 * <Author>     <Time>        <Version>      <Descr>
 * Mark Yan     2015/09/09    1.0.0          rebuild the old lib
 * </pre>
 */

// To be determined:
// TIMEDELAY
// TIMETURN
// TIMEGRID

// delay for how often position is checked
// should be a factor of TIMEGRID
#define TIMEDELAY 500 
#define TIMETURN 1100 // time needed for mBot to turn 90 deg
#define TIMEGRID 2000 // time needed to travel 1 grid
 
#include "MeMCore.h"

MeDCMotor leftWheel(M1);

MeDCMotor rightWheel(M2);

uint8_t motorSpeed = 100;
// no. of TIMEDELAY cycles to travel 1 grid
int delayGrid = TIMEGRID/TIMEDELAY; 


void setup()
{
}

void loop()
{
  forwardGrid();
  turnRight();
  forwardGrid();
  turnLeft();
  forwardGrid();
  uTurn();
 
}

void forward() {
  leftWheel.run(-motorSpeed);
  rightWheel.run(+motorSpeed);
  delay(TIMEDELAY);
}

void forwardGrid() {
  for (int i=0; i<delayGrid;i++){
    forward();
  }
}

void turnRight() {
  leftWheel.run(-motorSpeed);
  rightWheel.run(-motorSpeed);
  delay(TIMETURN);
}

void turnLeft() {
  leftWheel.run(motorSpeed);
  rightWheel.run(motorSpeed);
  delay(TIMETURN);
}

void uTurn() {
  turnRight();
  turnRight();
}
