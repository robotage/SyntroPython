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

#include "SyntroPythonGlue.h"
#include "SyntroPythonMainConsole.h"
#include "SyntroPythonMainWindow.h"
#include "SyntroPythonClient.h"

#include "SyntroUtils.h"

#include <qmutex.h>

QMutex lockSyntroPython;

SyntroPythonGlue::SyntroPythonGlue()
{
    m_main = NULL;
    m_connected = false;
    m_directoryRequested = false;
}

SyntroPythonGlue::~SyntroPythonGlue()
{
    if (m_main != NULL)
        stopLib();
}

void SyntroPythonGlue::startLib(const char *productType, int& argc, char **argv)
{
    m_argc = argc;
    m_argv = argv;
    if (SyntroUtils::checkConsoleModeFlag(m_argc, m_argv)) {
        QCoreApplication a(m_argc, m_argv);
        SyntroUtils::loadStandardSettings(productType, a.arguments());
        m_main = new SyntroPythonMainConsole(&a, this);
        a.exec();
    } else {
        QApplication a(m_argc, m_argv);
        SyntroUtils::loadStandardSettings(productType, a.arguments());
        m_main = new SyntroPythonMainWindow(this);
        a.exec();
    }
 }

void SyntroPythonGlue::stopLib()
{
    m_main->stopRunning();
    m_main = NULL;
}

void SyntroPythonGlue::setWindowTitle(char *title)
{
    m_main->setWindowTitle(title);
}

void SyntroPythonGlue::displayImage(unsigned char *image, int length,
                                    int width, int height, char *timestamp)
{
    m_main->displayImage(QByteArray((const char *)image, length), width, height, timestamp);
}

char *SyntroPythonGlue::getAppName()
{
    static char name[256];

    strcpy(name, qPrintable(SyntroUtils::getAppName()));
    return name;
}

void SyntroPythonGlue::setConnected(bool state)
{
    QMutexLocker lock(&lockSyntroPython);
    m_connected = state;
}

bool SyntroPythonGlue::isConnected()
{
    QMutexLocker lock(&lockSyntroPython);
    return m_connected;
}

bool SyntroPythonGlue::requestDirectory()
{
    QMutexLocker lock(&lockSyntroPython);

    if (!m_connected)
        return false;
    m_directoryRequested = true;
    return true;
}

bool SyntroPythonGlue::isDirectoryRequested()
{
    QMutexLocker lock(&lockSyntroPython);

    bool ret = m_directoryRequested;
    m_directoryRequested = false;
    return ret;
}

char *SyntroPythonGlue::lookupMulticastSources(const char *sourceName)
{
    return m_main->getClient()->lookupSources(sourceName, SERVICETYPE_MULTICAST);
}

int SyntroPythonGlue::addMulticastSourceService(const char *servicePath)
{
    return m_main->getClient()->clientAddService(servicePath, SERVICETYPE_MULTICAST, true);
}

int SyntroPythonGlue::addMulticastSinkService(const char *servicePath)
{
    return m_main->getClient()->clientAddService(servicePath, SERVICETYPE_MULTICAST, false);
}

char *SyntroPythonGlue::lookupE2ESources(const char *sourceType)
{
    return m_main->getClient()->lookupSources(sourceType, SERVICETYPE_E2E);
}

int SyntroPythonGlue::addE2ESourceService(const char *servicePath)
{
    return m_main->getClient()->clientAddService(servicePath, SERVICETYPE_E2E, true);
}

int SyntroPythonGlue::addE2ESinkService(const char *servicePath)
{
    return m_main->getClient()->clientAddService(servicePath, SERVICETYPE_E2E, false);
}

bool SyntroPythonGlue::removeService(int servicePort)
{
    return m_main->getClient()->clientRemoveService(servicePort);
}

bool SyntroPythonGlue::isClearToSend(int servicePort)
{
    return m_main->getClient()->clientClearToSend(servicePort);
}

bool SyntroPythonGlue::isServiceActive(int servicePort)
{
    return m_main->getClient()->clientIsServiceActive(servicePort);
}

bool SyntroPythonGlue::sendAVData(int servicePort, unsigned char *videoData, int videoLength,
                    unsigned char *audioData, int audioLength)
{
    return m_main->sendAVData(servicePort, videoData, videoLength, audioData, audioLength);
}

bool SyntroPythonGlue::getAVData(int servicePort, long long *timestamp, unsigned char **videoData, int *videoLength,
                    unsigned char **audioData, int *audioLength)
{
    return m_main->getClient()->getAVData(servicePort, timestamp, videoData, videoLength, audioData, audioLength);
}

void SyntroPythonGlue::setVideoParams(int width, int height, int rate)
{
    m_main->getClient()->setVideoParams(width, height, rate);
}

void SyntroPythonGlue::setAudioParams(int rate, int size, int channels)
{
    m_main->getClient()->setVideoParams(rate, size, channels);
}

bool SyntroPythonGlue::sendMulticastData(int servicePort, unsigned char *data, int dataLength)
{
    return m_main->sendMulticastData(servicePort, data, dataLength);
}

bool SyntroPythonGlue::sendE2EData(int servicePort, unsigned char *data, int dataLength)
{
    return m_main->sendE2EData(servicePort, data, dataLength);
}

bool SyntroPythonGlue::getMulticastData(int servicePort, unsigned char **data, int *dataLength)
{
    return m_main->getClient()->getGenericData(servicePort, data, dataLength);
}

bool SyntroPythonGlue::getE2EData(int servicePort, unsigned char **data, int *dataLength)
{
    return m_main->getClient()->getGenericData(servicePort, data, dataLength);
}



