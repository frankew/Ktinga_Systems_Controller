/*
 * K't'inga Systems Controller
 * Author: Frankie Winters
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
FewPatterns torpedo = FewPatterns(1, TorpedoPin, NEO_RGB + NEO_KHZ800, NULL);

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

uint32_t black = exhaust.Color(0,0,0);
uint32_t red = exhaust.Color(20, 255, 0);
uint32_t blue = exhaust.Color(128, 0, 153);
uint32_t orangered = exhaust.Color(85, 255, 0);
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
        reactorLights.fade(255, 850);

        intake.FadeFlicker(orangered, 80, 35);
        nextStatus = stationKeeping;
    } break;

    /* Stationkeeping: 
     - Ramp up Environment Lights
     - Ramp up Floodlights */  
    case stationKeeping: {
        missionReport("StationKeeping");
        environmentLights.fade(85, 2500);
        floodlights.fade(255, 1200);
        nextStatus = thrusters;
    } break;

    /* Thrusters: 
     - Start Intake and Exhaust Lights */  
    case thrusters: {
        missionReport("Thrusters!");
        exhaust.FadeFlicker(red, 80, 35);
        nextStatus = fire;
    } break;

    /* Fire! 
     - Dim floodlights
     - Fire forward  torpedo 
     - Ramp up floodlights */    
    case fire: {
        missionReport("Fire");

        floodlights.fade(0, 250);
        torpedo.ColorSet(red);
        timeUntilStateChange = 2500;
        nextStatus = nominal;
    } break;

    /* Nominal:
     - Wait a while, then fire torpedoes again */
    case nominal:
    default: {
        missionReport("Nominal");
        floodlights.fade(255, 1200);
        torpedo.ColorSet(black);

        timeUntilStateChange = 6000;
        nextStatus = fire;
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
}

void intakeComplete()
{
  intake.ActivePattern = FLICKER;
  missionReport("intake fade complete");
}

void exhaustComplete()
{
  exhaust.ActivePattern = FLICKER;
  missionReport("exhaust fade complete");
}