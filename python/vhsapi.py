import requests
from time import sleep

class VHSApi:
  # The VHS API runs at http://api.hackspace.ca/
  # Source code is at   https://github.com/vhs/api/blob/master/lib/VHSAPI.pm
  # The online API supports a wide range of queries. (But this code does not)
  #  '<hostname>/s/<spacename>/data/<dataname>/update?value=<datavalue>'
  #  '<hostname>/s/<spacename>/data/<dataname>/fullpage'
  #  '<hostname>/s/<spacename>/data/<dataname>'
  #  '<hostname>/s/<spacename>/data/<dataname>.json'
  #  '<hostname>/s/<spacename>/data/<dataname>.txt'
  #  '<hostname>/s/<spacename>/data/<dataname>.js'
  #  '<hostname>/s/<spacename>/data/<dataname>/feed'
  #  '<hostname>/s/<spacename>/data/history/<dataname>.json'
  api_update_str = '/update?value='
  
  def __init__(self, dataURL = 'http://api.hackspace.ca/s/vhs/data/', timeout = 5):
    self.baseURL = dataURL
    self.timeout = timeout
  
  def WaitForConnect(self, dataname):
    #Periodically queries the API until it receives a successful response.
    #dataname is the variable we query the API for.
    sleepAmt = .25
    sleepMax = 16
    while self.Query(dataname) == False:
      print('Waiting ' + str(sleepAmt) + 's for retry...')
      sleep(sleepAmt)
      if sleepAmt < sleepMax:
        sleepAmt *= 2

  def UpdateIfNecessary(self, dataname, datavalue):
    #Query the dataname to see if it matches datavalue, and then
    #update with datavalue if they are different. This prevents
    #needless writes, which is important because each API variable
    #has a timestamp that is updated every time it changes.
    
    q = self.Query(dataname)
    if q != False and q != datavalue:
      self.Update(dataname, datavalue)
  
  def Query(self, dataname):
    #Returns the value of dataname from the VHSApi server, or False if query failed.
    try:
      r = requests.get( self.baseURL + dataname + '.json' , timeout = self.timeout )
      if r.status_code == requests.codes.ok:
        #Expected json response in format:
        #{"last_updated":<unixtimestamp>,"name":"<dataname>","value":"<datavalue>"}
        j = r.json()
        if (j['name'] == dataname):
          print('VHSApi Query  "' + dataname + '" is "' + j['value'] + '"')
          return j['value']
      print('VHSApi Query of "' + dataname + '" failed.')
      print('Response code: ' + str(r.status_code))
      print('Response text: ' + r.text)
    except Exception as e:
      print('VHSApi Query failed: ', str(e))
    return False
  
  def Update(self, dataname, datavalue):
    try:
      r = requests.get( self.baseURL + dataname + self.api_update_str + datavalue , timeout = self.timeout )
      if r.status_code == requests.codes.ok:
        #Expected json response in format:
        #{"result":{"value":"<datavalue>","last_updated":<unixtimestamp>,"name":"<dataname>"},"status":"OK"}
        j = r.json()
        if (j['status'] == 'OK' and j['result']['name'] == dataname and j['result']['value'] == datavalue):
          print('VHSApi Update "' + dataname + '" to "' + datavalue + '"')
          return datavalue
      print('VHSApi Update of "' + dataname + '" failed.')
      print('Response code: ' + str(r.status_code))
      print('Response text: ' + r.text)
    except Exception as e:
      print('VHSApi Update failed: ' + str(e))
    return False