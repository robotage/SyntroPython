//
//  Copyright (c) 2014 richards-tech
//
//  This file is part of SyntroNet
//
//  SyntroNet is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  SyntroNet is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with SyntroNet.  If not, see <http://www.gnu.org/licenses/>.
//

#include <Python.h>
#include "SyntroPythonGlue.h"
#include <pthread.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>

static pthread_t syPyThread;
static SyntroPythonGlue syPyGlue;
static int argc;
static char **argv;

void *runThread(void *appName)
{
    syPyGlue.startLib((const char *)appName, argc, argv);
    pthread_exit(0);
}

static PyObject *start(PyObject *self, PyObject *args)
{
    char *appName;
    PyObject *cmdArgs;

    if (!PyArg_ParseTuple(args, "sO!", &appName, &PyList_Type, &cmdArgs)) {
        printf("Bad argument to start\n");
        Py_RETURN_NONE;
    }

    argc = PyList_Size(cmdArgs);
    argv = (char **)malloc(argc * sizeof(char *));

    for (int i = 0; i < argc; i++) {
        char *arg = PyString_AsString(PyList_GetItem(cmdArgs, i));
        argv[i] = (char *)malloc(strlen(arg) + 1);
        strcpy(argv[i], arg);
    }

    pthread_create(&syPyThread, NULL, runThread, appName);

    Py_RETURN_NONE;
}

static PyObject *stop(PyObject *self, PyObject *args)
{
    void *threadRes;
    syPyGlue.stopLib();
    pthread_join(syPyThread, &threadRes);
    Py_RETURN_NONE;
}

static PyObject *setWindowTitle(PyObject *self, PyObject *args)
{
    char *title;

    if (!PyArg_ParseTuple(args, "s", &title)) {
        printf("Bad argument to setWindowTitle\n");
        Py_RETURN_NONE;
    }

    syPyGlue.setWindowTitle(title);
    Py_RETURN_NONE;
}

static PyObject *displayImage(PyObject *self, PyObject *args)
{
    unsigned char *image;
    int length;
    int width;
    int height;
    char *timestamp;

    if (!PyArg_ParseTuple(args, "s#iis", &image, &length, &width, &height, &timestamp)) {
        printf("Bad argument to displayImage\n");
        Py_RETURN_NONE;
    }

    syPyGlue.displayImage(image, length, width, height, timestamp);
    Py_RETURN_NONE;
}


static PyObject *startConsoleInput(PyObject *self, PyObject *args)
{
    struct termios	ctty;

    tcgetattr(fileno(stdout), &ctty);
    ctty.c_lflag &= ~(ICANON);
    tcsetattr(fileno(stdout), TCSANOW, &ctty);
    Py_RETURN_NONE;
}

static PyObject *getConsoleInput(PyObject *self, PyObject *args)
{
    int i;

    ioctl(0, FIONREAD, &i);
    if (i <= 0)
        Py_RETURN_NONE;

    return Py_BuildValue("b", getchar());
}


static PyObject *getAppName(PyObject *self, PyObject *args)
{
    return Py_BuildValue("s", syPyGlue.getAppName());
}

static PyObject *isConnected(PyObject *self, PyObject *args)
{
    if (syPyGlue.isConnected())
        return Py_BuildValue("i", 1);
    else
        return Py_BuildValue("i", 0);
}

static PyObject *requestDirectory(PyObject *self, PyObject *args)
{
    if (syPyGlue.requestDirectory())
        return Py_BuildValue("i", 1);
    else
        return Py_BuildValue("i", 0);
}

static PyObject *lookupMulticastSources(PyObject *self, PyObject *args)
{
    char *serviceName;

    if (!PyArg_ParseTuple(args, "s", &serviceName)) {
        printf("Bad argument to lookupMulticastSources\n");
        return Py_BuildValue("s", "");
    }

    return Py_BuildValue("s", syPyGlue.lookupMulticastSources(serviceName));
}

