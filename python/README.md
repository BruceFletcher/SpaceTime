Raspberry Pi username/password and local IP address are marked on the physical SpaceTime box. IP address is also currently recorded as spacetime_ip on the Hackspace API. http://api.hackspace.ca/s/vhs/data/spacetime_ip To connect to the Raspberry Pi from the local network, make sure you are on the same subnet by ensuring that spacetime_ip start the same as your computer's IP. Then connect using any SSH client, such as putty, or the linux command line ssh.

### To configure a Raspberry Pi from scratch

#### Install Python packages from command line

```Shell
> sudo easy_install -U pyserial
> sudo easy_install -U requests
```

#### Enable UART serial on Raspberry Pi
To enable access of UART serial on GPIO pins through Python, edit system files:
http://www.irrational.net/2012/04/19/using-the-raspberry-pis-serial-port/

#### Set local timezone for Raspberry Pi
```Shell
> sudo ln -sf /usr/share/zoneinfo/America/Vancouver /etc/localtime
```
Check that NTP Daemon runs on startup. Ensure that /etc/rc.conf contains the line:
```Shell
DAEMONS=(!hwclock ntpd ntpdate)
```
Query system datetime:
```Shell
> date
```
If the date has drifted by more than 1000s, ntpd will not correct it by default. This will force time synchronization:
```Shell
> sudo ntpd -gq
```

#### Installing SpaceTime from GitHub

If Git is not installed:
```Shell
> sudo apt-get install git-core
```
Create directory for the Python code:
```Shell
> mkdir /usr/local/bin/spacetime
```
Navigate to the /usr/local/bin directory and perform Git Clone of this repo. The following will download only the latest version of the /SpaceTime/tree/master/python folder:
```Shell
> cd /usr/local/bin
> git clone --no-checkout --depth=1 git://github.com/<YOUR_ACCOUNT>/SpaceTime
> cd SpaceTime
> git config core.sparsecheckout true
> echo python > .git/info/sparse-checkout
> git checkout master
```
The code should now be at /usr/local/bin/spacetime/python
In the future, to download the latest code from GitHub:
```Shell
> cd /usr/local/bin/spacetime
> git pull
```

#### Make program run on startup
Add to the end of /etc/rc.local
```Shell
> sudo nano /etc/rc.local

python /usr/local/bin/spacetime/main.py &
```

#### Configure VHS API variables
Find and edit the Hackspace API variable names used at the top of main.py. Change them to match whatever is being displayed by http://www.isvhsopen.com.

To run unit tests from command line:
```Shell
> python -m unittest test_timeutil
> python -m unittest test_vhsapi
> python -m unittest test_spacetime
```

To speak directly to the SpaceTime board, connect via SSH to the Raspberry Pi, and then start a serial connection with /dev/ttyAMA0 at 57600 baud. Ex:
```Shell
> screen /dev/ttyAMA0 57600
```

Type 'AT?' and <Enter> to see help from the SpaceTime board.

Note that only one serial connection can be made at a time, so running screen will prevent the python script from communicating with the SpaceTime board. While using screen, the following commands are useful:
Exit:		Ctrl-a \
Detach:		Ctrl-a d
Reattach:	screen -x
List:		screen -ls





	
	
TODO in case I update the SpaceTime chip code:
- Make the whole active LED blink, not just the D segment (referred to as 'underbar' in code), unless the active digit is blank.
- When editing the time on a keypad, if you click Green or Red (to edit again), it resets the clock time in memory, but doesn't reset the clock display. So if you press Red, set the time, then press Red, then press Enter, it looks like you set it (except the colon is still off) but you didn't actually set it.
- When editing the time on a keypad, if you press the right arrow to set a one-digit hour, upon pressing Enter, it succeeds, but adds 10 hours to your time.
- Make 6's have A-segment turned on and 9's have D-segment turned on.
- Make the AT? help better.
- Get rid of cleanup_time and countdown_time (they were planned features that never got implemented).
- When querying SpaceTime (ATSTn?) for a particular clock's time, it returns with either 'Not set' or an 'HH:MM:SS' time, but no label specifying which clock it refers to. It should respond with the clock name and time. (i.e. 'Closing time: 04:12:36')
- Once Query response of ATSTn? is updated, the Python code can be updated to safely query Closing Time (ATST1?) on startup, and the 'AmbiguousTime' type can be removed.
- Consider 12hr time vs 24hr time.
- Add warning beeps at 15/30min before time's up.
- Add Wagner song at 13:37.



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

  
  
