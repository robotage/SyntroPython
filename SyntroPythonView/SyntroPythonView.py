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

# make the serviceName empty to find first source or else specify the path to the desired source
# serviceName will be the host name of the source by default

serviceName = ""

# Now do the main imports

import SyntroPython
import SyntroPythonPy
import sys
import cv2
import numpy
import time

# start SyntroPython running
SyntroPython.start("SyntroPythonView", sys.argv)

# this delay is necessary to allow Qt startup to complete
time.sleep(1)

# put console into single character mode
SyntroPython.startConsoleInput()

# wake up the console
print("SyntroPythonView starting...")
print("Enter command: "),
sys.stdout.flush()

# preset some global vars
connected = False
gotSource = False;
servicePort = -1;

while(True):
    if (SyntroPython.isConnected()):
        # do connected state processing
        if (not connected):
            # see if we need to get the directory or if the name is fixed
            if (serviceName == ""):
                SyntroPython.requestDirectory()
            connected = True
            
        # see if we are waiting to get service path
        
        if (serviceName == ""):
            sourceList = SyntroPython.lookupMulticastSources(SyntroPythonPy.SYNTRO_STREAMNAME_AVMUX)
            if (len(sourceList) > 0):
                sources = sourceList.splitlines()
                serviceName = sources[0]
            
        # see if we need to activate the service
        
        if ((serviceName != "") and (servicePort == -1)):
            servicePort = SyntroPython.addMulticastSink(serviceName + 
                    SyntroPythonPy.SYNTRO_SERVICEPATH_SEP + SyntroPythonPy.SYNTRO_STREAMNAME_AVMUX)
            # set the title if in GUI mode
            SyntroPython.setWindowTitle("Stream from " + serviceName)
           
        if (servicePort != -1):
            ret, timestamp, videoData, audioData = SyntroPython.getAVData(servicePort);
            if (ret):
                if (videoData != None):
                    numpyImage = numpy.frombuffer(videoData, dtype = 'uint8')
                    image = cv2.imdecode(numpyImage, 1)
                    timestring = time.strftime("%a, %d %b %Y %H:%M:%S", time.localtime(float(timestamp) / 1000.0)) + \
                        (".%03d" % (timestamp % 1000))
                    SyntroPython.displayImage(image.tostring(), image.shape[1], image.shape[0], timestring)

    else:
        if (connected):
            connected = False
      
    # process any user console input that might have arrived.   
       
    c = SyntroPython.getConsoleInput()
    if (c != None):
        print("\n")
        sc = chr(c)
        if (sc == "x"):
            break
        elif (sc == "d"):
            print("*** Source directory:")
            print(SyntroPython.lookupMulticastSources(SyntroPythonPy.SYNTRO_STREAMNAME_AVMUX))
        elif (sc == "s"):
            print("Status:")
            if (connected):
                print("SyntroLink connected")
            else:
                print("SyntroLink closed")
        elif (sc == 'h'):
            print("Available commands are:")
            print("  d - display list of sources")
            print("  s - display status")
            print("  x - exit")
            
        print("\nEnter command: "),
        sys.stdout.flush()
    
# Exiting so clean everything up.    

if (servicePort != -1):
    SyntroPython.removeService(servicePort)
SyntroPython.stop()
print("Exiting")
