Raspberry Pi username/password and local IP address are marked on the physical SpaceTime box. IP address is also on the Hackspace API as [spacetime_ip](http://api.hackspace.ca/s/vhs/data/spacetime_ip.txt). To connect to the Raspberry Pi from the local network, make sure you are on the same subnet by ensuring that spacetime_ip starts the same as your computer's IP. Then connect using any SSH client, such as putty, or the linux command line ssh.

### Configuring Raspberry Pi from scratch

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
> mkdir /usr/local/bin/spacetime
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
The code should now be at `/usr/local/bin/spacetime/python`
In the future, to download the latest code from GitHub:
```Shell
> cd /usr/local/bin/spacetime
> git pull
```

#### Make program run on startup
Add to the end of `/etc/rc.local`
```Shell
> sudo nano /etc/rc.local

python /usr/local/bin/spacetime/python/main.py &
```

#### Configure VHS API variables
Find and edit the Hackspace API variable names used at the top of `main.py`. Change them to match whatever is being displayed by http://www.isvhsopen.com.

#### Testing
To run unit tests from command line:
```Shell
> python -m unittest test_timeutil
> python -m unittest test_vhsapi
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

### TODO List

See [bugs and to-do's](../TODO.md).
