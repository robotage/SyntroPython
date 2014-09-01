# SyntroPython

Python bindings and example scripts for SyntroNet.

Check out www.richards-tech.com for more details.

### Introduction

SyntroPython makes it really easy to write Python scripts that take advantage of the capabilities of a SyntroNet cloud. SyntroPython appears as an extension library so no knowledge of C++ or Qt programming is required. After a simple install, it's all Python!

SyntroPython has been tested on Linux (Ubuntu 14.04 to be precise), NVIDIA Jetson TK1 and Raspberry Pi running Raspbian. Installation instructions here are relevant to Ubuntu but will be similar for other Linux systems.

### Installation

Prepare the system by installing the pre-requisites:

	sudo apt-get git build-essential qt4-default python-dev

This code has been tested with Qt4.8.2, Qt4.8.6 and Qt5.2.1 so qt5-default can be used instead if desired.

SyntroCore must be cloned and built:

	cd ~/
	mkdir SyntroNet
	cd SyntroNet
	git clone git://github.com/richards-tech/SyntroCore.git
	cd SyntroCore
	qmake
	make -j4
	sudo make install

Note that "-j4" is suitable for quad core machines. This can be adjusted up or down as required. For example, on an Intel i7 use -j8.

Now SyntroPython can be cloned and built:

	cd ~/
	cd SyntroNet
	git clone git://github.com/richards-tech/SyntroPython.git
	cd SyntroPython/SyntroPythonLib
	qmake
	make -j4
	sudo make install
	python setup.py build
	sudo python setup.py install

The same comments apply to the "-j4" here. 

### The sample scripts

Once the libraries have been installed as described above, it's possible to try out the example scripts. There are five:

* SPCam - video streaming from a USB webcam
* SPPicam - video streaming from a Raspberry Pi using the pi camera
* SPView - video stream viewer for streams proced by SPCams and SPPicams
* SensorNetPoP - a Raspberry Pi based SensorNet Point of Presence (video, environmental)
* SensorNetView - a viewer for sensor data collected by SensorNetPoPs

#### SPCam

SPCam requires a UVC webcam. The Logitech C920 is an excellent choice, especially for HD video, but most modern webcams will work if they are compatible with Linux.

To run, do:

	cd ~/SyntroNet/SyntroPython/SPCam
	python SPCam.py

You should see a window pop up showing the video stream from the camera. It will generate a stream with the same name as the hostname of the machine on which it is running. This can be changed if desired. Running the script will create a file "~/.config/Syntro/SPCam.ini". Using an editor, find the entry for "appName" and change this to whatever is required. Then restart the script.

You can customize the frame size and rate by editing SPCam.py. If you have an HD camera, changine the width to 1280 and the height to 720 will give nice results.

You can also just run the script in console only mode:

	python SPCam.py -c

There is a performance advantage to running in console mode. Where possible, SyntroPython will try to capture jpeg frames from the camera and decompression is only necessary if the frames need to be displayed. Running in console mode avoids this.

#### SPPicam

This is a version of SPCam that uses the Raspberry Pi's camera rather than a USB webcam. It uses the picamera library so there is one extra pre-requisite (latest Raspbian versions already include picamera however):

	sudo apt-get install python-picamera

then:

	python SPPicam.py -c

It's recommended to run in console mode (the '-c') as displaying the stream locally can overload the Pi.

