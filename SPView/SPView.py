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

# make the serviceName empty to find first source or else specify the path to the desired source
# serviceName will be the host name of the source by default

serviceName = ""

# Now do the main imports

import SyntroPython
import SyntroPythonPy
import sys
import time

# start SyntroPython running
SyntroPython.start("SPView", sys.argv, True)

# this delay is necessary to allow Qt startup to complete
time.sleep(1)

# put console into single character mode
SyntroPython.startConsoleInput()

# wake up the console
print("SPView starting...")
print("Enter command: "),
sys.stdout.flush()

# preset some global vars
connected = False
gotSource = False
servicePort = -1

while(True):
    # give other things a chance
    time.sleep(0.02)
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
            ret, timestamp, videoData, audioData = SyntroPython.getAVData(servicePort)
            if (ret):
                if (videoData != None):
                    timestring = time.strftime("%a, %d %b %Y %H:%M:%S", time.localtime(float(timestamp) / 1000.0)) + \
                        (".%03d" % (timestamp % 1000))
                    SyntroPython.displayJpegImage(videoData, timestring)

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
