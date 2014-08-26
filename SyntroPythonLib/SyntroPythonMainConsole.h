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

#ifndef SYNTROPYTHONMAINCONSOLE_H
#define SYNTROPYTHONMAINCONSOLE_H

#include <QThread>

#include "SyntroPythonMain.h"

class SyntroPythonMainConsole : public QThread, public SyntroPythonMain
{
    Q_OBJECT

public:
    SyntroPythonMainConsole(QObject *parent, SyntroPythonGlue *glue);

    void setWindowTitle(char *) {}
    void displayImage(QByteArray, int , int, QString ) {}
    bool sendAVData(int servicePort, unsigned char *videoData, int videoLength,
                        unsigned char *audioData, int audioLength); // sends an AV data message
    bool sendMulticastData(int servicePort, unsigned char *data, int dataLength); // sends a generic multicast message
    bool sendE2EData(int servicePort, unsigned char *data, int dataLength); // sends a generic E2E message

    void stopRunning() { m_mustExit = true; }

signals:
    void clientSendAVData(int servicePort, QByteArray video, QByteArray audio);
    void clientSendMulticastData(int servicePort, QByteArray data);
    void clientSendE2EData(int servicePort, QByteArray data);

protected:
    void run();
};

#endif // SYNTROPYTHONMAINCONSOLE_H

