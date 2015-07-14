import time

timeFormatStr = "%H:%M:%S"

def StrToTime(str):
  return time.strptime(str, timeFormatStr)

def TimeToStr(t):
  return time.strftime(timeFormatStr, t)
  
def TimeToSeconds(t):
  return t.tm_sec + 60*(t.tm_min + 60*t.tm_hour)

def TimeOffsetSeconds(t1, t2):
  #Returns the offset between two times (ignoring dates), in seconds.
  #Will choose the shortest offset, even if it crosses a date boundry,
  #so the result will always be within -12h and +12h.
  _12h = 43200 #Number of seconds in 12 hours
  _24h = 86400
  s1 = TimeToSeconds(t1)
  s2 = TimeToSeconds(t2)
  diff = s2 - s1
  
  #if abs(diff) is greater than 12h, then the diff across a day boundary will be smaller
  if diff > _12h:
    diff -= _24h;
  elif diff < -_12h:
    diff += _24h;
  return diff

def IsTimeStr( str ):
  #Basic test for time-formatted string, returns True if format matches "dd:dd:dd"
  #where d is any digit.
  if len(str) == 8 and ':' == str[2] == str[5]:
    for i in [0, 1, 3, 4, 6, 7]:
      if not '0' <= str[i] <= '9':
        return False
    return True
  return False
  