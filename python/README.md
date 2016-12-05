## SpaceTime - Python Overview
The SpaceTime box contains a Raspberry Pi and a custom AVR-based PCB called [SpaceTime](../README.md). The Raspberry Pi runs a Python program that performs NTP time sync and facilitates network communication between SpaceTime and both the [isvhsopen.com Web API](http://isvhsopen.com/api/status/) ([GitHub](https://github.com/vhs/isvhsopen)) and the [Hackspace API](http://api.vanhack.ca) ([GitHub](https://github.com/vhs/api)). Communication with the Hackspace API is mostly deprecated in favour of the new isvhsopen Web API, but is still used to log the Raspberry Pi's local IP address. A local web server also provides a REST interface on port 80 to change the status from within the VHS local network.

The Raspberry Pi username/password and local IP address can be found on the physical SpaceTime box. The device is also configured with hostname ```isvhsopen-spacetime```. IP address is also on the Hackspace API as [spacetime_ip](http://api.vanhack.ca/s/vhs/data/spacetime_ip.txt). To connect to the Raspberry Pi from the local network, make sure you are on the same subnet by ensuring that `spacetime_ip` starts the same as your computer's IP. Then connect using any SSH client, such as `putty` or the Linux command line `ssh`.

## Configuring Raspberry Pi from scratch

#### Install Python packages from command line

```Shell
> sudo easy_install -U pyserial
> sudo easy_install -U requests
> sudo easy_install -U web.py
```

#### Enable UART serial on Raspberry Pi
To enable access of UART serial on GPIO pins through Python, edit system files:
http://www.irrational.net/2012/04/19/using-the-raspberry-pis-serial-port/

#### Set local timezone for Raspberry Pi
```Shell
> sudo ln -sf /usr/share/zoneinfo/America/Vancouver /etc/localtime
```
Check that NTP Daemon runs on startup. Ensure that `/etc/rc.conf` contains the line:
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
> mkdir /usr/local/bin/SpaceTime
```
Navigate to the `/usr/local/bin` directory and perform Git Clone of this repo. The following will download only the latest version of the `/SpaceTime/tree/master/python` folder:
```Shell
> cd /usr/local/bin
> git clone --no-checkout --depth=1 https://github.com/<YOUR_ACCOUNT>/SpaceTime
> cd SpaceTime
> git config core.sparsecheckout true
> echo python > .git/info/sparse-checkout
> git checkout master
```
The code should now be at `/usr/local/bin/SpaceTime/python`
In the future, to download the latest code from GitHub:
```Shell
> cd /usr/local/bin/SpaceTime
> git pull
```
Note that you'll have to reapply the Web API Key after performing a ```git pull``` (see below).

#### Make program run on startup
Add to the end of `/etc/rc.local`
```Shell
> sudo nano /etc/rc.local

python /usr/local/bin/SpaceTime/python/main.py 80 &
```
The parameter after main.py is the port (80) for which to host the RESTful web server.

#### Get isvhsopen Web API Key
Refer to the [isvhsopen.com API on GitHub](https://github.com/vhs/isvhsopen) to obtain an API Key. Once obtained, run this command, replacing ```[Generated Key]``` with the actual API Key. HTTP POST commands to update the API will fail without a valid API Key.
```Shell
> cd /usr/local/bin/SpaceTime/python
> sudo sed -i 's/ISVHSOPEN_API_KEY/[Generated Key]/g' webapi.py
```
Please do not push changes to GitHub that include the API Key. This key should only be stored locally on the Raspberry Pi.

#### Configure VHS API variables
Find and edit the Hackspace API variable names used at the top of `main.py`. With the new isvhsopen.com API (independent of the Hackspace API), the only variable we still update directly on api.vanhack.ca is the one that stores the Raspberry Pi's local IP.

#### Set hostname for Raspberry Pi
To configure the Raspberry Pi with hostname ```isvhsopen-spacetime```:
Edit the 127.0.1.1 entry in `/etc/hosts`
```Shell
> sudo nano /etc/hosts

127.0.1.1       isvhsopen-spacetime
```
Replace the current hostname in `/etc/hostname` with ```isvhsopen-spacetime```
```Shell
> sudo nano /etc/hostname

isvhsopen-spacetime
```
Commit the changes and reboot
```Shell
> sudo /etc/init.d/hostname.sh
> sudo reboot
```
Now you should be able to ssh into the Raspberry Pi using either its [local IP address](http://api.vanhack.ca/s/vhs/data/spacetime_ip.txt) or hostname (```isvhsopen-spacetime```).

#### Testing
To run unit tests from command line:
```Shell
> python -m unittest test_timeutil
> python -m unittest test_vhsapi
> python -m unittest test_webapi
> python -m unittest test_spacetime
```

#### Debugging via serial
To speak directly to the SpaceTime board, connect via SSH to the Raspberry Pi, and then start a serial connection with `/dev/ttyAMA0` at `57600` baud. Ex:
```Shell
> screen /dev/ttyAMA0 57600
```

Type `AT?` and `<Enter>` to see help from the SpaceTime board.

Note that only one serial connection can be made at a time, so running screen will prevent the python script from communicating with the SpaceTime board. While using screen, the following commands are useful:
```
Exit:		Ctrl-a \
Detach:		Ctrl-a d
Reattach:	screen -x
List:		screen -ls
```

#### Using the REST API
The Python code hosts a web server on the local network. Anyone connected to the VHS network (anyone physically at the space) can connect to http://isvhsopen-spacetime/ to open the space, close the space, or change the closing time. Network admins, please do not expose this web service to the public internet. If you cannot access the URL, try [spacetime_ip](http://api.vanhack.ca/s/vhs/data/spacetime_ip.txt) on the Hackspace API and confirm that you're on the same network. Also try on port 8080, as this is the default if one is not specified on startup.

### TODO List

See [bugs and to-do's](../TODO.md).
