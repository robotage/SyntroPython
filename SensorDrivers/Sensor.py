#!/usr/bin/python

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

import json
import SensorJSON

# global defines

SENSOR_RECORD_TIME_INTERVAL = 1.0                           # everything is unified to this interval (in seconds)
SENSOR_RECORD_LENGTH = 600                                  # number of time intervals (default 10 mins of data)

class Sensor():
    ''' Sensor model used by viewers '''
        
    def __init__(self, serviceName, servicePort):       
        # these vars are used to accumulate data in the current time interval
        self.currentAccelDataX = 0
        self.currentAccelDataY = 0
        self.currentAccelDataZ = 0
        self.currentLightData = 0
        self.currentTemperatureData = 0
    
        self.currentDataCount = 0                           # used for the averaging
    
        self.currentTime = 0
    
        # these vars are used to store averaged records per time interval
        self.accelDataX = []
        self.accelDataY = []
        self.accelDataZ = []
        self.lightData = []
        self.temperatureData = []
    
        # these vars record which sensor data has been received
        self.accelValid = False
        self.lightValid = False
        self.temperatureValid = False
    
        # these cars used to identify the SyntroNet connection
        self.serviceName = serviceName
        self.servicePort = servicePort
        
        # preset the stores
        for i in range (0, SENSOR_RECORD_LENGTH):
            self.accelDataX.append(0.0)
            self.accelDataY.append(0.0)
            self.accelDataZ.append(0.0)
            self.lightData.append(0.0)
            self.temperatureData.append(0.0)    
                          
    def newJSONData(self, data):
        ''' adds a sensor JSON record to the record '''
        # decode the JSON record
        sensorDict = json.loads(data)
        newTimestamp = sensorDict.get(SensorJSON.TIMESTAMP)
        newAccelData = sensorDict.get(SensorJSON.ACCEL_DATA)
        newLightData = sensorDict.get(SensorJSON.LIGHT_DATA)
        newTemperatureData = sensorDict.get(SensorJSON.TEMPERATURE_DATA)
        
        if (newTimestamp == None):
            print ("Received JSON record without timestamp")
            return
            
        # now decide how to deal with things based on the timestamp
        
        if (self.currentTime == 0):
            # special case for first record
            self.currentTime = newTimestamp
            if (newAccelData != None):
                self.accelValid = True
        
            if (newLightData != None):
                self.lightValid = True
        
            if (newTemperatureData != None):
                self.temperatureValid = True
        
            if (self.accelValid):
                self.currentAccelDataX = newAccelData[0]
                self.currentAccelDataY = newAccelData[1]
                self.currentAccelDataZ = newAccelData[2]
            if (self.lightValid):
                self.currentLightData = newLightData
            if (self.temperatureValid):
                self.currentTemperatureData = newTemperatureData
            self.currentDataCount = 1
            return
            
        if ((newTimestamp - self.currentTime) >= SENSOR_RECORD_TIME_INTERVAL):
            # this record is for a new time interval
            timeUnits = int((newTimestamp - self.currentTime) /
                        SENSOR_RECORD_TIME_INTERVAL)        # this is how many slots to fill based on gap
            self.currentTime += timeUnits                   # advance the clock
            
            for i in range(0, timeUnits):
                if (self.accelValid):
                    self.accelDataX.pop(0)
                    self.accelDataX.append(self.currentAccelDataX / self.currentDataCount)
                    self.accelDataY.pop(0)
                    self.accelDataY.append(self.currentAccelDataY / self.currentDataCount)
                    self.accelDataZ.pop(0)
                    self.accelDataZ.append(self.currentAccelDataZ / self.currentDataCount)
                if (self.lightValid):
                    self.lightData.pop(0)
                    self.lightData.append(self.currentLightData / self.currentDataCount)
                if (self.temperatureValid):
                    self.temperatureData.pop(0)
                    self.temperatureData.append(self.currentTemperatureData / self.currentDataCount)
                
            # zero out accumulators for next set
            self.currentAccelDataX = 0
            self.currentAccelDataY = 0
            self.currentAccelDataZ = 0
            self.currentLightData = 0
            self.currentTemperatureData = 0
            self.currentDataCount = 0
        
        # add in new data   
        if (self.accelValid):     
            self.currentAccelDataX += newAccelData[0] 
            self.currentAccelDataY += newAccelData[1]
            self.currentAccelDataZ += newAccelData[2] 
        if (self.lightValid):
            self.currentLightData += newLightData
        if (self.temperatureValid):
            self.currentTemperatureData += newTemperatureData
        self.currentDataCount += 1
        
    # accumulated data access functions
        
    def getAccelDataX(self):
        return self.accelDataX
         
    def getAccelDataY(self):
        return self.accelDataY
         
    def getAccelDataZ(self):
        return self.accelDataZ
      
    def getLightData(self):
        return self.lightData
 
    def getTemperatureData(self):
        return self.temperatureData
         
    def getServiceName(self):
        return self.serviceName
        
    def getServicePort(self):
        return self.servicePort
        
    # current data access functions
    
    def getCurrentAccelDataX(self):
        if (self.currentDataCount == 0):
            return 0
        else:
            return self.currentAccelDataX / self.currentDataCount
            
    def getCurrentAccelDataY(self):
        if (self.currentDataCount == 0):
            return 0
        else:
            return self.currentAccelDataY / self.currentDataCount
            
    def getCurrentAccelDataZ(self):
        if (self.currentDataCount == 0):
            return 0
        else:
            return self.currentAccelDataZ / self.currentDataCount
            
    def getCurrentLightData(self):
        if (self.currentDataCount == 0):
            return 0
        else:
            return self.currentLightData / self.currentDataCount
            
    def getCurrentTemperatureData(self):
        if (self.currentDataCount == 0):
            return 0
        else:
            return self.currentTemperatureData / self.currentDataCount
            
    # data valid functions
    
    def getAccelValid(self):
        return self.accelValid
             
    def getLightValid(self):
        return self.lightValid
             
    def getTemperatureValid(self):
        return self.temperatureValid
             
        
        
        
        
