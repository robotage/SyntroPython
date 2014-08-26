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

#ifndef SYNTROPYTHONCLIENT_H
#define SYNTROPYTHONCLIENT_H

#include "Endpoint.h"

#include <qmutex.h>

class SyntroPythonGlue;

class SyntroPythonClient : public Endpoint
{
    Q_OBJECT

friend class SyntroPythonGlue;

public:
    SyntroPythonClient(SyntroPythonGlue *glue);
    virtual ~SyntroPythonClient();
    char *lookupSources(const char *sourceType, int serviceType);   // gets a list of sources of the specified type

    void setVideoParams(int width, int height, int rate);   // sets the video capture data
    void setAudioParams(int rate, int size, int channels);  // sets the audio capture data
    bool getAVData(int servicePort, qint64 *timestamp, unsigned char **videoData, int *videoLength,
                        unsigned char **audioData, int *audioLength); // gets AV data if available

    bool getGenericData(int servicePort, unsigned char **data, int *dataLength); // gets generic data if available

public slots:
    void clientSendAVData(int servicePort, QByteArray video, QByteArray audio);
    void clientSendMulticastData(int servicePort, QByteArray data);
    void clientSendE2EData(int servicePort, QByteArray data);

protected:
    void appClientInit();
    void appClientExit();
    void appClientReceiveMulticast(int servicePort, SYNTRO_EHEAD *message, int length);
    void appClientReceiveE2E(int servicePort, SYNTRO_EHEAD *header, int length); // process an E2E message
    void appClientConnected();								// called when endpoint is connected to SyntroControl
    void appClientClosed();								    // called when endpoint connection closes
    void appClientBackground();
    void appClientReceiveDirectory(QStringList directory);

private:
    SyntroPythonGlue *m_glue;
    int m_recordIndex;

    SYNTRO_AVPARAMS m_avParams;                             // used to hold stream parameters
    QStringList m_directory;                                // holds the latest directory
    QList< QQueue<QByteArray> > m_receivedData;               // received data for every slot potentially
    QMutex m_lock;
};

#endif // SYNTROPYTHONCLIENT_H

