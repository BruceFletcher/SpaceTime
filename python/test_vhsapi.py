import unittest
from vhsapi import VHSApi

#To run these unit tests from command line:
#python -m unittest test_vhsapi

class TestVHSApi(unittest.TestCase):
  
  #----Query----
  
  def test_VHSApi_Query_Success(self):
    v = VHSApi()
    r = v.Query('door')
    self.assertTrue(r == 'open' or r == 'closed')

  def test_VHSApi_Query_Timeout(self):
    v = VHSApi(timeout=0)
    self.assertFalse(v.Query('door'))
    
  def test_VHSApi_Query_ConnectionError(self):
    v = VHSApi(dataURL = 'http://doesnotexist.hackspace.ca/')
    self.assertFalse(v.Query('door'))
    
  def test_VHSApi_Query_NoJSON(self):
    v = VHSApi(dataURL = 'http://www.google.ca/?q=')
    self.assertFalse(v.Query('door'))
    
  def test_VHSApi_Query_BadUrl(self):
    v = VHSApi(dataURL = 'http://api.hackspace.ca/')
    self.assertFalse(v.Query('door'))
    
  #----Update----
    
  def test_VHSApi_Update_Success(self):
    v = VHSApi()
    self.assertTrue('v1' == v.Update('test1', 'v1') == v.Query('test1'))
    self.assertTrue('v2' == v.Update('test1', 'v2') == v.Query('test1'))
    
  def test_VHSApi_Update_Timeout(self):
    v = VHSApi(timeout=0)
    self.assertFalse(v.Update('test1', 'v1'))
    
  def test_VHSApi_Update_ConnectionError(self):
    v = VHSApi(dataURL = 'http://doesnotexist.hackspace.ca/')
    self.assertFalse(v.Update('test1', 'v1'))
    
  def test_VHSApi_Update_NoJSON(self):
    v = VHSApi(dataURL = 'http://www.google.ca/?q=')
    self.assertFalse(v.Update('test1', 'v1'))
    
  def test_VHSApi_Update_BadUrl(self):
    v = VHSApi(dataURL = 'http://api.hackspace.ca/')
    self.assertFalse(v.Update('test1', 'v1'))
  
  
if __name__ == '__main__':
  unittest.main()