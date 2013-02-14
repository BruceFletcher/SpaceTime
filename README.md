SpaceTime
=========

Clock design to help manage open hours at Vancouver Hack Space (VHS)

When this is done, it should be a design for a three line clock using 2.3" 7-segment LED
displays.  The three rows are:

* current time
* closing time (i.e. when the night's responsible keyholding member is planning to leave)
* cleanup time (say, 20 minutes before closing time)

The clock is AVR based and intended to be hooked up to more network-capable systems over
a serial link for display on http:isvhsopen.com or whatever else.

Hardware design
~~~~~~~~~~~~~~~

The schematics and board layout are done in KiCAD.  The project has one main board with
the AVR MCU, power, LED drivers and various connectors on it, as well as a board designed
to hold four 7-segment LED characters for displaying a time value.  This LED board needs
to be replicated three times in production.

Enclosure design is TBD.  I'd like to do it with a Lamicoid faceplate, but it may end
up being easier to do it with laser-cut acrylic or plywood, since that seems to be what
is commonly available around VHS.

I don't have a keypad or other on-unit inputs picked out yet,  Presumably I'll
go with a 4x4 scanned matrix keypad, but I'm open to cooler ideas.  I also should have
some sort of sound output for when cleanup time is reached, but I don't have anything
picked out for that yet.

I'm assuming an FTDI serial interface (6 * 0.1" headers) for communication to a
network-capable host, but that connector choice may have to be reconsidered.  Being
able to start software development with an FTDI cable attached seems convenient, though.

Software design
~~~~~~~~~~~~~~~

This is going to have to be heavily interrupt-driven, so Arduino code is out.

Nothig here should be difficult to implement in C.

