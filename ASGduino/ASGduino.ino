/*
    ASG Controller Code
    
    Designed to run on a Freetronics Leostick, but will work on any other Arduino board.
    
    
    Copyright (C) 2013 Mark Jessop <mark.jessop@adelaide.edu.au>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    For a full copy of the GNU General Public License, 
    see <http://www.gnu.org/licenses/>.

*/


#if ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <TimerOne.h>

// PIN DEFINITIONS
#define FIRE  2
#define AZ_DIR  9
#define AZ_STEP 8
#define ELE_DIR 4
#define ELE_STEP 3
#define LED    13
#define POT_INPUT   5

#define STEP_UNIT  500
#define FIRE_PERIOD  1500

#define TIMER_PERIOD  300
unsigned int fire_timer = 0;
unsigned int az_timer = 0;
unsigned int ele_timer = 0;

void asg_isr(){
   if(fire_timer>0) fire_timer--;
   if(az_timer>0) az_timer--;
   if(ele_timer>0) ele_timer--; 
   
   if(fire_timer==0) digitalWrite(FIRE, LOW);
   if(az_timer>0){
     if(az_timer&1 == 1) digitalWrite(AZ_STEP, HIGH);
     if(az_timer&0 == 0) digitalWrite(AZ_STEP, LOW);
    }
    
    if(ele_timer>0){
        if(ele_timer&1 == 1) digitalWrite(ELE_STEP, HIGH);
        if(ele_timer&0 == 0) digitalWrite(ELE_STEP, LOW);
    }
   if(az_timer==0 && fire_timer==0) digitalWrite(LED, LOW);
}

void right(){
  digitalWrite(AZ_DIR, HIGH);
}

void left(){
  digitalWrite(AZ_DIR, LOW);
}

void az_step(){
  Timer1.detachInterrupt();
  az_timer = az_timer + STEP_UNIT;
  digitalWrite(LED, HIGH);
  Timer1.attachInterrupt(asg_isr);
}

void up(){
  digitalWrite(ELE_DIR, HIGH);
}

void down(){
  digitalWrite(ELE_DIR, LOW);
}

void ele_step(){
  Timer1.detachInterrupt();
  ele_timer = ele_timer + STEP_UNIT;
  digitalWrite(LED, HIGH);
  Timer1.attachInterrupt(asg_isr);
}

void stop(){
  Timer1.detachInterrupt();
  fire_timer = 0;
  az_timer = 0;
  ele_timer = 0;
  Timer1.attachInterrupt(asg_isr);
}

void fire(){
  Timer1.detachInterrupt();
  fire_timer = fire_timer + FIRE_PERIOD;
  digitalWrite(FIRE, HIGH);
  digitalWrite(LED, HIGH);
  Timer1.attachInterrupt(asg_isr);
}

void setup(){
  pinMode(FIRE, OUTPUT);
  digitalWrite(FIRE,LOW);
  pinMode(AZ_DIR, OUTPUT);
  pinMode(AZ_STEP, OUTPUT);
  pinMode(ELE_DIR, OUTPUT);
  pinMode(ELE_STEP, OUTPUT);
  digitalWrite(ELE_DIR, LOW);
  digitalWrite(AZ_DIR, LOW);
  digitalWrite(AZ_DIR, LOW);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  
  Serial.begin(9600);
  
  Timer1.initialize(TIMER_PERIOD);
  Timer1.attachInterrupt(asg_isr);
}

void loop(){
  char incomingByte = 0;
  if( Serial.available()>0 ){
      incomingByte = Serial.read();
      
      if(incomingByte == '<'){
        //Serial.println("OK");
        left(); az_step();
      }else if(incomingByte == '>'){
        //Serial.println("OK");
        right(); az_step();
      }else if(incomingByte == '^'){
        up(); ele_step();
      }else if(incomingByte == 'v'){
        down(); ele_step();
      }else if(incomingByte == 'f'){
        //Serial.println("OK");
         fire();
      }else if(incomingByte == 's'){
        stop();
      }else if(incomingByte == 'p'){
        Serial.println(analogRead(POT_INPUT));
      
      }else{
        //Serial.println("ERROR");
      }
      
  }
  
}
