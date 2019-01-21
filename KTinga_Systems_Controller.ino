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
uint16_t timeInThisState = 1000;
uint32_t missionDuration = 0;

enum ShipStates {
  offline = 0,
  initialized = 1,
  nominal = 2,
  encounteredAnomaly = 7
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
  lastStateChange = millis();    // when we last changed states
  timeInThisState = 1000;        // default one second between states

  switch(shipStatus) {
    case offline: {
        printTickTock("Initializing");

        navigationMarkers.begin();
        environmentLights.set_curve(Curve::exponential);
        environmentLights.fade(255, 1850);

        shipStatus = initialized;
    } break;
    case initialized: {
        floodlights.fade(155, 2500);
        shipStatus = nominal;
        printTickTock("Initialized");
    } break;
    case nominal: {
        // Serial.println("nominal");
        // environmentLights.fade(255, 850);
        printTickTock("Nominal");
    } break;
    default: {
        printTickTock("Default");
    }
  }


}  

void printTickTock(String label) {
  uint32_t tseconds = lastStateChange / 1000;
  uint32_t tminutes = tseconds / 60;
  uint32_t seconds = tseconds % 60;
  Serial.print(label);
  Serial.print(": ");
  Serial.print(tminutes);
  Serial.print(":");
  Serial.println(seconds);
}


void loop ()
{
  floodlights.update();
  environmentLights.update();
  navigationMarkers.update();
  if (millis() - lastStateChange >= timeInThisState)  {
    doStateChange ();
  }
}  
