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

#include "SyntroPythonClient.h"
#include "SyntroUtils.h"
#include "SyntroPythonGlue.h"

#include "DirectoryEntry.h"

#include <qdebug.h>

SyntroPythonClient::SyntroPythonClient(SyntroPythonGlue *glue)
    : Endpoint(100, "SyntroPythonClient")
{
    m_glue = glue;
    m_recordIndex = 0;
    m_avParams.avmuxSubtype = SYNTRO_RECORD_TYPE_AVMUX_MJPPCM;
    m_avParams.videoSubtype = SYNTRO_RECORD_TYPE_VIDEO_MJPEG;
    m_avParams.audioSubtype = SYNTRO_RECORD_TYPE_AUDIO_PCM;
    m_avParams.videoWidth = 640;
    m_avParams.videoHeight = 480;
    m_avParams.videoFramerate = 10;
    m_avParams.audioSampleRate = 8000;
    m_avParams.audioChannels = 2;
    m_avParams.audioSampleSize = 16;
}

SyntroPythonClient::~SyntroPythonClient()
{
}

void SyntroPythonClient::setVideoParams(int width, int height, int rate)
{
    m_avParams.videoWidth = width;
    m_avParams.videoHeight = height;
    m_avParams.videoFramerate = rate;
}

void SyntroPythonClient::setAudioParams(int rate, int size, int channels)
{
    m_avParams.audioSampleRate = rate;
    m_avParams.audioChannels = channels;
    m_avParams.audioSampleSize = size;
}

void SyntroPythonClient::appClientInit()
{
}

void SyntroPythonClient::appClientExit()
{

}

void SyntroPythonClient::appClientBackground()
{
    if (clientIsConnected() && m_glue->isDirectoryRequested())
        requestDirectory();
}

void SyntroPythonClient::appClientReceiveDirectory(QStringList directory)
{
    QMutexLocker lock (&m_lock);

    m_directory = directory;
}

void SyntroPythonClient::appClientConnected()
{
    m_glue->setConnected(true);
}

void SyntroPythonClient::appClientClosed()
{
    QMutexLocker lock (&m_lock);

    m_directory.clear();
    m_glue->setConnected(false);
    m_receivedData.clear();
}

void SyntroPythonClient::appClientReceiveMulticast(int servicePort, SYNTRO_EHEAD *message, int length)
{
    QMutexLocker lock(&m_lock);

    while (servicePort >= m_receivedData.count())
        m_receivedData.append(QQueue<QByteArray>());

    m_receivedData[servicePort].append(QByteArray((const char *)(message + 1), length));
    if (m_receivedData[servicePort].count() > 5)            // stop queue getting too long
        m_receivedData[servicePort].dequeue();
    clientSendMulticastAck(servicePort);
    free(message);
}

void SyntroPythonClient::appClientReceiveE2E(int servicePort, SYNTRO_EHEAD *message, int length)
{
    QMutexLocker lock(&m_lock);

    while (servicePort >= m_receivedData.count())
        m_receivedData.append(QQueue<QByteArray>());

    m_receivedData[servicePort].append(QByteArray((const char *)(message + 1), length));
    if (m_receivedData[servicePort].count() > 5)            // stop queue getting too long
        m_receivedData[servicePort].dequeue();
    free(message);
}

void SyntroPythonClient::clientSendAVData(int servicePort, QByteArray video, QByteArray audio)
{
    if (clientIsServiceActive(servicePort) && clientClearToSend(servicePort)) {
        SYNTRO_EHEAD *multiCast = clientBuildMessage(servicePort, sizeof(SYNTRO_RECORD_AVMUX)
                                                  + video.size() + audio.size());
        SYNTRO_RECORD_AVMUX *avHead = (SYNTRO_RECORD_AVMUX *)(multiCast + 1);
        SyntroUtils::avmuxHeaderInit(avHead, &m_avParams, SYNTRO_RECORDHEADER_PARAM_NORMAL, m_recordIndex++, 0, video.size(), audio.size());
        SyntroUtils::convertInt64ToUC8(QDateTime::currentMSecsSinceEpoch(), avHead->recordHeader.timestamp);

        unsigned char *ptr = (unsigned char *)(avHead + 1);

        if (video.size() > 0) {
            memcpy(ptr, video.data(), video.size());
            ptr += video.size();
        }

        if (audio.size() > 0) {
            memcpy(ptr, audio.data(), audio.size());
        }

        int length = sizeof(SYNTRO_RECORD_AVMUX) + video.size() + audio.size();
        clientSendMessage(servicePort, multiCast, length, SYNTROLINK_MEDPRI);
    }
}

