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
    void clientSendJpegAVData(int servicePort, QByteArray video, QByteArray audio);
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

