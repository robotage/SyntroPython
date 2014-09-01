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

# DIRECTORY_INTERVAL controls the interval between directory fetches in seconds
DIRECTORY_INTERVAL = 5                  

# PLOT_INTERVAL sets the interval between plot display updates
PLOT_INTERVAL = 1.0

# Now do the imports

import sys

# add the sensor driver directory
sys.path.append('../SensorDrivers')

import SyntroPython
import SyntroPythonPy
import time
import json
import SensorJSON
import Sensor
import SensorPlot

# The service name map contains the mapping between service name and service port
serviceNameMap = {}

# The service port map contains the mapping between service port and sensor object
servicePortMap = {}

def processDirectory():
    ''' processDirectory checks current multicast sources in the directory against
    the list in serviceNameMap. If any are missing or new ones found, the map is adjusted accordingly. '''
    
    global serviceNameMap
    global servicePortMap
    
    sourceList = SyntroPython.lookupMulticastSources(SyntroPythonPy.SYNTRO_STREAMNAME_SENSOR)
    if (len(sourceList) == 0):
        sources = []
    else:
        sources = sourceList.splitlines()
 
    # check for new sources
    for source in sources:
        if (serviceNameMap.get(source) == None):
            # found a new source - activate
            servicePort = SyntroPython.addMulticastSink(source + 
                    SyntroPythonPy.SYNTRO_SERVICEPATH_SEP + SyntroPythonPy.SYNTRO_STREAMNAME_SENSOR)
            if (servicePort != -1):
                # successfully activated source so add to serviceMap
                serviceNameMap[source] = servicePort
                servicePortMap[servicePort] = Sensor.Sensor(source, servicePort)
                print("Added source %s" % source)
                
    # check for sources that have gone away
    for source in serviceNameMap.keys():
        try:
            sources.index(source)
        except:
            # source has gone missing so delete it
            servicePort = serviceNameMap.pop(source)
            SyntroPython.removeService(servicePort)
            servicePortMap.pop(servicePort)
            print("Removed source %s" % source)
    
    
def processSensorData():
    ''' For each active sensor stream, see if there's new data and handle it '''
    global serviceMap
    
    for servicePort in servicePortMap.keys():
        ret, data = SyntroPython.getMulticastData(servicePort)
        if (ret):
            servicePortMap[servicePort].newJSONData(data)
        

def processUserInput():
    # process any user console input that might have arrived.   
       
    c = SyntroPython.getConsoleInput()
    if (c != None):
        print("\n")
        sc = chr(c)
        if (sc == "x"):
            return False
        elif (sc == "d"):
            print("*** Source directory:")
            print(SyntroPython.lookupMulticastSources(SyntroPythonPy.SYNTRO_STREAMNAME_SENSOR))
        elif (sc == "s"):
            print("Status:")
            if (SyntroPython.isConnected()):
                print("SyntroLink connected")
            else:
                print("SyntroLink closed")
        elif (sc == 'h'):
            print("Available commands are:")
            print("  d - display list of sensors")
            print("  s - display status")
            print("  x - exit")
            
        print("\nEnter command: "),
        sys.stdout.flush()
        
    return True

# start SyntroPython running
SyntroPython.start("SensorNetView", sys.argv, False)

# this delay is necessary to allow Qt startup to complete
time.sleep(1)

# put console into single character mode
SyntroPython.startConsoleInput()

# wake up the console
print("SensorNetView starting...")
print("Enter command: "),
sys.stdout.flush()

# lastDirectoryTime is used to control directory fetching 
lastDirectoryTime = time.time() - DIRECTORY_INTERVAL

# start up the plotter

sensorPlot = SensorPlot.SensorPlot()

# lastPlotTime is used to control plot updates
lastPlotTime = time.time() - PLOT_INTERVAL

while(True):
    if (SyntroPython.isConnected()):
        # do connected state processing
        if (time.time() - lastDirectoryTime) >= DIRECTORY_INTERVAL:
            processDirectory()
            lastDirectoryTime = time.time()
            SyntroPython.requestDirectory()
            
        processSensorData()
            
    if (not processUserInput()):
        break;     
    
    if ((time.time() - lastPlotTime) >= PLOT_INTERVAL):
        lastPlotTime = time.time()
        sensorPlot.plot(servicePortMap.values()) 
 
# Exiting so clean everything up.    

for source in serviceNameMap.keys():
    SyntroPython.removeService(serviceNameMap[source])

SyntroPython.stop()
print("Exiting")
