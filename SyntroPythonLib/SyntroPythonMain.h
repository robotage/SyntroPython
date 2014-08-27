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
    virtual void displayJpegImage(QByteArray image, QString timestamp) = 0; // displays a Jpeg image in GUI mode

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

