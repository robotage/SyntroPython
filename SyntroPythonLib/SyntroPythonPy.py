'''

  This file is part of SyntroPython

  Copyright (c) 2014, richards-tech

  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to use,
  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
  Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

'''

# This file contains useful SyntroNet definitions

# Reserved characters and their functions

SYNTRO_SERVICEPATH_SEP = '/'                                # the path component separator
SYNTRO_STREAM_TYPE_SEP = ':'                                # used to delimit a stream type in path
SYNTROCFS_FILENAME_SEP = ';'                                # used to seperate file paths in a CFS dir string
SYNTRO_LOG_COMPONENT_SEP = '|'                              # separates parts of log messages

#   Standard multicast stream names

SYNTRO_STREAMNAME_AVMUX = "avmux"
SYNTRO_STREAMNAME_AVMUXLR = "avmux:lr"
SYNTRO_STREAMNAME_AVMUXMOBILE = "avmuxmobile"
SYNTRO_STREAMNAME_VIDEO = "video"
SYNTRO_STREAMNAME_VIDEOLR = "video:lr"
SYNTRO_STREAMNAME_AUDIO = "audio"
SYNTRO_STREAMNAME_NAV = "nav"
SYNTRO_STREAMNAME_LOG = "log"
SYNTRO_STREAMNAME_SENSOR = "sensor"
SYNTRO_STREAMNAME_TEMPERATURE = "temperature"
SYNTRO_STREAMNAME_HUMIDITY = "humidity"
SYNTRO_STREAMNAME_LIGHT = "light"
SYNTRO_STREAMNAME_MOTION = "motion"
SYNTRO_STREAMNAME_AIRQUALITY = "airquality"
SYNTRO_STREAMNAME_PRESSURE = "pressure"
SYNTRO_STREAMNAME_ACCELEROMETER = "accelerometer"
SYNTRO_STREAMNAME_ZIGBEE_MULTICAST = "zbmc"
SYNTRO_STREAMNAME_HOMEAUTOMATION = "ha"
SYNTRO_STREAMNAME_THUMBNAIL = "thumbnail"
SYNTRO_STREAMNAME_IMAGE = "image"

#   Standard E2E stream names

SYNTRO_STREAMNAME_ZIGBEE_E2E = "zbe2e"
SYNTRO_STREAMNAME_CONTROL = "control"
SYNTRO_STREAMNAME_FILETRANSFER = "filetransfer"
SYNTRO_STREAMNAME_PTZ = "ptz"
SYNTRO_STREAMNAME_CFS = "cfs"

