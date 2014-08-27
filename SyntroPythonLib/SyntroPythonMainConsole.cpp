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

#include "SyntroPythonMainConsole.h"
#include "SyntroUtils.h"
#include "SyntroPythonClient.h"
#include "SyntroPythonGlue.h"

SyntroPythonMainConsole::SyntroPythonMainConsole(QObject *parent, SyntroPythonGlue *glue)
    : QThread(parent), SyntroPythonMain(glue)
{
    connect(this, SIGNAL(clientSendAVData(int,QByteArray,QByteArray)),
                         m_client, SLOT(clientSendAVData(int,QByteArray,QByteArray)));

    connect(this, SIGNAL(clientSendMulticastData(int,QByteArray)),
                         m_client, SLOT(clientSendMulticastData(int,QByteArray)));

    connect(this, SIGNAL(clientSendE2EData(int,QByteArray)),
                         m_client, SLOT(clientSendE2EData(int,QByteArray)));

    start();
}

void SyntroPythonMainConsole::run()
{
    while(!m_mustExit) {
        msleep(10);
    }

    m_client->exitThread();
    SyntroUtils::syntroAppExit();
    QCoreApplication::exit();
}

bool SyntroPythonMainConsole::sendAVData(int servicePort, unsigned char *videoData, int videoLength,
                    unsigned char *audioData, int audioLength)
{
    if (m_mustExit)
        return false;
    emit clientSendAVData(servicePort, QByteArray((const char *)videoData, videoLength), QByteArray((const char *)audioData, audioLength));
    return true;
}

bool SyntroPythonMainConsole::sendMulticastData(int servicePort, unsigned char *data, int dataLength)
{
    if (m_mustExit)
        return false;
    emit clientSendMulticastData(servicePort, QByteArray((const char *)data, dataLength));
    return true;
}

bool SyntroPythonMainConsole::sendE2EData(int servicePort, unsigned char *data, int dataLength)
{
    if (m_mustExit)
        return false;
    emit clientSendE2EData(servicePort, QByteArray((const char *)data, dataLength));
    return true;
}