static PyObject *addMulticastSource(PyObject *self, PyObject *args)
{
    char *servicePath;

    if (!PyArg_ParseTuple(args, "s", &servicePath)) {
        printf("Bad argument to addMulticastSource\n");
        return Py_BuildValue("i", -1);
    }
    return Py_BuildValue("i", syPyGlue.addMulticastSourceService(servicePath));
}

static PyObject *addMulticastSink(PyObject *self, PyObject *args)
{
    char *servicePath;

    if (!PyArg_ParseTuple(args, "s", &servicePath)) {
        printf("Bad argument to addMulticastSink\n");
        return Py_BuildValue("i", -1);
    }
    return Py_BuildValue("i", syPyGlue.addMulticastSinkService(servicePath));
}

static PyObject *lookupE2ESources(PyObject *self, PyObject *args)
{
    char *serviceName;

    if (!PyArg_ParseTuple(args, "s", &serviceName)) {
        printf("Bad argument to lookupE2ESources\n");
        return Py_BuildValue("s", "");
    }

    return Py_BuildValue("s", syPyGlue.lookupE2ESources(serviceName));
}

static PyObject *addE2ESource(PyObject *self, PyObject *args)
{
    char *servicePath;

    if (!PyArg_ParseTuple(args, "s", &servicePath)) {
        printf("Bad argument to addE2ESource\n");
        return Py_BuildValue("i", -1);
    }
    return Py_BuildValue("i", syPyGlue.addE2ESourceService(servicePath));
}

static PyObject *addE2ESink(PyObject *self, PyObject *args)
{
    char *servicePath;

    if (!PyArg_ParseTuple(args, "s", &servicePath)) {
        printf("Bad argument to addE2ESink\n");
        return Py_BuildValue("i", -1);
    }
    return Py_BuildValue("i", syPyGlue.addE2ESinkService(servicePath));
}


static PyObject *removeService(PyObject *self, PyObject *args)
{
    int servicePort;

    if (!PyArg_ParseTuple(args, "i", &servicePort)) {
        printf("Bad argument to removeService\n");
        return Py_BuildValue("i", 0);
    }
    return Py_BuildValue("i", syPyGlue.removeService(servicePort));
}

static PyObject *isClearToSend(PyObject *self, PyObject *args)
{
    int servicePort;

    if (!PyArg_ParseTuple(args, "i", &servicePort)) {
        printf("Bad argument to isClearToSend\n");
        return Py_BuildValue("i", 0);
    }

    if (syPyGlue.isClearToSend(servicePort))
        return Py_BuildValue("i", 1);
    else
        return Py_BuildValue("i", 0);
}

static PyObject *isServiceActive(PyObject *self, PyObject *args)
{
    int servicePort;

    if (!PyArg_ParseTuple(args, "i", &servicePort)) {
        printf("Bad argument to isServiceActive\n");
        return Py_BuildValue("i", 0);
    }

    if (syPyGlue.isServiceActive(servicePort))
        return Py_BuildValue("i", 1);
    else
        return Py_BuildValue("i", 0);
}

static PyObject *sendAVData(PyObject *self, PyObject *args)
{
    int servicePort;
    unsigned char *videoData;
    int videoLength;
    unsigned char *audioData;
    int audioLength;

    if (!PyArg_ParseTuple(args, "is#s#", &servicePort, &videoData, &videoLength, &audioData, &audioLength)) {
        printf("Bad argument to sendAVData\n");
        return Py_BuildValue("i", 0);
    }

    if (syPyGlue.sendAVData(servicePort, videoData, videoLength, audioData, audioLength))
        return Py_BuildValue("i", 1);
    else
        return Py_BuildValue("i", 0);
}

static PyObject *getAVData(PyObject *self, PyObject *args)
{
    int servicePort;
    unsigned char *videoData = NULL;
    int videoLength = 0;
    unsigned char *audioData = NULL;
    int audioLength = 0;
    long long timestamp = 0;

    if (!PyArg_ParseTuple(args, "i", &servicePort)) {
        printf("Bad argument to getAVData\n");
        return Py_BuildValue("iLs#s#", false, timestamp, videoData, videoLength, audioData, audioLength);
    }
    bool ret = syPyGlue.getAVData(servicePort, &timestamp, &videoData, &videoLength, &audioData, &audioLength);
    PyObject* retData = Py_BuildValue("iLs#s#", ret, timestamp, videoData, videoLength, audioData, audioLength);
    if (videoData != NULL)
        free(videoData);
    if (audioData != NULL)
        free(audioData);
    return retData;
}


