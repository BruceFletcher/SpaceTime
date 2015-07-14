import time
import socket
from spacetime import SpaceTime
from vhsapi import VHSApi
from timeutil import *

dbg_showAllSerial = False #If true, prints out all received serial messages
lastClockSync   = 0       #Time of last clock sync with SpaceTime
api_var_door    = 'key'   #Should be 'door' when goes live
api_var_closing = 'test1' #'open_until' or 'isvhsopen_until'
api_var_ip      = 'spacetime_ip'
max_clock_drift = 10 #Allowable error (in seconds) between SpaceTime clock and system clock

def UpdateDoorStatus(vhsApi, closing_time):
  #Update VHSApi only if it's changed
  if closing_time == None:
    vhsApi.UpdateIfNecessary(api_var_door, 'closed')
    vhsApi.UpdateIfNecessary(api_var_closing, 'closed')
  else:
    vhsApi.UpdateIfNecessary(api_var_door, 'open')
    #Removing seconds part, and replacing colons with underscores
    #because VHS Api is currently ignoring most symbols.
    closing_time = closing_time[:2] + '_' + closing_time[3:5]
    vhsApi.UpdateIfNecessary(api_var_closing, closing_time)
    
def ProcessSerialMsg(msg, vhsApi, st):
  
  if msg.type == 'Current' or msg.type == 'AmbiguousTime':
    #SpaceTime is telling us what it thinks is the current time
    #It's telling us either because the user just set it, or
    #because we asked it.
    #Currently, we never ask for the Closing time, so if we receive
    #an AmbiguousTime, we can safely assume it is the Current time.

    global lastClockSync
    lastClockSync = time.time()
    curTime = time.localtime(lastClockSync)
    
    shouldUpdate = True
    if msg.val != None:
      #Check if SpaceTime time is close to current system time
      SpaceTimeDrift = TimeOffsetSeconds(curTime, StrToTime(msg.val))
      #Don't bother updating SpaceTime if time is within 10s of system time
      shouldUpdate = abs(SpaceTimeDrift) > max_clock_drift
    if shouldUpdate:
      print('Synchronizing SpaceTime\'s clock to ' + TimeToStr(curTime))
      #SpaceTime Current clockID = 0
      st.SetTime(0, curTime)
    
  elif msg.type == 'Closing':
    #SpaceTime is telling us the status of closing time.
    #It's telling us because the user just set it, because
    #we asked for it, or because it just expired.
    
    print('SpaceTime reports that Closing time is ' + ('not set' if msg.val == None else msg.val))
    #Update VHSApi only if it's changed
    UpdateDoorStatus(vhsApi, msg.val)
  elif msg.type == 'OK':
    return #Can ignore 'OK' responses
  elif msg.type == 'Echo':
    return #SpaceTime echoes all commands sent to it, so we can ignore these
  elif msg.type == 'Boot':
    print('SpaceTime has just been reset!')
    print('Resetting VHS Api variables and setting SpaceTime\'s clock')
    UpdateDoorStatus(vhsApi, None)
    #Query SpaceTime's clock. Its response will trigger us to update it if necessary.
    st.GetTime(0)
    return
  else:
    print('Serial message ignored: "' + msg.val + '"')

def GetLocalIP():
  #Returns the machine's local IP address as a string, or 'unknown' if error.
  ip = 'unknown'
  try:
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(('8.8.8.8', 80))
    ip = s.getsockname()[0]
    s.close()
  except Exception as e:
    print('IP lookup failed: ', e)
  return ip

def ShouldSyncClock():
  #Sync the clock every 24h, and do it when we're around 
  #40-50s in the current minute (to minimize the chance of the clock
  #jumping back 1min or forward 2min).
  _24h = 86400 #seconds in 24h
  curTime = time.time()
  if curTime - lastClockSync > _24h:
    curTime = time.localtime(curTime)
    if 40 < curTime.tm_sec < 50:
      return True
  return False

def setup():
  #Connects to the internet, updates local IP address 
  #on VHS Api, connects to SpaceTime Serial, updates
  #VHS Api variables if necessary, and queries
  #SpaceTime's clock (to trigger an update upon its response).
  #returns initialized (VHSApi, SpaceTime)
  
  print('Initializing SpaceTime...')
  vhs = VHSApi()
  st = SpaceTime()

  print('Connecting to the internet...')
  vhs.WaitForConnect(api_var_door)
  print('Connected!')
  #Update the machine's local IP on the VHS Api. The timestamp can serve as a boot history.
  #Replacing periods with dashes, because VHS Api is currently ignoring most symbols.
  vhs.Update(api_var_ip, GetLocalIP().replace('.', '-'))
  
  print('Initializing Serial connection with SpaceTime (' + st.serial.name + ')...')
  while not st.IsConnected():
    print('Failed to init Serial connection with SpaceTime. Trying again...')
  print('Initialized!')
  
  #Query Closing time (this is the only time we do this)
  #in case RPi was rebooted but SpaceTime wasn't.
  st.GetTime(1)    #Closing time is ID 1
  st.Read()        #Ignore the echo of the GetTime command
  ct = st.Read()   #Read the response
  #It is an unlabeled time, but we know it should be Closing time
  if ct.type == 'AmbiguousTime':
    ct.type = 'Closing'
    #Update VHS Api if necessary
    ProcessSerialMsg(ct, vhs, st)
  
  #Query SpaceTime's clock. Its response will trigger us to update it if necessary.
  st.GetTime(0)  #Current time is ID 0
  return vhs, st

def loop(vhs, st):
  #Check for new Serial messages every second.
  #If there is a Serial message to read, read and process it.
  if st.CanRead():
    msg = st.Read()
    if dbg_showAllSerial:
      dbgmsg = 'SerialDbg ' + msg.type + ': ' + str(msg.val)
      print(dbgmsg if not dbgmsg.endswith('\r\n') else dbgmsg[:-2])
    ProcessSerialMsg(msg, vhs, st)
  elif ShouldSyncClock():
    #Query SpaceTime's clock. Its response will trigger us to update it if necessary.
    st.GetTime(0)
    #Give SpaceTime enough time to respond so that we only send one st.GetTime(0) per sync period.
    time.sleep(0.5)
  else:
    time.sleep(1)

def main():
  #Run setup and then loop indefinitely
  vhs, st = setup()
  
  #Loop indefinitely - Catch and report any unhandled exceptions,
  #but try to keep going anyway.
  while 1:
    try:
      loop(vhs, st)
    except Exception as e:
      print('Exception in main loop! ', e)
      #Give some time for whatever caused the error to go away.
      #Also don't want to flood a log file with identical exceptions.
      time.sleep(5)

if __name__ == '__main__': 
  main()