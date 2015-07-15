### TODO List
In case someone wishes to configure the build environment for the SpaceTime AVR chip, the following is a list of minor bugs and UI improvements that would be nice to address in the AVR's C code:

- Make the whole active LED blink, not just the D segment (referred to as 'underbar' in code), unless the active digit is blank.
- Make 6's have their A-segment turned on and 9's have their D-segment turned on.
- Consider 12hr time vs 24hr time (see what people think, after some use).
- Bug: When editing the time on a keypad, if you click Green or Red (to edit again), it resets the clock time in memory, but doesn't reset the clock display. So if you press Red, set the time, then press Red, then press Enter, it looks like you set it (except the colon is still off) but you didn't actually set it.
- Bug: When editing the time on a keypad, if you press the right arrow to set a one-digit hour, upon pressing Enter, it succeeds, but adds 10 hours to your time.
- Get rid of cleanup_time and countdown_time (they were planned features that never got implemented).
- When querying SpaceTime (ATSTn?) for a particular clock's time, it returns with either 'Not set' or an 'HH:MM:SS' time, but no label specifying which clock it refers to. It should respond with the clock name and time. (i.e. 'Closing time: 04:12:36')
 - Once Query response of ATSTn? is updated, the Python code can be updated to safely query Closing Time (ATST1?) on startup, and the 'AmbiguousTime' type can be removed.
- Add warning beeps at 15/30min before time's up (currently only beeps when time's up).
- Make the AT? help better (see below).

Proposed new format for help command (when you type 'AT?' in the serial console to SpaceTime):
```
SpaceTime commands:
  AT? - display this help
  AT  - display 'OK'
  ATST<n>?                  Query time
  ATST<n>=<hh:mm[:ss[.cc]]> Set time - .cc is 100ths of a second
  ATST<n>=x                 Clear time - intended for clearing closing time
  
  For all commands, valid values for n:
	0 - current, 1 - closing, 2 - cleanup, 3 - countdown
  
  Note: SpaceTime echoes all commands received. If you're using a serial 
        console, this is why you can see yourself typing).
```
**Note that if we remove cleanup_time and countdown_time from code, we should remove them from the help command as well.*
