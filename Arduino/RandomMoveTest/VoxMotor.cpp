#include "Arduino.h"
#include "VoxMotor.h"

#define PWM_MAX_DUTY 0.8

VoxMotor::VoxMotor(){
  pin[0] = pin[1] = 0;
  limit[0] = limit[1] = 0;
  currentDirection = 0;
  currentDutyCycle = 0.0;
}

void VoxMotor::setup(int motor0, int motor1, int switch0, int switch1){
  // assume pwm'ing pin[0] will trigger limit[0]
  pin[0] = motor0;
  pin[1] = motor1;
  limit[0] = switch0;
  limit[1] = switch1;

  pinMode(pin[0], OUTPUT);
  pinMode(pin[1], OUTPUT);
  pinMode(limit[0], INPUT_PULLUP);
  pinMode(limit[1], INPUT_PULLUP);

  // make sure switches are not pressed initially
  if(!(digitalRead(limit[0]) && digitalRead(limit[1]))){
    analogWrite(pin[0], 255);
    analogWrite(pin[1], 85);
    delay(200);
    analogWrite(pin[0], 255);
    analogWrite(pin[1], 255);
  }
  if(!(digitalRead(limit[0]) && digitalRead(limit[1]))){
    analogWrite(pin[0], 85);
    analogWrite(pin[1], 255);
    delay(200);
    analogWrite(pin[0], 255);
    analogWrite(pin[1], 255);
  }

  while(digitalRead(limit[0]) && digitalRead(limit[1])){
    // pwm motor0
    analogWrite(pin[0], 85);
    analogWrite(pin[1], 255);
  }
  // some switch hit: stop
  analogWrite(pin[0], 255);

  // if limit[1] was hit, swicth limits
  if(digitalRead(limit[0])){
    limit[0] = switch1;
    limit[1] = switch0;
  }

  // unpress switch
  while(!(digitalRead(limit[0]) && digitalRead(limit[1]))){
    analogWrite(pin[0], 255);
    analogWrite(pin[1], 85);
  }

  changeStateMillis = millis() + 1000;
  currentState = VoxMotor::PAUSE;
}

void VoxMotor::stop() {
  analogWrite(pin[currentDirection], 255);
}

void VoxMotor::update() {
  // deal with direction
  if(digitalRead(limit[0]) == LOW){
    currentDirection = 1;
  }
  if(digitalRead(limit[1]) == LOW){
    currentDirection = 0;
  }

  if(currentState == PAUSE){
    currentDutyCycle = 0.0;
    analogWrite(pin[0], 255);
    analogWrite(pin[1], 255);
    if(millis() > changeStateMillis){
      currentDirection = (random(10)<5);
      changeStateMillis = millis()+random(500,800);
      currentState = MOVE;
    }
  }
  if(currentState == MOVE){
    currentDutyCycle += (currentDutyCycle<PWM_MAX_DUTY)?0.05:0;
    analogWrite(pin[0], (currentDirection==0)?(1.0-currentDutyCycle)*255.0:255);
    analogWrite(pin[1], (currentDirection==1)?(1.0-currentDutyCycle)*255.0:255);
    if(millis() > changeStateMillis){
      changeStateMillis = millis()+random(3000,8000);
      currentState = PAUSE;
    }
  }
}

