Arduino Fan Controller
======================

Custom controller for a whole house fan. Designed for use with an [Arduino], an 16x2 [RGB LCD], four [DS18B20] temperature sensors, and a [Solid State Relay].

Currently a work in progress.

Primary goals are:

* Automatic temperature control based on internal/external sensors
* Temperature selection + on/off/standby mode via a potentiometer
* Reporting of outdoor temp and multiple indoor temperatures
* Backlight color to indicate difference between outdoor and indoor temperature (mainly for standby mode)
* Automatic brightness dimming at night

Potential future goals include:

* Ethernet connection and REST API
* Window & door open/closed status detection - don't turn on the fan if the house is shut
* Automatic window opening

The part choices might be a little odd, but they're primarily based on what I already had lying around and/or could source for free. You've got 6 analog lines available, so one could easily switch to analog sensors. And a regular (cheaper) mechanical relay would work fine as long as it (and it's PCB!) can handle the current of the fan.

MIT License
-----------

Copyright (c) 2014 Nathan Friedly - http://nfriedly.com/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.


[Arduino]: http://arduino.cc/
[RGB LCD]: http://www.adafruit.com/product/398
[DS18B20]: http://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS18B20.html
[Solid State Relay]: http://pewa.panasonic.com/components/relays/solid-state-relays/high-capacity-ssr/aq-a/