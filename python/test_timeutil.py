import unittest
from timeutil import *

#To run these unit tests from command line:
#python -m unittest test_timeutil

fmt = "%H:%M:%S"
t000000 = time.strptime('00:00:00', fmt)
t041000 = time.strptime('04:10:00', fmt)
t080020 = time.strptime('08:00:20', fmt)
t225035 = time.strptime('22:50:35', fmt)

class TestTimeUtil(unittest.TestCase):
  
  def test_StrToTime(self):
    t = StrToTime('00:00:00')
    t2 = time.strptime('','')
    self.assertEqual(t, t2)
    t = StrToTime('14:56:05')
    t2 = time.strptime('14:56:05', fmt)
    self.assertEqual(t, t2)
    
  def test_TimeToStr(self):
    t = time.gmtime(0)
    self.assertEqual(TimeToStr(t), '00:00:00')
    t = time.gmtime(1432757205)
    self.assertEqual(TimeToStr(t), '20:06:45')

  def test_TimeToSeconds_Zero(self):
    t = time.gmtime(0)
    self.assertEqual(TimeToSeconds(t), 0)
    
  def test_TimeToSeconds(self):
    t = time.gmtime(1432757205)
    self.assertEqual(TimeToSeconds(t), 72405)
  
  def test_TimeOffsetSeconds_Equal(self):
    t = time.gmtime(0)
    self.assertEqual(TimeOffsetSeconds(t, t000000), 0)

  def test_TimeOffsetSeconds_Pos_Within(self): #Offset is within the current day
    self.assertEqual(TimeOffsetSeconds(t041000, t080020), 13820)
    
  def test_TimeOffsetSeconds_Neg_Within(self):
    self.assertEqual(TimeOffsetSeconds(t080020, t041000), -13820)
    
  def test_TimeOffsetSeconds_Pos_Across(self): #Offset is across two days
    self.assertEqual(TimeOffsetSeconds(t225035, t000000), 4165)
    
  def test_TimeOffsetSeconds_Neg_Across(self):
    self.assertEqual(TimeOffsetSeconds(t000000, t225035), -4165)
  
  def test_IsTimeStr(self):
    self.assertTrue(IsTimeStr('00:00:00'))
    self.assertTrue(IsTimeStr('02:34:56'))
    self.assertTrue(IsTimeStr('21:54:77')) #Not a valid time, but we're Ok with letting this through
    self.assertFalse(IsTimeStr('12:12'))   #Need seconds
    self.assertFalse(IsTimeStr('2:12:34')) #Need two digits
    self.assertFalse(IsTimeStr(' 2:34:56'))
    self.assertFalse(IsTimeStr('12.34:56'))
    self.assertFalse(IsTimeStr('12:34 56'))
    self.assertFalse(IsTimeStr('12:34:5o'))
  
if __name__ == '__main__':
  unittest.main()