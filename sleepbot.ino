// Bots for Behavior - Sleep Pattern Bot

//SCALE --------------------------------------------------------------------
//#include <HX711.h>
#include "HX711.h"
#define DOUT  2
#define CLK  3
HX711 scale;
float calibration_factor = -216.65;

//RTC --------------------------------------------------------------------
#include "RTClib.h"
RTC_DS1307 rtc;

//E-PAPER --------------------------------------------------------------------
#include <SPI.h>
#include "epd2in7.h"
#include "imagedata.h"
#include "epdpaint.h"
#define COLORED     0
#define UNCOLORED   1
Epd epd;

// VARIABLES --------------------------------------------------------------------
int chek = 0; 
int weight = 0; 
boolean face = false; 
int check_ = 0; 
int min_counter = 0; 
int prev_min = 0; 
boolean timer = false; 
boolean buzzer = false; 
const int buzz = 6; //buzzer to arduino pin 6
int pr = 0;
int tot_counter = 0; 
int break_time = 21; 

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void setup() {

// SCALE --------------------------------------------------------------------

  scale.begin(DOUT, CLK);
  scale.set_scale();
  scale.tare(); //Reset the scale to 0

// RTC --------------------------------------------------------------------
 
Serial.begin(57600);

#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

// E-PAPER --------------------------------------------------------------------

 epd.Init();
 epd.ClearFrame();  
 epd.DisplayFrame(Sad);
 

// BUZZER --------------------------------------------------------------------
pinMode(buzz, OUTPUT); // Set buzzer - pin 6 as an output
 
}

// LOOP --------------------------------------------------------------------

void loop() { 
 DateTime now = rtc.now();
 int hr = now.hour();
 int sec = now.second(); 
 //Serial.println(now.hour(), DEC); 
 //Serial.println(now.minute(), DEC); 

 scale.set_scale(calibration_factor);
 weight = scale.get_units() * -1; 
 Serial.println(weight);
 
 chek = 0; 
 //weight of my phone is: 124g
 //weight of my laptop is: 1944
 //together they weight: 2068

//functions
 
checkWeightChangeFace(hr); // checks weight and changes face 

timer_(); // keeps track of how long the object been on the scale 

buzz_buzz(); // regulates the buzzer

total_time(); // prints total time the object has been on the scale (in min) 

}

void checkWeightChangeFace(int hr) {
DateTime now = rtc.now();

int hr_ = hr;

if (weight >= 121 && weight <= 131) {
  chek = 1;
 }

if (chek == 1) {
Serial.println("Good Job");
face = true; 
 } else {
Serial.println(":(");
face = false;
}

if (face == true && check_ == 0) {

epd.ClearFrame();  
epd.DisplayFrame(Smile_2);
check_ = 1; 
timer = true;
buzzer = false; 
prev_min = now.minute();
}

if (face == false && check_ == 1) {

showRecord(); 

epd.ClearFrame();  
epd.DisplayFrame(Sad);
check_ = 0; 
min_counter = 0;
timer = false;
buzzer = true; 
}

if (min_counter == 10 && now.second() == 5) {
  epd.ClearFrame();  
  epd.DisplayFrame(Sleep);
  Serial.println("Very Good Job");
}

}

void buzz_buzz(){
DateTime now = rtc.now();
if (buzzer == true && now.hour() >= break_time) {

  tone(buzz, 750); // Send 1KHz sound signal...
  delay(1000);        // ...for 1 sec
  noTone(buzz);     // Stop sound...
  delay(500);        // ...for 1sec
  
}

if (buzzer == true && now.hour() < 9) {

  tone(buzz, 750); // Send 1KHz sound signal...
  delay(1000);        // ...for 1 sec
  noTone(buzz);     // Stop sound...
  delay(500);        // ...for 1sec
  
}
  
}

void timer_() {
DateTime now = rtc.now();

if (timer == true) {
if (now.minute() != prev_min) {
  min_counter++;
  tot_counter++; 
  Serial.println(min_counter); 
  prev_min = now.minute(); 
}
}
} 

void showRecord() {

if (pr < min_counter) {
pr = min_counter;

}

char pr_[16];
itoa(pr, pr_, 10); 

char s[64];
strcpy (s, "PR: "); 
strcat (s, pr_); 

unsigned char image[1024];
Paint paint(image, 176, 24);    //width should be the multiple of 8  
paint.Clear(UNCOLORED);

paint.DrawStringAt(6, 12, s, &Font16, COLORED);
epd.TransmitPartialData(paint.GetImage(), 16, 155, paint.GetWidth(), paint.GetHeight());
epd.DisplayFrame();

}

int total_time() {

 Serial.println(tot_counter); 
  
  }
