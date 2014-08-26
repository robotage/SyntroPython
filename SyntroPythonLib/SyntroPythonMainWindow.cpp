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

#include "SyntroPythonMainWindow.h"
#include "SyntroUtils.h"
#include "SyntroPythonClient.h"
#include "SyntroPythonGlue.h"

#include <qboxlayout.h>
#include <qpainter.h>

SyntroPythonMainWindow::SyntroPythonMainWindow(SyntroPythonGlue *glue)
    : QMainWindow(), SyntroPythonMain(glue)
{
    setWindowFlags((Qt::WindowTitleHint | Qt::CustomizeWindowHint) &
                   ~(Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint));

    connect(this, SIGNAL(clientSendAVData(int,QByteArray,QByteArray)),
                         m_client, SLOT(clientSendAVData(int,QByteArray,QByteArray)));

    connect(this, SIGNAL(clientSendMulticastData(int,QByteArray)),
                         m_client, SLOT(clientSendMulticastData(int,QByteArray)));

    connect(this, SIGNAL(clientSendE2EData(int,QByteArray)),
                         m_client, SLOT(clientSendE2EData(int,QByteArray)));

    connect(this, SIGNAL(windowTitleSignal(QString)), this, SLOT(windowTitleSlot(QString)));

    connect(this, SIGNAL(displayImageSignal(QByteArray,int,int,QString)),
            this, SLOT(displayImageSlot(QByteArray,int,int,QString)));

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *verticalLayout = new QVBoxLayout(centralWidget);
    verticalLayout->setSpacing(6);
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    m_imageView = new QLabel(centralWidget);

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_imageView->sizePolicy().hasHeightForWidth());
    m_imageView->setSizePolicy(sizePolicy);
    m_imageView->setMinimumSize(QSize(320, 240));
    m_imageView->setAlignment(Qt::AlignCenter);

    verticalLayout->addWidget(m_imageView);
    setCentralWidget(centralWidget);
    show();
}

void SyntroPythonMainWindow::stopRunning()
{
    close();
}

bool SyntroPythonMainWindow::sendAVData(int servicePort, unsigned char *videoData, int videoLength,
                    unsigned char *audioData, int audioLength)
{
    if (m_mustExit)
        return false;
    emit clientSendAVData(servicePort, QByteArray((const char *)videoData, videoLength), QByteArray((const char *)audioData, audioLength));
    return true;
}

bool SyntroPythonMainWindow::sendMulticastData(int servicePort, unsigned char *data, int dataLength)
{
    if (m_mustExit)
        return false;
    emit clientSendMulticastData(servicePort, QByteArray((const char *)data, dataLength));
    return true;
}

bool SyntroPythonMainWindow::sendE2EData(int servicePort, unsigned char *data, int dataLength)
{
    if (m_mustExit)
        return false;
    emit clientSendE2EData(servicePort, QByteArray((const char *)data, dataLength));
    return true;
}

void SyntroPythonMainWindow::setWindowTitle(char *title)
{
    emit windowTitleSignal(title);
}

void SyntroPythonMainWindow::windowTitleSlot(QString title)
{
    QMainWindow::setWindowTitle(title);
}

void SyntroPythonMainWindow::displayImage(QByteArray image,
                                          int width, int height, QString timestamp)
{
    emit displayImageSignal(image, width, height, timestamp);
}

void SyntroPythonMainWindow::displayImageSlot(QByteArray image,
                                              int width, int height, QString timestamp)
{
    resize(width, height);
    QImage qi((const uchar *)image.data(), width, height, QImage::Format_RGB888);
    QImage rgbImage = qi.rgbSwapped();
    QPixmap pixmap = QPixmap::fromImage(rgbImage);

    QPainter painter(&pixmap);
    painter.setPen(Qt::yellow);
    painter.drawText(10, height - 10, timestamp);

    m_imageView->setPixmap(pixmap);
}
