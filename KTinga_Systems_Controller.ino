/*
 * K'Tinga Systems Controller
 * Author: Frankie Winters
 * License: MIT
 */
#include "Curve.h"
#include "LEDFader.h"
#include "LedFlasher.h"
#include "Adafruit_NeoPixel.h"
#include "Adafruit_NeoPatterns.h"

// Pin assignments
const byte NavigationBeaconPin = 9; 
const byte FloodlightsPin = 10;    
const byte EnvironmentLightsPin = 11; 

LEDFader environmentLights = LEDFader(EnvironmentLightsPin);
LedFlasher navigationMarkers = LedFlasher(NavigationBeaconPin, 2000, 1000, true);
LEDFader floodlights = LEDFader(FloodlightsPin);

uint32_t lastStateChange = 0;
uint16_t timeUntilStateChange = 0;
uint32_t missionDuration = 0;

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

void setup ()
{
  Serial.begin(9600);
  pinMode(EnvironmentLightsPin, OUTPUT);
  pinMode(FloodlightsPin, OUTPUT);
  pinMode(NavigationBeaconPin, OUTPUT);
 
  environmentLights.set_value(0);
  floodlights.set_value(0);

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
        navigationMarkers.begin();
        environmentLights.set_curve(Curve::exponential);
        environmentLights.fade(255, 1850);
        missionReport("Online");
        nextStatus = reactorInitialized;
    } break;
    
    /* Reactor Initalized: 
     - Start Navigation Lights */  
    case reactorInitialized: {
        floodlights.fade(155, 2500);
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
        nextStatus = nominal;
    } break;

    /* Nominal:
     - Wait a while, then fire torpedoes again */
    case nominal:
    default: {
        missionReport("Nominal");
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
  Serial.println(seconds);
  Serial.print(": ");
  Serial.println(label);
}

void loop ()
{
  floodlights.update();
  environmentLights.update();
  navigationMarkers.update();
  
  // check to see if we've spend enough time in this state. 
  if (millis() - lastStateChange >= timeUntilStateChange)  {
    doStateChange ();
  }
}  
