Klingon Battle Cruiser model lights controller for Arduino. 

There are seven circuits. Four are connected to "dumb" LEDs. These are driven thru PWM pins connected to n-channel MOSFETs with 1k pull-down resistors on the gate pin. The MOSFET interrupts the ground path for the loads. The circuits are:

1. Enviroment Lights
2. Navigation Beacons
3. Floodlights
4. Reactor Lights

Three digital out pins are connected to WS2812B NeoPixels:

1. Forward Torpedo
2. Reactor Intake (strip of three)
3. Thruster Exhaust

The NeoPixels need 5v. Each signal line is runs through a 470 Ω resistor at the NeoPixel end.