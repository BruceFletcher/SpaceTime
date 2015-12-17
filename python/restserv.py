import web
from timeutil import *

#SpaceTime object, defined when RestServ() is called
spacetime = None
  
urls = (
  '/', 'index',
  r'/set/open/(\d\d?):?(\d\d)', 'setopen',
  '/set/closed?/?', 'setclosed'
)

#Initializes a webserver with a restful API to control the SpaceTime board.
#Intended to be made available on the local VHS network, but not over the internet.
#Parameter st should be an initialized SpaceTime object.
def RestServ(st):
  global spacetime
  spacetime = st
  app = web.application(urls, globals())
  app.run()

class index:
  def GET(self):
    return "SpaceTime REST API\r\n" \
       + "/set/open/15:30 - Sets SpaceTime to stay open until 15:30\r\n" \
       + "/set/closed     - Sets SpaceTime to closed"

class setopen:
  def GET(self, hours, mins):
    #Make a HH:MM:SS time string
    timestr = hours.zfill(2) + ":" + mins + ":00"
    if IsTimeStr(timestr):
      #Closing time is ID 1
      spacetime.SetTime(1, StrToTime(timestr))
      return "SpaceTime set to " + timestr + "."
    return timestr + " is not a valid time."

class setclosed:
  def GET(self):
    #Closing time is ID 1
    spacetime.ClearTime(1)
    return "SpaceTime set to closed."