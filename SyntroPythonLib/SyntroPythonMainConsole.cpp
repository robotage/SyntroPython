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

