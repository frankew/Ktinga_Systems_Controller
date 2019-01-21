/*
 * KTinga Systems Controller
 * Author: Frankie Winters
 * License: WTFPL
 * ./arduino-builder -hardware ./hardware -tools ./hardware/tools/avr -tools ./tools-builder -libraries ./libraries -fqbn arduino:avr:nano ~/Documents/repositories/refit-system-control/Daystrom_FWG1-M6-1701_Refit_Systems_Controller/Daystrom_FWG1-M6-1701_Refit_Systems_Controller.ino
 */
#include "Curve.h"
#include "LEDFader.h"
#include "LedFlasher.h"
#include "Adafruit_NeoPixel.h"
#include "NeoPixel_Animator.h"
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

enum ShipStates {
  offline = 0,
  reactorInitialized = 1,
  stationKeeping = 2,
  thrusters = 3,
  fire = 4,
  nominal = 5,
  encounteredAnomaly = -1
};
ShipStates shipStatus = offline;

void setup ()
{
  Serial.begin(9600);
  pinMode(EnvironmentLightsPin, OUTPUT);
  pinMode(FloodlightsPin, OUTPUT);
  pinMode(NavigationBeaconPin, OUTPUT);
 
  environmentLights.set_value(0);
  floodlights.set_value(0);

  lastStateChange = millis();

  printTickTock("Setup");
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
        printTickTock("Online");
        nextStatus = reactorInitalized;
    } break;
    
    /* Reactor Initalized: 
     - Start Navigation Lights */  
    case reactorInitalized: {
        floodlights.fade(155, 2500);
        printTickTock("Fusion Reactor Initialized");
        nextStatus = stationKeeping;
    } break;

    /* Stationkeeping: 
     - Ramp up Environment Lights
     - Ramp up Floodlights */  
    case stationKeeping: {
        printTickTock("StationKeeping");
        nextStatus = thrusters;
    } break;

    /* Thrusters: 
     - Start Intake and Exhaust Lights */  
    case thrusters: {
        printTickTock("Thrusters!");
        nextStatus = fire;
    } break;

    /* Fire! 
     - Dim floodlights
     - Fire forward  torpedo 
     - Ramp up floodlights */    
    case fire: {
        printTickTock("Fire");
        nextStatus = nominal;
    } break;

    /* Nominal:
     - Wait a while, then fire torpedoes again */
    case nominal:
    default: {
        printTickTock("Nominal");
        timeUntilStateChange = 3000;
        nextStatus = fire;
    } break;

  }
  shipStatus = nextStatus; 
}  

void printTickTock(String label)
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