SPPicam generates a standard SyntroNet MJPEG AVMUX stream. It works very well at 640 x 480 at frame rates up to 30fps. At larger frame sizes something bad happens after a few frames requiring a reboot. This is a known problem with picamera (issue #47 in the GitHub repo).
	
#### SPView

There is one additional thing that needs to be done before running SPView. Every SyntroNet cloud needs at least one instance of SyntroControl (the SyntroNet message switch) running somewhere in the LAN. The SyntroControl app was installed with SyntroCore. So, just run:

	SyntroControl &

A window will pop up showing the SyntroControl display. This can be on any machine on the LAN and doesn't have to be on ones running SPCam or SPView.

To run SPView, do:

	cd ~/SyntroNet/SyntroPython/SPView
	python SPView.py


This can be on the same machine as SPCam or any other machine on the same LAN. By default, it will get a directory of video sources from SyntroNet and then use the first one found. If there's just a copy of SyntroPythonCam running, that will be the one found. A window will pop up showing the real time video stream from the camera.

You should be able to see the SyntroLinks to the two apps on the SyntroControl display now also.

Try running more than one instance or another copy of SPView on another machine. It should just work!

To force SPView to always display a particular stream, edit SPView.py file and change the "serviceName" to something other than blank. SPView will then wait for that stream to become available before opening its window and displaying it.

Note that in this simple implementation, the directory is only requested once when the SyntroLink comes up. This means that SPView has to be started after SPCam unless serviceName has a specific service name set. This is not fundamental and can easily be improved but at the expense of simplicity - it would be necessary to fetch the directory at regular intervals to pick up any new services that might have appeared.

#### SensorNetPoP

SensorNetPoP allows a Raspberry Pi fitted with sensor hardware to act as a sensor node in a sensor network. Supported hardware sensors include:

* USB video camera
* pi camera
* ADXL345 accelerometer
* TSL2561 light sensor
* TMP102 temperature sensor

More sensors will be added in the future. SensorNetPoP.py has many variables that can be used to configure the hardware setup. Choose between USB camera, pi camera or no camera. Enable or disable specific sensors and configure their hardware addresses.

To run, there is one more pre-requisite:

    sudo apt-get install python-smbus
    
By default, the Raspberry Pi I2C is not enabled. To enable it, add the following lines to /etc/modules:

    i2c-bcm2708
    i2c-dev
    
Then comment out the following line in /etc/modprobe.d/raspi-blacklist.conf:

    # blacklist i2c-bcm2708
    
It's worth setting the I2C speed to 400kHz at this time so create a file /etc/modprobe.d/i2c.conf and add the line:

    options i2c_bcm2708 baudrate=400000
    
To avoid having to use sudo every time to run scripts using I2C, create a file /etc/udev/rules.d/90-i2c.rules and add the line:

    KERNEL=="i2c-[0-7]",MODE="0666"
    
A reboot is required to get all this activated.
    
Then run SensorNetPoP in console mode:

	cd ~/SyntroNet/SyntroPython/SensorNetPoP
	python SensorNetPoP.py -c


#### SensorNetView

SensorNetView is a viewer for the sensor data (not video - use SPView or SyntroView for that) from SensorNetPoPs. There is one extra pre-requisite:

    sudo apt-get install python-matplotlib
    
The version of matplotlib in Raspbian is too old to run SensorNetView.py so something like an Ubuntu desktop would be the appropriate platform.

Then:

	cd ~/SyntroNet/SyntroPython/SensorNetView
	python SensorNetView.py

No configuration is required for SensorNetView. Note that SensorNet uses SyntroNet so at least one SyntroControl must be running somewhere on the LAN. SensorNetView will automatically discover any active SensorNetPoPs and start displaying data. It will show the last 10 minutes of data with the standard configuration. If new SensorNetPoPs appear while SensorNetView is running, they will be discovered automatically too. If any SensorNetPoPs disappear, then their displays will be deleted.

It would not be hard to extend SensorNetView to save sensor data to a file. The sensor data is transmitted on SyntroNet as a JSON record so processing of sensor dfata offline is very straightforward.

### The SyntroPython API

To see the functions included in the API do:

	python
	>>> import SyntroPython
	>>> help(SyntroPython)

This will display the functions and their descriptions.

### Creating real SyntroPython apps

The included examples are fairly trivial but still do something useful. For example SyntroNet also provides an end to end datagram service that can be very effective for any app requiring peer to peer communications across the SyntroNet cloud. Multiple SyntroNets can be joined together with inter-SyntroControl tunnels to fully integrate clouds on multiple LANs. The data produced by SPCam/SPPicam is fully compatible with standard SyntroNet AVMUX streams so SyntroView (in the SyntroApps repo) can be used to display the video stream. Likewise SPView can display video streams from any of the standard SyntroNet camera apps (such as SyntroLCam).

SyntroPython.cpp is the main interface between the Python world and the C++ world. It's in the SyntroPythonLib directory. SyntroPythonPy.py provides some useful Python definitions for the SyntroNet system. Importing SyntroPython and SyntroPythonPy into a script will give full access to SyntroPython functions.


