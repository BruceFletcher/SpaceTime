import requests
from time import sleep

class WebApi:
  # The isvhsopen.com Web API is at http://isvhsopen.com/api/status/
  # It is different from the VHS API at http://api.vanhack.ca/
  # Source code is at https://github.com/vhs/isvhsopen
  # An API Key must be obtained and added to this file to make updates possible.
  
  #Example queries:
  #GET http://isvhsopen.com/api/status/
  #POST http://isvhsopen.com/api/status/open?key=ISVHSOPEN_API_KEY&until=12:30
  #POST http://isvhsopen.com/api/status/closed?key=ISVHSOPEN_API_KEY
  
  def __init__(self, dataURL = 'http://isvhsopen.com/api/status/', apiKey = 'ISVHSOPEN_API_KEY', timeout = 5):
    self.baseURL = dataURL
    self.apiKey = apiKey
    self.timeout = timeout
    
  def WaitForConnect(self):
    #Periodically queries the API until it receives a successful response.
    sleepAmt = .25
    sleepMax = 1616
    while self.Query() == False:
      print('Waiting ' + str(sleepAmt) + 's for retry...')
      sleep(sleepAmt)
      if sleepAmt < sleepMax:
        sleepAmt *= 2
  
  def Query(self, dataname = None):
    #Returns the value of dataname from the isvhsopen server, or False if query failed.
    #If dataname is None or '', returns the full json response, or False if query failed.
    try:
      #Query should return a json object with all status info
      r = requests.get( self.baseURL , timeout = self.timeout )
      if r.status_code == requests.codes.ok:
        #Expected json response in format:
        #{"status":"open","last":"2015-12-06T20:05:17.669Z","_events":{"change":[null,null]},"_eventsCount":1,"openUntil":"2015-12-07T12:32:00.000Z"}
        #{"status":"closed","last":"2015-12-06T20:07:09.232Z","_events":{"change":[null,null]},"_eventsCount":1}
        j = r.json()
        if (dataname):
          val = j.get(dataname)
          #val is the value of dataname, or None if no such dataname exists
          if (val):
            print('isvhsopen Query "' + dataname + '" is "' + val + '"')
            return val
        else:
          #No dataname was specified, so return full json response
          return j
      print('isvhsopen Query' + (' of "' + dataname + '"' if dataname != None else '') + ' failed.')
      print('Response code: ' + str(r.status_code))
      print('Response text: ' + r.text)
    except Exception as e:
      print('isvhsopen Query failed: ', str(e))
    return False
  
  def Update(self, doorStatus, until = ''):
    #Updates the isvhsopen.com WebAPI with the current door status.
    #Valid values for doorStatus are 'open' and 'closed'
    #The until parameter is ignored by the server if doorStatus == 'closed'
    #Returns the json response object from the update if successful, or False if failed.
    try:
      d = { 'key': self.apiKey, 'until': until }
      p = requests.post( self.baseURL + doorStatus , data = d, timeout = self.timeout )
      if p.status_code == requests.codes.ok:
        #Expected json response in format:
        #{"result":"ok","status":"open","last":"2015-12-06T20:05:17.669Z","openUntil":"2015-12-07T12:32:00.000Z"}
        #{"result":"ok","status":"closed","last":"2015-12-06T20:07:09.232Z"}
        j = p.json()
        if (j['result'] == 'ok' and j['status'] == doorStatus):
          #Note we're just trusting that openUntil was set correctly
          print('isvhsopen Update door status to "' + doorStatus + '" until "' + str(j.get('openUntil')) + '"')
          return j
      print('isvhsopen Update door status to "' + doorStatus + '" failed.')
      print('Response code: ' + str(p.status_code))
      print('Response text: ' + p.text)
    except Exception as e:
      print('isvhsopen Update failed: ' + str(e))
    return False
