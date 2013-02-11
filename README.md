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

