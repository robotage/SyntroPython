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

void SyntroPythonGlue::startLib(const char *productType, int& argc, char **argv, bool showWindow)
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
        if (showWindow)
            ((SyntroPythonMainWindow *)m_main)->show();
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

void SyntroPythonGlue::displayJpegImage(unsigned char *image, int length, char *timestamp)
{
    m_main->displayJpegImage(QByteArray((const char *)image, length), timestamp);
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

bool SyntroPythonGlue::sendJpegAVData(int servicePort, unsigned char *videoData, int videoLength,
                    unsigned char *audioData, int audioLength)
{
    return m_main->sendJpegAVData(servicePort, videoData, videoLength, audioData, audioLength);
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

bool SyntroPythonGlue::vidCapOpen(int cameraNum, int width, int height, int rate)
{
    return m_main->vidCapOpen(cameraNum, width, height, rate);
}

bool SyntroPythonGlue::vidCapClose(int cameraNum)
{
    return m_main->vidCapClose(cameraNum);
}

bool SyntroPythonGlue::vidCapGetFrame(int cameraNum, unsigned char** frame, int& length, bool& jpeg,
                                         int& width, int& height, int& rate)
{
    QByteArray qframe;

    *frame = NULL;

    if (!m_main->vidCapGetFrame(cameraNum, qframe, jpeg, width, height, rate))
        return false;
    *frame = (unsigned char *)malloc(qframe.length());
    memcpy(*frame, qframe.data(), qframe.length());
    length = qframe.length();
    return true;
}

