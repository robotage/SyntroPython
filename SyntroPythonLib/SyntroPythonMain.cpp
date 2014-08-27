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

#include "SyntroPythonMain.h"
#include "SyntroUtils.h"
#include "SyntroPythonClient.h"
#include "SyntroPythonGlue.h"
#include "SyntroPythonVidCap.h"

//----------------------------------------------------------
//
//  SyntroPythonMain

SyntroPythonMain::SyntroPythonMain(SyntroPythonGlue *glue)
{
    m_client = NULL;
    m_glue = glue;

    SyntroUtils::syntroAppInit();

    m_client = new SyntroPythonClient(glue);

    m_client->resumeThread();
    m_mustExit = false;
}

SyntroPythonMain::~SyntroPythonMain()
{
    QMutexLocker lock(&m_vidCapLock);

    for (int i = 0; i < m_vidCaps.count(); i++)
        removeVidCap(i);
    m_vidCaps.clear();
}

bool SyntroPythonMain::vidCapOpen(int cameraNum, int width, int height, int rate)
{
    QMutexLocker lock(&m_vidCapLock);

    SYNTROPYTHON_CAPTURE *cap;

    if (cameraNum < 0)
        return false;

    for (int i = 0; cameraNum >= m_vidCaps.count(); i++)
        m_vidCaps.append(NULL);

    if (m_vidCaps[cameraNum] != NULL)
        removeVidCap(cameraNum);

    cap = new SYNTROPYTHON_CAPTURE;
    cap->vidCap = new SyntroPythonVidCap(cameraNum, width, height, rate);
    cap->cameraNum = cameraNum;
    cap->width = width;
    cap->height = height;
    cap->rate = rate;
    cap->jpeg = true;
    m_vidCaps[cameraNum] = cap;

    cap->vidCap->resumeThread();
    addVidCapSignal(cap->vidCap);
    return true;
}

bool SyntroPythonMain::vidCapClose(int cameraNum)
{
    QMutexLocker lock(&m_vidCapLock);

    if ((cameraNum < 0) || (cameraNum >= m_vidCaps.count()))
        return false;

    removeVidCap(cameraNum);
    return true;
}


void SyntroPythonMain::addFrameToQueue(int cameraNum, const QByteArray& frame, bool jpeg, int width, int height, int rate)
{
    QMutexLocker lock(&m_vidCapLock);

    if (cameraNum >= m_vidCaps.count())
        return;

    SYNTROPYTHON_CAPTURE *cap = m_vidCaps[cameraNum];

    if (cap == NULL)
        return;

    cap->frameQueue.append(frame);
    if (cap->frameQueue.count() > 5)
        cap->frameQueue.dequeue();

    cap->width = width;
    cap->height = height;
    cap->rate = rate;
    cap->jpeg = jpeg;
}

bool SyntroPythonMain::vidCapGetFrame(int cameraNum, QByteArray& frame, bool& jpeg,
                                         int& width, int& height, int& rate)
{
    QMutexLocker lock(&m_vidCapLock);

    if (cameraNum >= m_vidCaps.count())
        return false;

    SYNTROPYTHON_CAPTURE *cap = m_vidCaps[cameraNum];

    if (cap == NULL)
        return false;

    if (cap->frameQueue.empty())
        return false;

    frame = cap->frameQueue.dequeue();
    jpeg = cap->jpeg;
    width = cap->width;
    height = cap->height;
    rate = cap->rate;
    jpeg = cap->jpeg;
    return true;
}

void SyntroPythonMain::removeVidCap(int cameraNum)
{
    SYNTROPYTHON_CAPTURE *cap = m_vidCaps[cameraNum];
    if (cap == NULL)
        return;
    if (cap->vidCap != NULL) {
        cap->vidCap->exitThread();
        cap->vidCap = NULL;
    }
    delete cap;
    m_vidCaps[cameraNum] = NULL;
}
