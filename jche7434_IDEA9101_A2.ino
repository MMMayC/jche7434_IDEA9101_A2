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
#define TRIGGER_PIN  15
#define ECHO_PIN  16
#define MAX_DISTANCE 500
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); 
long rangeInCentimeters;

const int vibration = A15;

const int photoresistor = A8;



class Pitcher
{
 private:
  long score;
  char scoreChar[16];

  char totalChar[16];



  int startM;
  int startS;
  int timeRm;
  
  String timeString;
  char timeChar[2];

 public:

  long total;
  
  Pitcher(){
    score = 0;
    timeRm = 60;
  }

  void pitcherListener(){
    
    tft.drawString(0, 150, "Take the ball to start a game");
    if(analogRead(photoresistor) > 80){
      Serial.println("Game Started!");
      tft.clearScreen();
      tft.drawString(80, 150, "3");
      delay(1000);
      tft.drawString(80, 150, "2");
      delay(1000);
      tft.drawString(80, 150, "1");
      delay(1000);
      tft.drawString(80, 150, "Start!");
      delay(1000);
      pitcherStarted();
    }
  }

  void pitcherStarted(){
    
    startM = minute();
    startS = second();
    
    tft.clearScreen();
    
    tft.drawString(80, 80, "Time: ");
    tft.drawString(80, 150, "Score: ");
    tft.drawString(80, 170, "0");
    while(timeRm > 0){
      //Serial.println(analogRead(vibration));
      timeRm = 60 - ((minute()-startM)*60+second()-startS);
      if(timeRm < 10) {
        timeString = '0';
        timeString += String(timeRm);
      }else{
        timeString = String(timeRm);
      }
      timeString.toCharArray(timeChar, 3);
      tft.drawString(80, 100, timeChar);
      if(analogRead(vibration) > 80){
        score ++;
        String(score).toCharArray(scoreChar, 17);
        tft.drawString(80, 170, scoreChar);
      }
    }
    
    pitcherCompleted();
  }

  void pitcherCompleted(){

    total = score;
    
    String(score).toCharArray(totalChar, 17);

    tft.clearScreen();
    tft.drawString(80, 80, "Total");
    tft.drawString(80, 100, totalChar);
    delay(2000);
    tft.clearScreen();
    tft.drawString(80, 150, "Put the ball back, please.");
    while(analogRead(photoresistor) > 80){}
    tft.clearScreen();
  }
};



class Pomodoro
{
 private:
  int distance;
  int countPomo;
//  int pomoStartM;
//  int pomoStartS;
  int pomoTime = 15;
  int shortBreak = 5;
  int longBreak = 15;
  int deskDistance = 50;
  int commH = 7;
//  int commInterval = 24;
  boolean isSent = false;

  long highest = 0;

 public:
 
  Pomodoro(){
    countPomo = 0;
  }
  
  void pomoListener(){
    myDFPlayer.pause();



    
    Serial.println("pomoListener");
    int distance = getDistance();
    //Serial.println(getDistance());
    tft.clearScreen();
    tft.drawString(0, 100, "Come sit down and start a pomodoro!");
    delay(1000);
    tft.clearScreen();
    while(distance > -1){
      //Serial.print(hour());
      //Serial.println(minute());
      if(hour() == commH && minute() > 31 && isSent == false){
        //Serial.println("Date: " + day() + " " + month() + " " + year());
        Serial.print("Day: ");
        Serial.println(day());
        Serial.print("Month: ");
        Serial.println(month());
        Serial.print("Year: ");
        Serial.println(year());
        Serial.print("PomoAmount: ");
        Serial.println(countPomo);
        countPomo = 0;
        Serial.print("Highest: ");
        Serial.println(highest);
        isSent = true;
      }

      
    if(hour() == commH + 1 && isSent == true){
      isSent = false;
    }
      //Serial.println(distance);
      distance = getDistance();
      if(distance > deskDistance || distance == 0){
        //Serial.println("distance > deskDistance");
        tft.drawString(0, 100, "Come sit down and start a pomodoro!");
      }else{
        tft.clearScreen();
        tft.drawString(20, 100, "POMODORO Started.");
        delay(1000);
        tft.clearScreen();
        pomoStarted();
      }
    }
  }
  void pomoStarted(){
    
    Serial.println("pomoStarted");
    int pomoStartM = minute();
    int pomoStartS = second();
    int timeR = pomoTime;

    

    while(timeR > 0){
    //  Serial.println("timeR > 0");
      timeR = timeRemaining(pomoStartM, pomoStartS, pomoTime);

      displayTimer("Pomodoro: ", timeR);
      
      if(getDistance() > deskDistance){
        pomoFailed();
      }
    }

    pomoCompleted();
    
  }

