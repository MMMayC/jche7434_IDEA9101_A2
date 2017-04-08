//Time - TimeRTC
#include <TimeLib.h>
#include <Wire.h>
#include <DS1307RTC.h>
#include <NewPing.h>


// TFT Screen
#include <SPI.h>
#include <DmTftIli9341.h>
DmTftIli9341 tft = DmTftIli9341(10, 9);

//Player Module
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
SoftwareSerial mySoftwareSerial(A11, A10); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

//Ultrasonic Sensor
#define TRIGGER_PIN  14
#define ECHO_PIN  15
#define MAX_DISTANCE 500
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); 
//long rangeInCentimeters;

const int vibration = A0;

const int buttonG = A7;
const int buttonY = A6;
const int buttonR = A5;

const int joyStickX = A15;
const int joyStickY = A13;
const int joyStickZ = 45;



void setup() {
  
  Serial.begin(9600);

  //Time initialization
  setTime(7, 29, 50, 6, 4, 2017);

  pinMode(vibration, INPUT);
  pinMode(buttonG, INPUT);
  pinMode(buttonY, INPUT);
  pinMode(buttonR, INPUT);
  pinMode(joyStickX, INPUT);
  pinMode(joyStickY, INPUT);
  pinMode(joyStickZ, INPUT);

  //Screen Initialization
  tft.init();

  // Player Module Setup
  mySoftwareSerial.begin(9600);
    if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.volume(5);  //Set volume value. From 0 to 30

  
}

void loop() {
  displayTime();

  Serial.println(digitalRead(buttonG));

  if(digitalRead(buttonG) == HIGH){
    Serial.println("Game Started!");
    thePitcher();
  }
}

//Return a string of current time for displaying on the screen
void displayTime(){

  //store current time as a char array
  char timeChar[9];

  //store current time as a string
  String timeString;
  
  //append the day of the week to the time string
  switch (weekday()){
    case 1: 
    timeString = "SUN";
    break; 
    case 2: 
    timeString = "MON";
    break; 
    case 3: 
    timeString = "TUE";
    break; 
    case 4: 
    timeString = "WED";
    break; 
    case 5: 
    timeString = "THU";
    break; 
    case 6: 
    timeString = "FRI";
    break; 
    case 7: 
    timeString = "SAT";
    break; 
  }

  timeString += " ";

  //if the hour is only one digit, add 0 before it to keep an unified format
  if(hour() < 10){
    timeString += "0";
  }
  //append hour to the time string
  timeString += String(hour());

  timeString += ":";
  
  //if the minute is only one digit, add 0 before it to keep an unified format
  if(minute() < 10){
    timeString += "0";
  }
  //append minute to the time string
  timeString += String(minute());

  //convert the time string to a char array for display on the screen
  timeString.toCharArray(timeChar, 10);

  tft.drawString(100, 0, timeChar);
  
}


long thePitcher(){
  
  long score = 0;
  int startM = minute();
  int startS = second();

  while((minute()-startM)*60+second()-startS < 60){
    if(analogRead(vibration) != 0){
      score ++;
    }
  };
  Serial.print("Score: ");
  Serial.println(score);
  return score;
  
}


int getDistance(){
  unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds.
  int rangeInCentimeters = (uS / US_ROUNDTRIP_CM);

  return rangeInCentimeters;
}