static PyObject *setVideoParams(PyObject *self, PyObject *args)
{
    int width, height, rate;

    if (!PyArg_ParseTuple(args, "iii", &width, &height, &rate)) {
        printf("Bad argument to setVideoParams\n");
        Py_RETURN_NONE;
    }

    syPyGlue.setVideoParams(width, height, rate);
    Py_RETURN_NONE;
}

static PyObject *setAudioParams(PyObject *self, PyObject *args)
{
    int rate, size, channels;

    if (!PyArg_ParseTuple(args, "iii", &rate, &size, &channels)) {
        printf("Bad argument to setAudioParams\n");
        Py_RETURN_NONE;
    }

    syPyGlue.setAudioParams(rate, size, channels);
    Py_RETURN_NONE;
}

static PyObject *sendMulticastData(PyObject *self, PyObject *args)
{
    int servicePort;
    unsigned char *data;
    int dataLength;

    if (!PyArg_ParseTuple(args, "is#", &servicePort, &data, &dataLength)) {
        printf("Bad argument to sendMulticastData\n");
        return Py_BuildValue("i", 0);
    }

    if (syPyGlue.sendMulticastData(servicePort, data, dataLength))
        return Py_BuildValue("i", 1);
    else
        return Py_BuildValue("i", 0);
}

static PyObject *sendE2EData(PyObject *self, PyObject *args)
{
    int servicePort;
    unsigned char *data;
    int dataLength;

    if (!PyArg_ParseTuple(args, "is#", &servicePort, &data, &dataLength)) {
        printf("Bad argument to sendE2EData\n");
        return Py_BuildValue("i", 0);
    }

    if (syPyGlue.sendE2EData(servicePort, data, dataLength))
        return Py_BuildValue("i", 1);
    else
        return Py_BuildValue("i", 0);
}

static PyObject *getMulticastData(PyObject *self, PyObject *args)
{
    int servicePort;
    unsigned char *data = NULL;
    int dataLength = 0;

    if (!PyArg_ParseTuple(args, "i", &servicePort)) {
        printf("Bad argument to getMulticastData\n");
        return Py_BuildValue("is#", false, data, dataLength);
    }
    bool ret = syPyGlue.getMulticastData(servicePort, &data, &dataLength);
    PyObject* retData = Py_BuildValue("is#", ret, data, dataLength);
    if (data != NULL)
        free(data);
    return retData;
}

static PyObject *getE2EData(PyObject *self, PyObject *args)
{
    int servicePort;
    unsigned char *data = NULL;
    int dataLength = 0;

    if (!PyArg_ParseTuple(args, "i", &servicePort)) {
        printf("Bad argument to getE2EData\n");
        return Py_BuildValue("is#", false, data, dataLength);
    }
    bool ret = syPyGlue.getE2EData(servicePort, &data, &dataLength);
    PyObject* retData = Py_BuildValue("is#", ret, data, dataLength);
    if (data != NULL)
        free(data);
    return retData;
}