  void pomoFailed(){
    Serial.println("pomoFailed");
    tft.clearScreen();
    tft.drawString(20, 150, "Pomodoro Failed");
    delay(1000);
    pomoListener();
  }

  void pomoCompleted(){
    Serial.println("pomoCompleted");
    tft.clearScreen();
    tft.drawString(20, 150, "Pomodoro Completed");
    countPomo ++;
    pomoBreak();
  }

  int getDistance(){
   // Serial.println("getDistance");
   delay(100);
  unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds.
  int rangeInCentimeters = (uS / US_ROUNDTRIP_CM);
  //Serial.println(rangeInCentimeters);
  return rangeInCentimeters;
}

  int timePassed(int startM, int startS){
  //  Serial.println("timePassed");
    return (minute() - startM) * 60 + second() - startS;
  }

  int timeRemaining(int startM, int startS, int duration){
  //  Serial.println("timeRemaining");
    return (duration - ((minute() - startM) * 60 + second() - startS));
  }

  

  char timeToChar(int m, int s){
  //  Serial.println("timeToChar");
    String timeString;
    char timeChar[5];
    if(m < 10){
      timeString = "0";
      timeString += String(m);
    }else{
      timeString = String(m);
    }
    timeString += ":";
    if(s < 10){
      timeString += "0";
      timeString += String(s);
    }else{
      timeString += String(s);
    }
    timeString.toCharArray(timeChar, 6);
    //Serial.println(timeChar);
    return timeChar;
  }

  
  void displayTimer(char titleD[], int timeR){
   // Serial.println("displayTimer");
   String timeString;
    char timeChar[5];
    if(timeR/60 < 10){
      timeString = "0";
      timeString += String(timeR/60);
    }else{
      timeString = String(timeR/60);
    }
    timeString += ":";
    if(timeR%60 < 10){
      timeString += "0";
      timeString += String(timeR%60);
    }else{
      timeString += String(timeR%60);
    }
    timeString.toCharArray(timeChar, 6);
    
    tft.drawString(80, 80, titleD);
    tft.drawString(80, 100, timeChar);
  }
  
  void pomoBreak(){

    myDFPlayer.start();
    Serial.println("pomoBreak");

    int breakStartM = minute();
    int breakStartS = second();

    int timeR;

    int breakTime;
    
    if(countPomo % 4 == 0){
      breakTime = longBreak;
      timeR = breakTime;
    }else{
      breakTime = shortBreak;
      timeR = breakTime;
    }

    while(timeR > 0){
      timeR = timeRemaining(breakStartM, breakStartS, breakTime);
      displayTimer("Break: ", timeR);

      if(getDistance() <= 80){
        tft.drawString(0, 120, "Come on, you need a break!");
      }

      Pitcher pitcher;

      pitcher.pitcherListener();

      if(pitcher.total > highest){
        highest = pitcher.total;
      }
      
    }
    pomoListener();
  }

};



void setup() {
  
  Serial.begin(9600);

  //Time initialization
  setTime(7, 29, 50, 6, 4, 2017);

  pinMode(vibration, INPUT);
  pinMode(photoresistor, INPUT);

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
  myDFPlayer.play(3);
  myDFPlayer.pause();

  Pomodoro pomodoro;
  pomodoro.pomoListener();
}

void loop() {}

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








