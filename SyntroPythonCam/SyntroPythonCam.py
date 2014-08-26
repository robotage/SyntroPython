'''
  Copyright (c) 2014 richards-tech

  This file is part of SyntroNet

  SyntroNet is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  SyntroNet is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with SyntroNet.  If not, see <http://www.gnu.org/licenses/>.
'''

# camera parameters - change as required

CAMERA_INDEX = 0
CAMERA_WIDTH = 640
CAMERA_HEIGHT = 480
CAMERA_FPS = 10

# Now onto the real stuff

import SyntroPython
import SyntroPythonPy
import sys
import cv2
import numpy
import time

def startCamera():
    camera = cv2.VideoCapture(CAMERA_INDEX)
    if (not camera.isOpened()):
        print("No camera found")
        sys.exit()
    camera.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH, CAMERA_WIDTH)
    camera.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT, CAMERA_HEIGHT)
    camera.set(cv2.cv.CV_CAP_PROP_FPS, CAMERA_FPS)
    SyntroPython.setVideoParams(CAMERA_WIDTH, CAMERA_HEIGHT, CAMERA_FPS)
    return camera

# start SyntroPython running
SyntroPython.start("SyntroPythonCam", sys.argv)

# this delay is necessary to allow Qt startup to complete
time.sleep(1)

# Open the camera device
camera = startCamera()

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
    sys.exit()

while(True):
    # get a frame from the camera
    ret, frame = camera.read()
    if (ret):            
        # we have a frame so convert to string for display
        frameString = frame.tostring()
        
        # and display it
        SyntroPython.displayImage(frameString, CAMERA_WIDTH, CAMERA_HEIGHT, "");
        
        # now check if it can be sent on the SyntroLink
        if (SyntroPython.isServiceActive(servicePort) and SyntroPython.isClearToSend(servicePort)):
            
            # compress and send the frame
            encode_param = [cv2.IMWRITE_JPEG_QUALITY,80]
            ret, jpeg = cv2.imencode(".jpg", frame, encode_param)
            if (ret):
                pcm = numpy.ndarray(shape=(0, 1))
                SyntroPython.sendAVData(servicePort, jpeg, pcm)
                    
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

camera.release()
SyntroPython.removeService(servicePort)
SyntroPython.stop()
print("Exiting")
