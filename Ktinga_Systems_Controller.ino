/*
 * K't'inga Systems Controller
 * Author: Frankie Winters
 * License: MIT
 */
#include "Curve.h"
#include "LEDFader.h"
#include "LedFlasher.h"
#include "Adafruit_NeoPatterns.h"

// What's connected where?
const byte TorpedoPin = 3; 
const byte IntakePin = 4;    
const byte ExhaustPin = 5; 

const byte ReactorLightsPin = 6;
const byte NavigationBeaconsPin = 9; 
const byte FloodlightsPin = 10;    
const byte EnvironmentLightsPin = 11; 

// Controllers for different lighting circuits
LEDFader reactorLights = LEDFader(ReactorLightsPin);
LEDFader floodlights = LEDFader(FloodlightsPin);
LEDFader environmentLights = LEDFader(EnvironmentLightsPin);
LedFlasher navigationBeacons = LedFlasher(NavigationBeaconsPin, 2000, 1000, true);
NeoPatterns intake  = NeoPatterns(3, IntakePin, NEO_RGB + NEO_KHZ800, NULL);
NeoPatterns exhaust = NeoPatterns(1, ExhaustPin, NEO_RGB + NEO_KHZ800, NULL);
NeoPatterns torpedo = NeoPatterns(1, TorpedoPin, NEO_RGB + NEO_KHZ800, NULL);

// Ship state and time counting variables
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
uint32_t red = exhaust.Color(20, 248, 0);
uint32_t blue = exhaust.Color(128, 0, 153);
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
        navigationBeacons.begin();

        missionReport("Online");
        nextStatus = reactorInitialized;
    } break;
    
    /* Reactor Initalized: 
     - Start Navigation Lights */  
    case reactorInitialized: {
        // floodlights.fade(255, 2500);
        missionReport("Fusion Reactor Initialized");
        nextStatus = stationKeeping;
    } break;

    /* Stationkeeping: 
     - Ramp up Environment Lights
     - Ramp up Floodlights */  
    case stationKeeping: {
        missionReport("StationKeeping");
        nextStatus = thrusters;
    } break;

    /* Thrusters: 
     - Start Intake and Exhaust Lights */  
    case thrusters: {
        missionReport("Thrusters!");
        nextStatus = fire;
    } break;

    /* Fire! 
     - Dim floodlights
     - Fire forward  torpedo 
     - Ramp up floodlights */    
    case fire: {
        missionReport("Fire");
        torpedo.ColorSet(red);
        // intake.ColorSet(red);
        for (int pxl = 0; pxl < 3; pxl++) {
            intake.setPixelColor(pxl, red);
        }
        exhaust.ColorSet(violet);

        environmentLights.set_value(85);
        floodlights.set_value(255);
        reactorLights.fade(255, 1000);

        nextStatus = nominal;
    } break;

    /* Nominal:
     - Wait a while, then fire torpedoes again */
    case nominal:
    default: {
        missionReport("Nominal");
        torpedo.ColorSet(blue);
        // intake.ColorSet(blue);
        for (int pxl = 0; pxl < 3; pxl++) {
            intake.setPixelColor(pxl, blue);
        }
        exhaust.ColorSet(red);

        environmentLights.set_value(255);
        floodlights.set_value(0);
        reactorLights.fade(0, 1000);

        timeUntilStateChange = 3000;
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
}

void exhaustComplete()
{
}