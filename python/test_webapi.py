import unittest
from webapi import WebApi

#To run these unit tests from command line:
#python -m unittest test_webapi

#Note that the Update tests actually change the live status. It leaves the status as offline.

class TestWebApi(unittest.TestCase):
  
  #----Query----
  
  def test_WebApi_Query_Success(self):
    v = WebApi()
    r = v.Query('status')
    self.assertTrue(r == 'open' or r == 'closed')

  def test_WebApi_Query_Timeout(self):
    v = WebApi(timeout=0)
    self.assertFalse(v.Query('status'))
    
  def test_WebApi_Query_ConnectionError(self):
    #If querying a different server, we shouldn't be using our actual API key!
    v = WebApi(dataURL = 'http://doesnotexist.isvhsopen.com/', apiKey = 'BADBADBAD')
    self.assertFalse(v.Query('status'))
    
  def test_WebApi_Query_NoJSON(self):
    #If querying a different server, we shouldn't be using our actual API key!
    v = WebApi(dataURL = 'http://www.google.ca/?q=', apiKey = 'BADBADBAD')
    self.assertFalse(v.Query('status'))
    
  def test_WebApi_Query_BadUrl(self):
    v = WebApi(dataURL = 'http://isvhsopen.com/')
    self.assertFalse(v.Query('status'))
    
  #----Update----
    
  def test_WebApi_Update_Success(self):
    #Note that this only checks the response json to ensure it matches
    #the update; we don't wait and then query to verify, but just trust it.
    v = WebApi()
    j = v.Update('open', '12:34')
    self.assertTrue(j['status'] == 'open' and j['openUntil'].find('12:34:00') == 11)
    j = v.Update('closed')
    self.assertTrue(j['status'] == 'closed' and j.get('openUntil') == None)
    
  def test_WebApi_Bad_Key(self):
    v = WebApi(apiKey = 'BADBADBAD')
    self.assertFalse(v.Update('open', '12:34'))
  
  def test_WebApi_Update_Timeout(self):
    v = WebApi(timeout=0)
    self.assertFalse(v.Update('open', '12:34'))
    
  def test_WebApi_Update_ConnectionError(self):
    #If querying a different server, we shouldn't be using our actual API key!
    v = WebApi(dataURL = 'http://doesnotexist.isvhsopen.com/', apiKey = 'BADBADBAD')
    self.assertFalse(v.Update('open', '12:34'))
    
  def test_WebApi_Update_NoJSON(self):
    #If querying a different server, we shouldn't be using our actual API key!
    v = WebApi(dataURL = 'http://www.google.ca/?q=', apiKey = 'BADBADBAD')
    self.assertFalse(v.Update('test1', 'v1'))
    
  def test_WebApi_Update_BadUrl(self):
    v = WebApi(dataURL = 'http://isvhsopen.com/')
    self.assertFalse(v.Update('test1', 'v1'))
  
  
if __name__ == '__main__':
  unittest.main()