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

#ifndef SYNTROPYTHONMAIN_H
#define SYNTROPYTHONMAIN_H

#include <QThread>
#include <qdialog.h>

class SyntroPythonClient;
class SyntroPythonGlue;

class SyntroPythonMain
{
public:
    SyntroPythonMain(SyntroPythonGlue *glue);

    SyntroPythonClient *getClient() { return m_client; }

    virtual void setWindowTitle(char *title) = 0;           // sets the window title in GUI mode
    virtual void displayImage(QByteArray image, int width, int height, QString timestamp) = 0; // displays an image in GUI mode

    virtual void stopRunning() = 0;
    virtual bool sendAVData(int servicePort, unsigned char *videoData, int videoLength,
                        unsigned char *audioData, int audioLength) = 0; // sends an AV data message
    virtual bool sendMulticastData(int servicePort, unsigned char *data, int dataLength) = 0; // sends a generic multicast message
    virtual bool sendE2EData(int servicePort, unsigned char *data, int dataLength) = 0; // sends a generic E2E message

protected:
    SyntroPythonClient *m_client;
    SyntroPythonGlue *m_glue;
    bool m_mustExit;
};
#endif // SYNTROPYTHONMAIN_H

