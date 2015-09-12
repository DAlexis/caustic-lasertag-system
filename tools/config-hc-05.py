#!/usr/bin/python

import serial
from time import gmtime, strftime, sleep
import sys
import os
from optparse import OptionParser

cmdLineParser = OptionParser()
cmdLineParser.add_option("-d", "--device", dest="device", default="/dev/ttyUSB0",
                  help="Serial port HC-05 connected to", metavar="PORT")
cmdLineParser.add_option("-b", "--baudrate", dest="baudrate", default="38400",
                  help="Serial port baudrate", metavar="PORT")

(cmdLineOptions, cmdLineArgs) = cmdLineParser.parse_args()

ser = serial.Serial()
ser.baudrate = cmdLineOptions.baudrate
ser.port = cmdLineOptions.device

try:
    ser.open()
    print("Setting UART speed to 115200 b/s, 1 stop bit, 0 parity")
    ser.write(b"AT+UART=115200,1,0\r\n"); 
    print("Configuring device name")
    ser.write(b"AT+NAME=nrfBridge\r\n");
    
    print("All done")
except serial.SerialException as ex:
    print("Serial port errer, check the parameters")
    raise
except:
    print("Unexpected error:", sys.exc_info()[0])
    raise    
finally:
    ser.close()
