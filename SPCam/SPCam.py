'''
////////////////////////////////////////////////////////////////////////////
//
//  This file is part of SyntroPython
//
//  Copyright (c) 2014, richards-tech
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of 
//  this software and associated documentation files (the "Software"), to deal in 
//  the Software without restriction, including without limitation the rights to use, 
//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the 
//  Software, and to permit persons to whom the Software is furnished to do so, 
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all 
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'''

# camera parameters - change as required

CAMERA_INDEX = 0
CAMERA_WIDTH = 640
CAMERA_HEIGHT = 480
CAMERA_RATE = 10

# Now onto the real stuff

import SyntroPython
import SyntroPythonPy
import sys
import time

# start SyntroPython running
SyntroPython.start("SyntroPythonCam", sys.argv, True)

# this delay is necessary to allow Qt startup to complete
time.sleep(1)

# Open the camera device
if (not SyntroPython.vidCapOpen(CAMERA_INDEX, CAMERA_WIDTH, CAMERA_HEIGHT, CAMERA_RATE)):
    print("Failed to open vidcap")
    SyntroPython.stop()
    sys.exit()

# put console into single character mode
SyntroPython.startConsoleInput()

# set the title if in GUI mode
SyntroPython.setWindowTitle(SyntroPython.getAppName() + " camera stream")

# wake up the console
print("SyntroPythonCam starting...")
print("Enter command: "),
sys.stdout.flush()

# Activate the stream
servicePort = SyntroPython.addMulticastSource(SyntroPythonPy.SYNTRO_STREAMNAME_AVMUX)
if (servicePort == -1):
    print("Failed to activate video service")
    SyntroPython.stop()
    sys.exit()

while(True):
    # give other things a chance
    time.sleep(0.02)
    # get a frame from the camera
    ret, frame, jpeg, width, height, rate = SyntroPython.vidCapGetFrame(CAMERA_INDEX)
    if (ret):            
        # and display it
        if (jpeg):
            SyntroPython.displayJpegImage(frame, "")
        else:
            SyntroPython.displayImage(frame, width, height, "")
        
        # now check if it can be sent on the SyntroLink
        if (SyntroPython.isServiceActive(servicePort) and SyntroPython.isClearToSend(servicePort)):
            # send the frame
            SyntroPython.setVideoParams(width, height, rate)
            pcm = str("")
            if (jpeg):
                SyntroPython.sendJpegAVData(servicePort, frame, pcm)
            else:
                SyntroPython.sendAVData(servicePort, frame, pcm)
                    
    # process any user console input that might have arrived.   
       
    c = SyntroPython.getConsoleInput()
    if (c != None):
        print("\n")
        sc = chr(c)
        if (sc == "x"):
            break
        elif (sc == "s"):
            print("Status:")
            if (SyntroPython.isConnected()):
                print("SyntroLink connected")
            else:
                print("SyntroLink closed")
        elif (sc == 'h'):
            print("Available commands are:")
            print("  s - display status")
            print("  x - exit")
            
        print("\nEnter command: "),
        sys.stdout.flush()
    
# Exiting so clean everything up.    

SyntroPython.vidCapClose(CAMERA_INDEX)
SyntroPython.removeService(servicePort)
SyntroPython.stop()
print("Exiting")
