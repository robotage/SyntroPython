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

# camera parameters - change as required

CAMERA_ENABLE = True

# If CAMERA_USB is true, expects a webcam
# If False, expects a Pi camera
CAMERA_USB = False

CAMERA_INDEX = 0
CAMERA_WIDTH = 640
CAMERA_HEIGHT = 480
CAMERA_RATE = 10

import sys

# import the sensor drivers
sys.path.append('../SensorDrivers')
import RT_ADXL345
import RT_TSL2561
import RT_TMP102
import RT_BMP180
import RT_MCP9808
import RT_HTU21D
import RT_NullSensor

# the sensor update interval (in seconds). Change as required.
SENSOR_UPDATE_INTERVAL = 0.5

# The set of sensors. Choose which ones are required or use NullSensor if no physical sensor
# Multi sensor objects (such as BMP180 for temp and pressure) can be reused

accel = RT_ADXL345.RT_ADXL345()
light = RT_TSL2561.RT_TSL2561()
temperature = RT_HTU21D.RT_HTU21D()
pressure = RT_BMP180.RT_BMP180()
humidity = temperature

# Now import what we need

import SyntroPython
import SyntroPythonPy
import time
import io
import picamera
import json
import SensorJSON

'''
------------------------------------------------------------
    User interface function
'''

def processUserInput():
    ''' Process any user console input that might have arrived. 
        Returns False if need to exit. '''
    
    # see if in daemon mode. if so, just return
    if SyntroPython.checkDaemonMode():
        return True
    c = SyntroPython.getConsoleInput()
    if (c != None):
        print("\n")
        sc = chr(c)
        if (sc == "x"):
            return False
        elif (sc == "s"):
            print("Status:")
            if (SyntroPython.isConnected()):
                print("SyntroLink connected")
            else:
                print("SyntroLink closed")
            if accel.getDataValid():
                data = accel.readAccel()
                print("Accel: x = %.2fg, y = %.2fg, z = %.2fg" % (data[0], data[1], data[2]))
            if light.getDataValid():
                print("Light: %.2f lux" % light.readLight())
            if temperature.getDataValid():
                print("Temperature: %.2fC" % temperature.readTemperature())
            if pressure.getDataValid():
                print("Pressure: %.2fhPa" % pressure.readPressure())
            if humidity.getDataValid():
                print("Humidity: %.2f%%RH" % humidity.readHumidity())
        elif (sc == 'h'):
            print("Available commands are:")
            print("  s - display status")
            print("  x - exit")
            
        print("\nEnter command: "),
        sys.stdout.flush()
    return True
    
'''
------------------------------------------------------------
    Sensor functions
'''

# global to maintain last sensor read time
lastSensorReadTime = time.time() 

# global to maintain the sensor service port
sensorPort = -1

def initSensors():
    ''' Initialize the sensors and establish the sensor multicast source port '''
    global sensorPort
    
    sensorPort = SyntroPython.addMulticastSource(SyntroPythonPy.SYNTRO_STREAMNAME_SENSOR)
    if (sensorPort == -1):
        print("Failed to activate sensor service")
        SyntroPython.stop()
        sys.exit()

    accel.enable()
    light.enable()
    temperature.enable()
    pressure.enable()
    humidity.enable()

def readSensors():
    global lastSensorReadTime
    global sensorPort 
    
    if ((time.time() - lastSensorReadTime) < SENSOR_UPDATE_INTERVAL):
        # call background loops
        accel.background()
        light.background()
        temperature.background()
        pressure.background()
        humidity.background()
        return
        
    # time send send the sensor readings    
    lastSensorReadTime = time.time()
    
    sensorDict = {}
    
    sensorDict[SensorJSON.TIMESTAMP] = time.time()
    
    if accel.getDataValid():
        accelData = accel.readAccel()
        sensorDict[SensorJSON.ACCEL_DATA] = accelData
        
    if light.getDataValid():
        lightData = light.readLight()
        sensorDict[SensorJSON.LIGHT_DATA] = lightData

    if temperature.getDataValid():
        temperatureData = temperature.readTemperature()
        sensorDict[SensorJSON.TEMPERATURE_DATA] = temperatureData

    if pressure.getDataValid():
        pressureData = pressure.readPressure()
        sensorDict[SensorJSON.PRESSURE_DATA] = pressureData
        
    if humidity.getDataValid():
        humidityData = humidity.readHumidity()
        sensorDict[SensorJSON.HUMIDITY_DATA] = humidityData

    if (SyntroPython.isServiceActive(sensorPort) and SyntroPython.isClearToSend(sensorPort)):
        SyntroPython.sendMulticastData(sensorPort, json.dumps(sensorDict))    


'''
------------------------------------------------------------
    USB camera functions and main loop
'''