static PyMethodDef SyntroPythonMethods[] = {
    {"start", (PyCFunction)start, METH_VARARGS,
    "Starts the SyntroPython library.\n"
    "Call this before calling any other SyntroPython functions.\n"
    "The function takes two parameters:\n"
    "  appType - a string defining the type of the app\n"
    "  args - the command line args as string (sys.argv)\n"
    "The function returns None"},

    {"stop", (PyCFunction)stop, METH_NOARGS,
    "Stops the SyntroPython library.\n"
    "This should be called just before exiting.\n"
    "There are no parameters and the function returns None."},

    {"setWindowTitle", (PyCFunction)setWindowTitle, METH_VARARGS,
    "Sets the window title in GUI mode.\n"
    "The parameter is a string containign the new window title.\n"
    "The function returns None"},

    {"displayImage", (PyCFunction)displayImage, METH_VARARGS,
    "Displays an umcompressed image in the GUI window.\n"
    "There are four parameters:\n"
    "  image - the image as a string\n"
    "  width - the width of the image\n"
    "  height - the height of the image\n"
    "  timestamp - a string containing the timestamp (can be empty)\n"
    "The function returns None"},

    {"getAppName", (PyCFunction)getAppName, METH_NOARGS,
    "Returns the app name. This is used as the streamName of any stream generated by this app.\n"
    "There are no parameters. The function returns a string containing the app name."},

    {"isConnected", (PyCFunction)isConnected, METH_NOARGS,
    "This function checks if the SyntroLink is in the connected state.\n"
    "The function takes no parameters and returns 1 if connected, 0 otherwise"},

    {"requestDirectory", (PyCFunction)requestDirectory, METH_NOARGS,
    "Requests that an updated SyntroNet directory be sent to this app.\n"
    "The directory will be requested at the next opportunity and will be received\n"
    "some time after that - the function itself is non-blocking.\n"
     "The function takes no parameters and returns true if the request was queued."},

    {"lookupMulticastSources", (PyCFunction)lookupMulticastSources, METH_VARARGS,
    "This function takes a serviceType as a parameter and returns a list\n"
    "of serviceNames that have published that serviceType as a multicast\n"
    "service. SyntroPythonPy.py contains a list of standard serviceTypes\n"
     "although custom ones can be used. An example is SYNTRO_STREAMNAME_AVMUX\n"
     "(yes, confusingly this is called a name).\n"
     "The function returns a string containing end of line separated\n"
     "serviceNames. The list can be empty."},

    {"lookupE2ESources", (PyCFunction)lookupE2ESources, METH_VARARGS,
    "Same as lookupMulticastSources except that E2E sources are returned."},

    {"addMulticastSource", (PyCFunction)addMulticastSource, METH_VARARGS,
    "Activates a multicast source. In other words, it publishes a\n"
    "multicast source. The parameter to the function is a string containing\n"
    "a service name.\n"
    "The function returns the allocated service port (0 or greater) or else\n"
     "-1 if en error occurred."},

    {"addMulticastSink", (PyCFunction)addMulticastSink, METH_VARARGS,
    "Activates a multicast sink. In other words, it subscribes to a\n"
    "multicast source. The parameter to the function is a string containing\n"
    "a service path. A service path is a concatentation of a serviceName,\n"
    "a '/' character and a serviceType. For example:\n"
    "  appname/avmux would describe an AVMUX stream from an app with name appname.\n"
    "The function returns the allocated service port (0 or greater) or else\n"
     "-1 if en error occurred."},


    {"addE2ESource", (PyCFunction)addE2ESource, METH_VARARGS,
    "Activates an E2E source. In other words, it publishes an\n"
    "end to end source. The parameter to the function is a string containing\n"
    "a service name.\n"
    "The function returns the allocated service port (0 or greater) or else\n"
    "-1 if en error occurred."},

    {"addE2ESink", (PyCFunction)addE2ESink, METH_VARARGS,
    "Activates an E2E sink. In other words, it subscribes to an\n"
    "end to end source. The parameter to the function is a string containing\n"
    "a service path. A service path is a concatentation of a serviceName,\n"
    "a '/' character and a serviceType. For example:\n"
    "'appname/control' would describe a CONTROL stream from an app with name appname.\n"
    "The function returns the allocated service port (0 or greater) or else\n"
    "-1 if en error occurred."},

    {"removeService", (PyCFunction)removeService, METH_VARARGS,
    "Removes an active service (source or sink, multicast or E2E)\n"
    "and returns True if ok, False if error. The parameter to the function\n"
    "is the service port return by the original activation."},

    {"isClearToSend", (PyCFunction)isClearToSend, METH_VARARGS,
    "This function is only valid for a multicast source. It is a rate\n"
    "limiting mechanism and it is only possible to send multicast data\n"
    "if the acknowledgement window is open.\n"
    "The function takes the service port as its parameter and returns\n"
    "True if the window is open, False otherwise."},

    {"isServiceActive", (PyCFunction)isServiceActive, METH_VARARGS,
    "This function's meaning depends on the type of service:\n"
    "  multicast source: return sTrue if there is at least one subscriber\n"
    "  multicast sink: returns true if the service path lookup has succeeded\n"
    "  E2E source: not valid (do not use this function)\n"
    "  E2E sink: returns true if the service path lookup has succeeded\n"
    "The function takes the service port as its parameter."},

    {"setVideoParams", (PyCFunction)setVideoParams, METH_VARARGS,
    "This function should be called by an AVMUX source before sending\n"
    "any data. It takes three parameters:\n"
    "  width - the width of the frames\n"
    "  height - the height of the frames\n"
    "  rate - the frame rate\n"
    "There is no returned value."},

    {"setAudioParams", (PyCFunction)setAudioParams, METH_VARARGS,
    "This function should be called by an AVMUX source before sending\n"
    "any data. It takes three parameters:\n"
    "  rate - the sample rate of the audio data\n"
    "  size - the number of bits per sample\n"
    "  channels - the number of channels\n"
    "There is no returned value."},

    {"sendAVData", (PyCFunction)sendAVData, METH_VARARGS,
    "This function sends sends audio and video data in the AVMUX format.\n."
    "The service must be a multicast source that is active and clear to send.\n"
    "The function takes three parameters:\n"
    "  servicePort - the service port on which to send the data\n"
    "  videoData - a str containing a jpeg compressed frame\n"
    "  audioData - a str containing uncompressed PCM format audio\n"
    "Returns True if the call succeeded, False otherwise."},

    {"getAVData", (PyCFunction)getAVData, METH_VARARGS,
    "Gets audio and video data from an AVMUX format multicast source.\n"
    "The function takes the service port as its parameter.\n"
    "It returns four values:\n"
    "  valid - True or False depending on whether the call succeeded\n"
    "  timestamp - the timestamp from the message (mS since epoch)\n"
    "  videoData - any compressed jpeg image received\n"
    "  audioData - any uncompressed PCM audio data received\n"
    "Note that either videoData or audioData could be None."},

    {"sendMulticastData", (PyCFunction)sendMulticastData, METH_VARARGS,
    "This function sends sends generic data.\n."
    "The service must be a multicast source that is active and clear to send.\n"
    "The function takes two parameters:\n"
    "  servicePort - the service port on which to send the data\n"
    "  data - a str containing the data to be sent\n"
    "Returns True if the call succeeded, False otherwise."},

    {"sendE2EData", (PyCFunction)sendE2EData, METH_VARARGS,
    "This function sends sends generic data.\n."
    "The service must be an E2E source or sink that is active.\n"
    "The function takes two parameters:\n"
    "  servicePort - the service port on which to send the data\n"
    "  data - a str containing the data to be sent\n"
    "Returns True if the call succeeded, False otherwise."},

    {"getMulticastData", (PyCFunction)getMulticastData, METH_VARARGS,
    "Gets generic data from a multicast source.\n"
    "The function takes the service port as its parameter.\n"
    "It returns two values:\n"
    "  valid - True or False depending on whether the call succeeded\n"
    "  data - the data contained in the message\n"},

    {"getE2EData", (PyCFunction)getE2EData, METH_VARARGS,
    "Gets generic data from an E2E source.\n"
    "The function takes the service port as its parameter.\n"
    "It returns two values:\n"
    "  valid - True or False depending on whether the call succeeded\n"
    "  data - the data contained in the message\n"},

    {"startConsoleInput", (PyCFunction)startConsoleInput, METH_NOARGS,
    "Configures stdin for single char input."},

    {"getConsoleInput", (PyCFunction)getConsoleInput, METH_NOARGS,
    "Gets a single char from input or None if nothing"},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initSyntroPython()
{
    Py_InitModule3("SyntroPython", SyntroPythonMethods, "SyntroPython library");
}
