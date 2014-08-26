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

#ifndef SYNTROPYTHONMAINWINDOW_H
#define SYNTROPYTHONMAINWINDOW_H

#include <qmainwindow.h>
#include <qlabel.h>

#include "SyntroPythonMain.h"

class SyntroPythonMainWindow : public QMainWindow, public SyntroPythonMain
{
    Q_OBJECT

public:
    SyntroPythonMainWindow(SyntroPythonGlue *glue);

    void setWindowTitle(char *title);
    void displayImage(QByteArray image, int width, int height, QString timestamp);

    bool sendAVData(int servicePort, unsigned char *videoData, int videoLength,
                        unsigned char *audioData, int audioLength); // sends an AV data message
    bool sendMulticastData(int servicePort, unsigned char *data, int dataLength); // sends a generic multicast message
    bool sendE2EData(int servicePort, unsigned char *data, int dataLength); // sends a generic E2E message

    void stopRunning();

private slots:
    void windowTitleSlot(QString title);
    void displayImageSlot(QByteArray image, int width, int height, QString timestamp);

signals:
    void windowTitleSignal(QString title);
    void displayImageSignal(QByteArray image, int width, int height, QString timestamp);
    void clientSendAVData(int servicePort, QByteArray video, QByteArray audio);
    void clientSendMulticastData(int servicePort, QByteArray data);
    void clientSendE2EData(int servicePort, QByteArray data);

private:
    QLabel *m_imageView;
};

#endif // SYNTROPYTHONMAINWINDOW_H

