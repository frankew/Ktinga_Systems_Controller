/*
 * K't'inga Systems Controller
 * Author: Frankie Winters
 * Version 1.0
 * License: MIT
 */
#include "Curve.h"
#include "LEDFader.h"
#include "LedFlasher.h"
#include "NeoPixel_FewPatterns.h"

// What's connected where?
const byte TorpedoPin = 3; 
const byte IntakePin = 4;    
const byte ExhaustPin = 5; 

const byte ReactorLightsPin = 6;
const byte NavigationBeaconsPin = 9; 
const byte FloodlightsPin = 10;    
const byte EnvironmentLightsPin = 11; 

void intakeComplete();

// Controllers for different lighting circuits
LEDFader reactorLights = LEDFader(ReactorLightsPin);
LEDFader floodlights = LEDFader(FloodlightsPin);
LEDFader environmentLights = LEDFader(EnvironmentLightsPin);
LedFlasher navigationBeacons = LedFlasher(NavigationBeaconsPin, 2000, 1000, true);
FewPatterns intake  = FewPatterns(3, IntakePin, NEO_RGB + NEO_KHZ800, &intakeComplete);
FewPatterns exhaust = FewPatterns(1, ExhaustPin, NEO_RGB + NEO_KHZ800, &exhaustComplete);
FewPatterns torpedo = FewPatterns(1, TorpedoPin, NEO_RGB + NEO_KHZ800, &torpedoComplete);

// Ship status and time counting variables
enum ShipStatus {
  offline = 0,
  reactorInitialized = 1,
  stationKeeping = 2,
  thrusters = 3,
  fire = 4,
  nominal = 5,
  encounteredAnomaly = -1
};
ShipStatus shipStatus = offline;
uint32_t lastStateChange = 0;
uint16_t timeUntilStateChange = 0;
bool canFireTorpedos = true;

uint32_t black = exhaust.Color(0,0,0);
uint32_t red = exhaust.Color(20, 255, 0);
uint32_t blue = exhaust.Color(128, 0, 153);
uint32_t orangered = exhaust.Color(68, 205, 0);
uint32_t yellow = exhaust.Color(115, 115, 20);
uint32_t violet = exhaust.Color(0, 159, 255);

void setup ()
{
  Serial.begin(9600);
  pinMode(ReactorLightsPin, OUTPUT);
  pinMode(EnvironmentLightsPin, OUTPUT);
  pinMode(FloodlightsPin, OUTPUT);
  pinMode(NavigationBeaconsPin, OUTPUT);
 
  pinMode(TorpedoPin, OUTPUT);
  pinMode(IntakePin, OUTPUT);
  pinMode(ExhaustPin, OUTPUT);

  reactorLights.set_value(0);
  environmentLights.set_value(0);
  floodlights.set_value(0);

  torpedo.begin();
  torpedo.show();
  intake.begin();
  intake.show();
  exhaust.begin();
  exhaust.show();

  lastStateChange = millis();

  missionReport("Setup");
}

void doStateChange ()
{
  lastStateChange = millis();    // remember when we last changed states (now)
  timeUntilStateChange = 1000;   // default one second between states
  ShipStatus nextStatus; 
  
  switch(shipStatus) 
  { 
    /* Offline: 
     - Start Reactor Lights */ 
    case offline: {
        missionReport("Online");
        nextStatus = reactorInitialized;
    } break;
    
    /* Reactor Initalized: 
     - Start Navigation Lights */  
    case reactorInitialized: {
        missionReport("Fusion Reactor Initialized");
        navigationBeacons.begin();
        intake.FadeFlicker(orangered, 80, 35);
        reactorLights.fade(255, 3850);
        timeUntilStateChange = 6000;
        nextStatus = stationKeeping;
    } break;

    /* Stationkeeping: 
     - Ramp up Environment Lights
     - Ramp up Floodlights */  
    case stationKeeping: {
        missionReport("StationKeeping");
        environmentLights.fade(85, 1650);
        timeUntilStateChange = 8000;
        nextStatus = thrusters;
    } break;

    /* Thrusters: 
     - Start Intake and Exhaust Lights */  
    case thrusters: {
        missionReport("Thrusters!");
        floodlights.fade(255, 1850);
        exhaust.FadeFlicker(red, 80, 35);
        nextStatus = nominal;
    } break;

    /* Nominal:
     - Wait a while, then fire torpedoes  */
    case nominal:
    default: {
        missionReport("Nominal");
        timeUntilStateChange = 16000;
        if (canFireTorpedos) {
          nextStatus = fire;
        }
    } break;

    /* Fire! 
     - Dim floodlights
     - Fire forward  torpedo 
     - Ramp up floodlights */    
    case fire: {
        missionReport("Fire");
        canFireTorpedos = false;
        floodlights.fade(0, 285);
        exhaust.FadeFlicker(black, 35, 35);
        torpedo.FadeFlicker(yellow, 65, 35);
        // timeUntilStateChange = 7500;
        nextStatus = nominal;
    } break;

  }
  shipStatus = nextStatus; 
}  

void missionReport(String label)
{
  uint32_t tseconds = lastStateChange / 1000;
  uint32_t tminutes = tseconds / 60;
  uint32_t seconds = tseconds % 60;
  Serial.print(tminutes);
  Serial.print(":");
  Serial.print(seconds);
  Serial.print(": ");
  Serial.println(label);
}

void loop ()
{
  floodlights.update();
  environmentLights.update();
  reactorLights.update();
  navigationBeacons.update();
  
  torpedo.Update();
  intake.Update();
  exhaust.Update();

  // check to see if we've spend enough time in this state. 
  if (millis() - lastStateChange >= timeUntilStateChange)  {
    doStateChange ();
  }
}

void torpedoComplete()
{
  torpedo.FadeFlicker(red, 10, 15);
  torpedo.OnComplete = &torpedoComplete2;
}

void torpedoComplete2()
{
  torpedo.FadeFlicker(yellow, 65, 35);
  torpedo.OnComplete = &torpedoComplete3;
}

void torpedoComplete3()
{ 
  torpedo.FadeFlicker(red, 10, 15);
  torpedo.OnComplete = &torpedoComplete4;
}

void torpedoComplete4()
{
  torpedo.FadeFlicker(yellow, 65, 35);
  torpedo.OnComplete = &torpedoComplete5;
}

void torpedoComplete5()
{ 
  torpedo.FadeFlicker(red, 10, 15);
  torpedo.OnComplete = &torpedoComplete6;
}

void torpedoComplete6()
{ 
  torpedo.FadeFlicker(yellow, 45, 35);
  torpedo.OnComplete = &torpedoComplete7;
}

void torpedoComplete7()
{ 
  torpedo.FadeFlicker(black, 15, 35);
  torpedo.OnComplete = &torpedoComplete8;
}

void torpedoComplete8()
{
  torpedo.ActivePattern = NONE;
  torpedo.ColorSet(black);
  torpedo.OnComplete = &torpedoComplete;
  floodlights.fade(255, 1200);
  exhaust.FadeFlicker(red, 225, 35);
  canFireTorpedos = true;
}

void intakeComplete()
{
  intake.ActivePattern = FLICKER;
}

void exhaustComplete()
{
  exhaust.ActivePattern = FLICKER;
}