void SyntroPythonClient::clientSendMulticastData(int servicePort, QByteArray data)
{
    if (clientIsServiceActive(servicePort) && clientClearToSend(servicePort)) {
        SYNTRO_EHEAD *multiCast = clientBuildMessage(servicePort, data.size());
        memcpy(multiCast + 1, data.data(), data.size());
        clientSendMessage(servicePort, multiCast, data.size(), SYNTROLINK_MEDPRI);
    }
}

void SyntroPythonClient::clientSendE2EData(int servicePort, QByteArray data)
{
    if (clientIsServiceActive(servicePort)) {
        SYNTRO_EHEAD *multiCast = clientBuildMessage(servicePort, data.size());
        memcpy(multiCast + 1, data.data(), data.size());
        clientSendMessage(servicePort, multiCast, data.size(), SYNTROLINK_MEDPRI);
    }
}

char *SyntroPythonClient::lookupSources(const char *sourceName, int serviceType)
{
    DirectoryEntry de;
    QString servicePath;
    QString streamName;
    QString streamSource;
    static char availableSources[SYNTRO_MAX_DELENGTH];
    QStringList services;
    QMutexLocker lock(&m_lock);

    availableSources[0] = 0;

    for (int i = 0; i < m_directory.count(); i++) {
        de.setLine(m_directory.at(i));

        if (!de.isValid())
            continue;

        if (serviceType == SERVICETYPE_MULTICAST)
            services = de.multicastServices();
        else
            services = de.e2eServices();

        for (int i = 0; i < services.count(); i++) {
            servicePath = de.appName() + SYNTRO_SERVICEPATH_SEP + services.at(i);

            SyntroUtils::removeStreamNameFromPath(servicePath, streamSource, streamName);

            if (streamName == sourceName) {
                strcat(availableSources, qPrintable(streamSource));
                strcat(availableSources, "\n");
            }
        }
    }
    return availableSources;
}

bool SyntroPythonClient::getAVData(int servicePort, qint64 *timestamp, unsigned char **videoData, int *videoLength,
                    unsigned char **audioData, int *audioLength)
{
    unsigned char *rxVideoData, *rxAudioData;
    int rxMuxLength, rxVideoLength, rxAudioLength;

    *videoData = *audioData = NULL;
    *videoLength = *audioLength = 0;

    QMutexLocker lock(&m_lock);

    if (servicePort >= m_receivedData.count())
        return false;                                       // no queue for this service port yet

    if (m_receivedData[servicePort].empty())
        return false;                                       // no data in the queue

    QByteArray data = m_receivedData[servicePort].dequeue();

    SYNTRO_RECORD_AVMUX *avmuxHeader = (SYNTRO_RECORD_AVMUX *)data.data();

    int recordType = SyntroUtils::convertUC2ToUInt(avmuxHeader->recordHeader.type);

    if (recordType != SYNTRO_RECORD_TYPE_AVMUX)
        return false;                                       // incorrect record type

    if (!SyntroUtils::avmuxHeaderValidate(avmuxHeader, data.length(),
                    NULL, rxMuxLength, &rxVideoData, rxVideoLength, &rxAudioData, rxAudioLength))
        return false;                                       // something wrong with data

    *timestamp = SyntroUtils::convertUC8ToInt64(avmuxHeader->recordHeader.timestamp);

    if (rxVideoLength != 0) {
        // there is video data present

        if ((rxVideoLength < 0) || (rxVideoLength > 300000))
            return false;                               // illegal size

        *videoData = (unsigned char *)malloc(rxVideoLength);
        memcpy(*videoData, rxVideoData, rxVideoLength);
        *videoLength = rxVideoLength;
    }
    if (rxAudioLength != 0) {
        // there is audio data present

        if ((rxAudioLength < 0) || (rxAudioLength > 300000)) {
            if (*videoData != NULL)
                free(*videoData);
            *videoData = NULL;
            return false;                               // illegal size
        }

        *audioData = (unsigned char *)malloc(rxAudioLength);
        memcpy(*audioData, rxAudioData, rxAudioLength);
        *audioLength = rxAudioLength;
    }
    return true;
}

bool SyntroPythonClient::getGenericData(int servicePort, unsigned char **data, int *dataLength)
{
    *data = NULL;
    *dataLength = 0;

    QMutexLocker lock(&m_lock);

    if (servicePort >= m_receivedData.count())
        return false;                                       // no queue for this service port yet

    if (m_receivedData[servicePort].empty())
        return false;                                       // no data in the queue

    QByteArray rxData = m_receivedData[servicePort].dequeue();

    *data = (unsigned char *)malloc(rxData.size());
    memcpy(*data, rxData.data(), rxData.size());
    *dataLength = rxData.size();

    return true;
}

