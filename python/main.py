import time
import socket
from spacetime import SpaceTime
from vhsapi import VHSApi #api.hackspace.ca
from webapi import WebApi #isvhsopen.com/api/status/
from restserv import RestServ #Webserver for REST API to allow updates from the VHS network 
from timeutil import *

dbg_showAllSerial = False #If true, prints out all received serial messages
lastClockSync   = 0       #Time of last clock sync with SpaceTime
lastHeartbeat   = 0       #Time of last update with isvhsopen.com WebApi
doorStatus_cache= ''      #The last known door status, to send periodic heartbeat to WebApi
api_var_ip      = 'spacetime_ip'
max_clock_drift = 10 #Allowable error (in seconds) between SpaceTime clock and system clock

def UpdateDoorStatus(webApi, closing_time):
  #Save current time and door status to send periodic heartbeats to WebAPI
  global lastHeartbeat, doorStatus_cache
  lastHeartbeat = time.time()
  doorStatus_cache = closing_time
  #Update WebAPI with door status. WebAPI is smart enough
  #to ignore duplicate submissions, so unnecessary updates
  #aren't harmful and do not affect the timestamp.
  if closing_time == None:
    webApi.Update('closed')
  else:
    #Removing seconds part
    closing_time = closing_time[:5]
    webApi.Update('open', closing_time)
    
def ProcessSerialMsg(msg, webApi, st):
  
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
    #Update WebAPI
    UpdateDoorStatus(webApi, msg.val)
  elif msg.type == 'OK':
    return #Can ignore 'OK' responses
  elif msg.type == 'Echo':
    return #SpaceTime echoes all commands sent to it, so we can ignore these
  elif msg.type == 'Boot':
    print('SpaceTime has just been reset!')
    print('Resetting Web API variables and setting SpaceTime\'s clock')
    UpdateDoorStatus(webApi, None)
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
  
def ShouldSendHeartbeat():
  #Send a heartbeat to isvhsopen.com every 15min
  _15min = 900 #seconds in 15min
  curTime = time.time()
  return (curTime - lastHeartbeat > _15min)

def setup():
  #Connects to the internet, updates local IP address 
  #on VHS Api, connects to SpaceTime Serial, updates
  #Web Api variables, and queries SpaceTime's clock
  #(to trigger an update upon its response).
  #returns initialized (WebAPI, SpaceTime)
  
  print('Initializing SpaceTime...')
  vhs = VHSApi()
  web = WebApi()
  st = SpaceTime()

  print('Connecting to the internet...')
  web.WaitForConnect()
  print('Connected!')
  #Update the machine's local IP on the VHS Api. The timestamp can serve as a boot history.
  vhs.Update(api_var_ip, GetLocalIP())
  
  print('Initializing Serial connection with SpaceTime (' + st.serial.name + ')...')
  while not st.IsConnected():
    print('Failed to init Serial connection with SpaceTime. Trying again...')
  print('Initialized!')
  
  print('Initializing webserver for REST API (only available to LAN)')
  RestServ(st)
  
  #Query Closing time (this is the only time we do this)
  #in case RPi was rebooted but SpaceTime wasn't.
  st.GetTime(1)    #Closing time is ID 1
  st.Read()        #Ignore the echo of the GetTime command
  ct = st.Read()   #Read the response
  #It is an unlabeled time, but we know it should be Closing time
  if ct.type == 'AmbiguousTime':
    ct.type = 'Closing'
    #Update Web Api if necessary
    ProcessSerialMsg(ct, web, st)
  
  #Query SpaceTime's clock. Its response will trigger us to update it if necessary.
  st.GetTime(0)  #Current time is ID 0
  return web, st

def loop(web, st):
  #Check for new Serial messages every second.
  #If there is a Serial message to read, read and process it.
  if st.CanRead():
    msg = st.Read()
    if dbg_showAllSerial:
      dbgmsg = 'SerialDbg ' + msg.type + ': ' + str(msg.val)
      print(dbgmsg if not dbgmsg.endswith('\r\n') else dbgmsg[:-2])
    ProcessSerialMsg(msg, web, st)
  elif ShouldSyncClock():
    #Query SpaceTime's clock. Its response will trigger us to update it if necessary.
    st.GetTime(0)
    #Give SpaceTime enough time to respond so that we only send one st.GetTime(0) per sync period.
    time.sleep(0.5)
  elif ShouldSendHeartbeat():
    print('Sending Heartbeat to Web API...')
    UpdateDoorStatus(web, doorStatus_cache);
  else:
    time.sleep(1)

def main():
  #Run setup and then loop indefinitely
  web, st = setup()
  
  #Loop indefinitely - Catch and report any unhandled exceptions,
  #but try to keep going anyway.
  while 1:
    try:
      loop(web, st)
    except Exception as e:
      print('Exception in main loop! ', e)
      #Give some time for whatever caused the error to go away.
      #Also don't want to flood a log file with identical exceptions.
      time.sleep(5)

if __name__ == '__main__': 
  main()