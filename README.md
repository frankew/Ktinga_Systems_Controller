Klingon Battle Cruiser model lights controller for Arduino. 

There are a total of seven circuits operated by the Arduino. Three pins are connected to WS2812B NeoPixels for the following circuits:

1. Forward Torpedo
2. Reactor Intake (strip of three)
3. Thruster Exhaust

The NeoPixels need 5v. Each data line runs through a 470 Ω resistor at the NeoPixel end.

Four more circuits are connected to PWM-capable digital out pins.  These circuits are:

1. Enviroment Lights
2. Navigation Beacons
3. Floodlights
4. Reactor Lights

These drive "dumb" LEDs via FQP27P06 MOSFETs with 1k pull-down resistors on the gate pin. The LEDs are connected directly to +12v and the MOSFETs interrupt the ground path.