def USBCameraLoop():
    ''' This is the main loop when the USB camera is enabled. '''
    
    # Open the camera device
    if (not SyntroPython.vidCapOpen(CAMERA_INDEX, CAMERA_WIDTH, CAMERA_HEIGHT, CAMERA_RATE)):
        print("Failed to open USB camera")
        SyntroPython.stop()
        sys.exit()

    # Activate the video stream
    videoPort = SyntroPython.addMulticastSource(SyntroPythonPy.SYNTRO_STREAMNAME_AVMUX)
    if (videoPort == -1):
        print("Failed to activate video service")
        SyntroPython.stop()
        sys.exit()

    while True:
        # try to get a frame from the USB camera
        ret, frame, jpeg, width, height, rate = SyntroPython.vidCapGetFrame(CAMERA_INDEX)
        if (ret):            
            # check if it can be sent on the SyntroLink
            if (SyntroPython.isServiceActive(videoPort) and SyntroPython.isClearToSend(videoPort)):
                # send the frame
                SyntroPython.setVideoParams(width, height, rate)
                pcm = str("")
                if (jpeg):
                    SyntroPython.sendJpegAVData(videoPort, frame, pcm)
                else:
                    SyntroPython.sendAVData(videoPort, frame, pcm)
                    
        # check if anything from the sensors            
        readSensors()
        
        # handle user input
        if (not processUserInput()):
            break;
            
        #give other things a chance
        time.sleep(0.02)
               
    # exiting            
    SyntroPython.removeService(videoPort)

'''
------------------------------------------------------------
    pi camera functions and main loop
'''

# this is used to track when we have a new frame
piCameraLastFrameIndex = -1

def piCameraSendFrameHelper(stream, frame, videoPort):
    ''' do the actual frame processing '''
    global piCameraLastFrameIndex
    
    # record index of new frame
    piCameraLastFrameIndex = frame.index

    # get the frame data and display it
    stream.seek(frame.position)
    image = stream.read(frame.frame_size)

    # now check if it can be sent on the SyntroLink
    if (SyntroPython.isServiceActive(videoPort) and SyntroPython.isClearToSend(videoPort)):
        # send the frame
        pcm = str("")
        SyntroPython.sendJpegAVData(videoPort, image, pcm)
    

def piCameraSendFrame(stream, videoPort):
    ''' sendFrame checks the circular buffer to see if there is a new frame
    and send it on the SyntroLink if that's possible '''

    global piCameraLastFrameIndex

    with stream.lock:
        if (CAMERA_RATE > 10):
            for frame in stream.frames:
                if (frame.index > piCameraLastFrameIndex):
                    piCameraSendFrameHelper(stream, frame, videoPort)
        else:
            # skip to last frame in iteration
            frame = None
            for frame in stream.frames:
                pass
 
            if (frame is None):
                return
         
            # check to see if this is new
            if (frame.index == piCameraLastFrameIndex):
                return
            piCameraSendFrameHelper(stream, frame, videoPort)
        
 
def piCameraLoop():
    ''' This is the main loop when the pi camera is enabled. '''
    # Activate the video stream
    videoPort = SyntroPython.addMulticastSource(SyntroPythonPy.SYNTRO_STREAMNAME_AVMUX)
    if (videoPort == -1):
        print("Failed to activate video service")
        SyntroPython.stop()
        sys.exit()

    # Tell the library what video params we are using
    SyntroPython.setVideoParams(CAMERA_WIDTH, CAMERA_HEIGHT, CAMERA_RATE)

    with picamera.PiCamera(CAMERA_INDEX) as camera:
        camera.resolution = (CAMERA_WIDTH, CAMERA_HEIGHT)
        camera.framerate = (CAMERA_RATE, 1)

        # need enough buffering to overcome any latency
        stream = picamera.PiCameraCircularIO(camera, seconds = 1)

        # start recoding in mjpeg mode
        camera.start_recording(stream, format = 'mjpeg')

        try:
            while(True):
                # process any new frames
                camera.wait_recording(0)
                piCameraSendFrame(stream, videoPort)
                
                # see if anythng new from the sensors
                readSensors()
                
                # handle user input
                if (not processUserInput()):
                    break;
                    
                #give other things a chance
                time.sleep(0.02)
  
        finally:
            camera.stop_recording()
            SyntroPython.removeService(videoPort)

'''
------------------------------------------------------------
    No camera main loop
'''


def noCameraLoop():
    ''' This is the main loop when no camera is running. '''

    while True:
        # see if anything from the sensors
        readSensors()
        
        # handle any user input
        if (not processUserInput()):
            break;
            
        # give other things a chance
        time.sleep(0.02)

'''
------------------------------------------------------------
    Main code
'''


# start SyntroPython running
SyntroPython.start("SensorNetPoP", sys.argv, False)

# this delay is necessary to allow Qt startup to complete
time.sleep(1)

if not SyntroPython.checkDaemonMode():
    # put console into single character mode
    SyntroPython.startConsoleInput()
    print("SensorNetPoP starting...")
    print("Enter command: "),
    sys.stdout.flush()

# set the title if in GUI mode
SyntroPython.setWindowTitle(SyntroPython.getAppName() + " camera stream")

initSensors()

if CAMERA_ENABLE:
    if CAMERA_USB:
        USBCameraLoop()
    else:
        piCameraLoop()
else:
    noCameraLoop()

# Exiting so clean everything up.    

SyntroPython.stop()
if not SyntroPython.checkDaemonMode():
    print("Exiting")
