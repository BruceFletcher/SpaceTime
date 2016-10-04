SpaceTime
=========

Clock design to help manage open hours at Vancouver Hack Space (VHS)

When this is done, it should be a design for a two line clock using 2.3" 7-segment LED
displays.  The two rows are:

* current time
* closing time (i.e. when the night's responsible keyholding member is planning to leave)

The clock is AVR-based and communicates over a serial link with a Raspberry Pi, which performs 
NTP time sync and updates variables on http://api.hackspace.com based on the clock's state.
http://isvhsopen.com references the VHS API to display the open state and the closing time. 

There is a separate readme for the [**Raspberry Pi setup instructions**](python/README.md) 
and [Python scripts](python).

Hardware design
---------------

The schematics and board layout are done in KiCAD.  The project has one main board with
the AVR MCU, power, LED drivers and various connectors on it, as well as a board designed
to hold four 7-segment LED characters for displaying a time value.  Two copies of this LED
board are required.

Enclosure design is TBD.  I'd like to do it with a Lamicoid faceplate, but it may end
up being easier to do it with laser-cut acrylic or plywood, since that seems to be what
is commonly available around VHS.

Software design
---------------

The firmware is currently a work in progress.  It is written in C for the avr-gcc
compiler.

Once complete it should support:

* Two rows of 7-segment output
* Keypad input
* Serial I/O, probably with an AT style interface
* Drift correction for the clock
* A reasonably intuitive interface for setting times and general use

The [Raspberry Pi's code](python) is written in Python and is described more [here](python/README.md).

See [bugs and to-do's](TODO.md) for remaining work.

Enclosure
---------

The enclosure is a laser cut box made from 5mm ply, primarily designed and built
by Luke Closs - thanks Luke!  The basic box design was created with
http://boxmaker.rahulbotics.com with the following parameters:

* Width: 350mm
* Height: 280mm
* Depth: 60mm
* Thickness: 5mm
* Notch length: 12.5mm (automatic)
* Cut width: 0.25mm

