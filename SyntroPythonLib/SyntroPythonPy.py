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

