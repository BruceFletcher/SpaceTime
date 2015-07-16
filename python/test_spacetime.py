import unittest
import serial
from timeutil import *
from spacetime import *

#To run these unit tests from command line:
#python -m unittest test_spacetime

class TestSpaceTime(unittest.TestCase):
  
  def test_ClearSerial(self):
    st = SpaceTime()
    st.ClearSerial()
    self.assertEqual(st.serial.inWaiting(), 0)
    st.serial.write('AT\r\nRandom')
    time.sleep(0.25)
    #SpaceTime should echo 'AT\r\nRandom' and respond to AT with 'OK\r\n', hence 14 characters.
    self.assertEqual(st.serial.inWaiting(), 14)
    #Remaining in SpaceTime's input buffer should be 'Random', but we have no way to test that.
    #Clear all buffers
    st.ClearSerial()
    #Verify input buffer is empty
    self.assertEqual(st.serial.inWaiting(), 0)
    #Verify SpaceTime's buffer by sending and receiving some commands.
    st.serial.write('AT' + CRLF)
    self.assertEqual(st.serial.readline(), 'AT' + CRLF)
    self.assertEqual(st.serial.readline(), 'OK' + CRLF)
    
  def test_CanRead(self):
    st = SpaceTime()
    st.ClearSerial()
    self.assertFalse(st.CanRead())
    st.serial.write('AT')
    time.sleep(0.25)
    self.assertTrue(st.CanRead())
    st.serial.read() #Read 'A'
    self.assertTrue(st.CanRead())
    st.serial.read() #Read 'T'
    self.assertFalse(st.CanRead())
    st.serial.write(CRLF)
    time.sleep(0.25)
    self.assertTrue(st.CanRead())
    st.serial.readline() #Read '\r\n'
    st.serial.readline() #Read 'OK\r\n'
    self.assertFalse(st.CanRead())
    
  def test_IsConnected_Success(self):
    st = SpaceTime()
    st.ClearSerial()
    self.assertTrue(st.IsConnected())
    #Make sure the buffers aren't polluted from IsConnected().
    st.serial.write(CRLF)
    self.assertFalse(st.CanRead())
    
  def test_IsConnected_Timeout(self):
    st = SpaceTime()
    st.ClearSerial()
    self.assertFalse(st.IsConnected(timeout = 0))
    st.ClearSerial()
    
  def test_SerialCommand(self):
    st = SpaceTime()
    st.ClearSerial()
    st.SerialCommand('AT')
    self.assertEqual(st.serial.readline(), 'AT' + CRLF)
    self.assertEqual(st.serial.readline(), 'OK' + CRLF)
    
  def test_SetTime(self):
    st = SpaceTime()
    st.ClearSerial()
    t = '14:56:05'
    st.SetTime(0, StrToTime(t))
    self.assertEqual(st.serial.readline(), 'ATST0=14:56:05' + CRLF)
    self.assertEqual(st.serial.readline(), 'Current time: 14:56:05' + CRLF)
    
  def test_GetTime(self):
    st = SpaceTime()
    st.ClearSerial()
    st.serial.write('ATST0=14:56:05' + CRLF)
    st.serial.readline()  #Read echo of above command
    st.serial.readline()  #Read current time response
    st.serial.readline()  #Read OK
    st.GetTime(0)
    self.assertEqual(st.serial.readline(), 'ATST0?' + CRLF)
    self.assertEqual(st.serial.readline(), '14:56:05' + CRLF)
    
  def test_ClearTime(self):
    st = SpaceTime()
    st.ClearSerial()
    st.ClearTime(1)
    self.assertEqual(st.serial.readline(), 'ATST1=X' + CRLF)
    self.assertEqual(st.serial.readline(), 'Closing time: Not set' + CRLF)
    
  #----Read----
    
  def test_Read_EchoText(self):
    st = SpaceTime()
    st.ClearSerial()
    st.ClearTime(1)
    r = st.Read()
    self.assertEqual(r.type, 'Echo')
    self.assertEqual(r.val, 'ATST1=X' + CRLF)
    
  def test_Read_OK(self):
    st = SpaceTime()
    st.ClearSerial()
    st.SerialCommand('AT')
    st.serial.readline()  #Read echo of above command
    r = st.Read()
    self.assertEqual(r.type, 'OK')
    self.assertEqual(r.val, 'OK' + CRLF)
  
  def test_Read_Time_Set(self):
    st = SpaceTime()
    st.ClearSerial()
    t = '14:56:05'
    st.SetTime(0, StrToTime(t))
    st.serial.readline()  #Read echo of above command
    r = st.Read()         #Read response to SetTime command
    #Response to SetTime specifies clock name 'Current'
    self.assertEqual(r.type, 'Current')
    self.assertEqual(r.val, t)
    st.serial.readline()  #Read OK
    st.GetTime(0)
    st.serial.readline()  #Read echo of above command
    r = st.Read()         #Read response to GetTime command
    #Ambiguous because response to GetTime doesn't specify clock name
    self.assertEqual(r.type, 'AmbiguousTime')
    self.assertEqual(r.val, t)
    
  def test_Read_Time_Not_Set(self):
    st = SpaceTime()
    st.ClearSerial()
    st.ClearTime(1)
    st.serial.readline()  #Read echo of above command
    r = st.Read()         #Read response to ClearTime command
    #Response to SetTime specifies clock name 'Closing'
    self.assertEqual(r.type, 'Closing')
    self.assertEqual(r.val, None)
    st.serial.readline()  #Read OK
    st.GetTime(1)
    st.serial.readline()  #Read echo of above command
    r = st.Read()         #Read response to GetTime command
    #Ambiguous because response to GetTime doesn't specify clock name
    self.assertEqual(r.type, 'AmbiguousTime')
    self.assertEqual(r.val, None)
    
  def test_Read_Unknown(self):
    st = SpaceTime()
    st.ClearSerial()
    st.SerialCommand('Pizza')
    r = st.Read()
    self.assertEqual(r.type, 'Unknown')
    self.assertEqual(r.val, 'Pizza' + CRLF)
    st.ClearSerial()
    
  def test_Read_Boot(self):
    #This is something that the Serial device normally sends to us,
    #but since it can't be triggered by software, we're mimicing it
    #here by using the fact that SpaceTime echoes all commands we send.
    #Normally we would never send 'SpaceTime, yay!' to the board.
    st = SpaceTime()
    st.ClearSerial()
    st.SerialCommand('SpaceTime, yay!')
    r = st.Read()
    self.assertEqual(r.type, 'Boot')
    self.assertEqual(r.val, 'SpaceTime, yay!' + CRLF)
    st.ClearSerial()
    
  def test_Read_Timeout(self):
    st = SpaceTime()
    st.ClearSerial()
    st.serial.timeout = 0
    r = st.Read()
    self.assertEqual(r.type, 'Unknown')
    self.assertEqual(r.val, '')
  
if __name__ == '__main__':
  unittest.main()