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

#include "SyntroPythonMainWindow.h"
#include "SyntroUtils.h"
#include "SyntroPythonClient.h"
#include "SyntroPythonGlue.h"
#include "SyntroPythonVidCap.h"

#include <qboxlayout.h>
#include <qpainter.h>

SyntroPythonMainWindow::SyntroPythonMainWindow(SyntroPythonGlue *glue)
    : QMainWindow(), SyntroPythonMain(glue)
{
    setWindowFlags((Qt::WindowTitleHint | Qt::CustomizeWindowHint) &
                   ~(Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint));

    connect(this, SIGNAL(clientSendAVData(int,QByteArray,QByteArray)),
                         m_client, SLOT(clientSendAVData(int,QByteArray,QByteArray)));

    connect(this, SIGNAL(clientSendJpegAVData(int,QByteArray,QByteArray)),
                         m_client, SLOT(clientSendJpegAVData(int,QByteArray,QByteArray)));

    connect(this, SIGNAL(clientSendMulticastData(int,QByteArray)),
                         m_client, SLOT(clientSendMulticastData(int,QByteArray)));

    connect(this, SIGNAL(clientSendE2EData(int,QByteArray)),
                         m_client, SLOT(clientSendE2EData(int,QByteArray)));

    connect(this, SIGNAL(windowTitleSignal(QString)), this, SLOT(windowTitleSlot(QString)));

    connect(this, SIGNAL(displayImageSignal(QByteArray,int,int,QString)),
            this, SLOT(displayImageSlot(QByteArray,int,int,QString)));

    connect(this, SIGNAL(displayJpegImageSignal(QByteArray,QString)),
            this, SLOT(displayJpegImageSlot(QByteArray,QString)));

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
}

void SyntroPythonMainWindow::addVidCapSignal(SyntroPythonVidCap *vidCap)
{
    connect(vidCap, SIGNAL(newFrame(int,QByteArray,bool,int,int,int)), this, SLOT(newFrame(int,QByteArray,bool,int,int,int)));
}

void SyntroPythonMainWindow::newFrame(int cameraNum, QByteArray frame, bool jpeg, int width, int height, int rate)
{
    addFrameToQueue(cameraNum, frame, jpeg, width, height, rate);
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

bool SyntroPythonMainWindow::sendJpegAVData(int servicePort, unsigned char *videoData, int videoLength,
                    unsigned char *audioData, int audioLength)
{
    if (m_mustExit)
        return false;
    emit clientSendJpegAVData(servicePort, QByteArray((const char *)videoData, videoLength), QByteArray((const char *)audioData, audioLength));
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

void SyntroPythonMainWindow::displayJpegImage(QByteArray image, QString timestamp)
{
    emit displayJpegImageSignal(image, timestamp);
}

void SyntroPythonMainWindow::displayImageSlot(QByteArray image,
                                              int width, int height, QString timestamp)
{
    QImage qi((const uchar *)image.data(), width, height, QImage::Format_RGB888);
    QImage rgbImage = qi.rgbSwapped();
    displayPixmap(rgbImage, timestamp);
}

void SyntroPythonMainWindow::displayJpegImageSlot(QByteArray image, QString timestamp)
{
    QImage img;
    img.loadFromData(image, "JPEG");
    displayPixmap(img, timestamp);
}

void SyntroPythonMainWindow::displayPixmap(const QImage& image, const QString& timestamp)
{
    QPixmap pixmap = QPixmap::fromImage(image);

    QPainter painter(&pixmap);
    painter.setPen(Qt::yellow);
    painter.drawText(10, image.height() - 10, timestamp);

    m_imageView->setPixmap(pixmap);
    resize(image.width(), image.height());
}
