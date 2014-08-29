# SyntroPython

Python bindings and example scripts for SyntroNet.

Check out www.richards-tech.com for more details.

### Introduction

SyntroPython makes it really easy to write Python scripts that take advantage of the capabilities of a SyntroNet cloud. SyntroPython appears as an extension library so no knowledge of C++ or Qt programming is required. After a simple install, it's all Python!

SyntroPython has been tested on Linux (Ubuntu 14.04 to be precise). Installation instructions here are relevant to Ubuntu but will be similar for other Linux systems.

### Installation

Prepare the system by installing the pre-requisites:

	sudo apt-get git build-essential qt4-default python-dev

This code has been tested with Qt4.8.6 and Qt5.2.1 so qt5-default can be used instead if desired.

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

Once the libraries have been installed as described above, it's possible to try out the example scripts. There are three - 
SyntroPythonCam (video streaming from a USB webcam), SyntroPythonPicam (video streaming from a Raspberry Pi with its camera) and SyntroPythonView (video stream viewer). These show the multicast capabilities of SyntroNet by generating and receiving a 
multicast video stream. Multiple SyntroPythonViews can receive the same video stream from any SyntroPythonCam/Picam at any time.

#### SyntroPythonCam

SyntroPythonCam requires a UVC webcam. The Logitech C920 is an excellent choice, especially for HD video, but most modern webcams will work if they are compatible with Linux.

To run, do:

	cd ~/SyntroNet/SyntroPython/SyntroPythonCam
	python SyntroPythonCam.py

You should see a window pop up showing the video stream from the camera. It will generate a stream with the same name as the hostname of the machine on which it is running. This can be changed if desired. Running the script will create a file "~/.config/Syntro/SyntroPythonCam.ini". Using an editor, find the entry for "appName" and change this to whatever is required. Then restart the script.

You can customize the frame size and rate by editing SyntroPythonCam.py. If you have an HD camera, changine the width to 1280 and the height to 720 will give nice results.

You can also just run the script in console only mode:

	python SyntroPythonCam.py -c

There is a performance advantage to running in console mode. Where possible, SyntroPython will try to capture jpeg frames from the camera and decompression is only necessary if the frames need to be displayed. Running in console mode avoids this.

#### SyntroPythonPicam

This is a version of SyntroPythonCam that uses the Raspberry Pi's camera rather than a USB webcam. It uses the picamera library so there is one extra pre-requisite:

	sudo apt-get install python-picamera

then:

	python SyntroPythonPicam.py -c

It's recommended to run in console mode (the '-c') as displaying the stream locally can overload the Pi.

SyntroPythonPicam generates a standard SyntroNet MJPEG AVMUX stream. It works very well at 640 x 480 at frame rates up to 30fps. At larger frame sizes something bad happens after a few frames requiring a reboot. This is being investigated.
	
#### SyntroPythonView

There is one additional thing that needs to be done before running SyntroPythonView. Every SyntroNet cloud needs at least one instance of SyntroControl (the SyntroNet message switch) running somewhere in the LAN. The SyntroControl app was installed with SyntroCore. So, just run:

	SyntroControl &

A window will pop up showing the SyntroControl display. This can be on any machine on the LAN and doesn't have to be on ones running SyntroPythonCam or SyntroPythonView.

To run SyntroPythonView, do:

	cd ~/SyntroNet/SyntroPython/SyntroPythonView
	python SyntroPythonView.py


This can be on the same machine as SyntroPythonCam or any other machine on the same LAN. By default, it will get a directory of video sources from SyntroNet and then use the first one found. If there's just a copy of SyntroPythonCam running, that will be the one found. A window will pop up showing the real time video stream from the camera.

You should be able to see the SyntroLinks to the two apps on the SyntroControl display now also.

Try running more than one instance or another copy of SyntroPythonView on another machine. It should just work!

To force SyntroPythonView to always display a particular stream, edit SyntroPythonView.py file and change the "serviceName" to something other than blank. SyntroPythonView will then wait for that stream to become available before opening its window and displaying it.

Note that in this simple implementation, the directory is only requested once when the SyntroLink comes up. This means that SyntroPythonView has to be started after SyntroPythonCam unless serviceName has a specific service name set. This is not fundamental and can easily be improved but at the expense of simplicity - it would be necessary to fetch the directory at regular intervals to pick up any new services that might have appeared.

### The SyntroPython API

To see the functions included in the API do:

	python
	>>> import SyntroPython
	>>> help(SyntroPython)

This will display the functions and their descriptions.

### Creating real SyntroPython apps

The included examples are fairly trivial but still do something useful. For example SyntroNet also provides an end to end datagram service that can be very effective for any app requiring peer to peer communications across the SyntroNet cloud. Multiple SyntroNets can be joined together with inter-SyntroControl tunnels to fully integrate clouds on multiple LANs. The data produced by SyntroPythonCam is fully compatible with standard SyntroNet AVMUX streams so SyntroView (in the SyntroApps repo) can be used to display the video stream. Likewise SyntroPythonView can display video streams from any of the standard SyntroNet camera apps (such as SyntroLCam).

SyntroPython.cpp is the main interface between the Python world and the C++ world. It's in the SyntroPythonLib directory. SyntroPythonPy.py provides some useful Python definitions for the SyntroNet system. Importing SyntroPython and SyntroPythonPy into a script will give full access to SyntroPython functions.


