#
#  Copyright (c) 2014 richards-tech
#
#  This file is part of SyntroNet
#
#  SyntroNet is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  SyntroNet is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with SyntroNet.  If not, see <http://www.gnu.org/licenses/>.
#

TEMPLATE = lib
TARGET = SyntroPythonLib

target.path = /usr/lib
headerfiles.path = /usr/include/syntro/SyntroPythonLib
headerfiles.files += *.h

CONFIG += link_pkgconfig
PKGCONFIG += syntro
INSTALLS += headerfiles target

QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += shared

# No debug info in release builds
unix:QMAKE_CXXFLAGS_RELEASE -= -g
QMAKE_CXXFLAGS += $$(QT_CXXFLAGS)

DEFINES += QT_NETWORK_LIB

INCLUDEPATH += GeneratedFiles \
    GeneratedFiles/release

LIBS += -lSyntroLib

MOC_DIR += GeneratedFiles/release

OBJECTS_DIR += release

UI_DIR += GeneratedFiles

RCC_DIR += GeneratedFiles

include(SyntroPythonLib.pri)
