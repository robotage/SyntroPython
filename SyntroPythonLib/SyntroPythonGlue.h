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

#ifndef SYNTROPYTHONGLUE_H
#define SYNTROPYTHONGLUE_H

class SyntroPythonMain;

class SyntroPythonGlue
{
public:
    SyntroPythonGlue();
    virtual ~SyntroPythonGlue();

    //  functions called from Python

    void startLib(const char *productType, int& argc, char **argv);
    void stopLib();
    void setWindowTitle(char *title);                       // sets the window title in GUI mode
    void displayImage(unsigned char *image, int length,
                      int width, int height, char *timestamp); // displays an image in GUI mode
    char *getAppName();                                     // returns the app name
    bool isConnected();                                     // returns true if link connected

    bool requestDirectory();                                // requests directory at next opportunity

    int addMulticastSourceService(const char *servicePath); // adds a multicast source and returns the service port or -1 if error
    int addMulticastSinkService(const char *servicePath);   // adds a multicast sink and returns the service port or -1 if error
    int addE2ESourceService(const char *servicePath);       // adds a E2E source and returns the service port or -1 if error
    int addE2ESinkService(const char *servicePath);         // adds a E2E sink and returns the service port or -1 if error
    bool removeService(int servicePort);                    // removes the service associated with servicePort

    char *lookupMulticastSources(const char *sourceType);   // gets a list of multicast sources of the specified type
    char *lookupE2ESources(const char *sourceType);         // gets a list of E2E sources of the specified type

    bool isClearToSend(int servicePort);                    // returns true if window open on multicast source service
    bool isServiceActive(int servicePort);                  // returns true if service is active (multicast source or remote E2E)

    void setVideoParams(int width, int height, int rate);   // sets the video capture data
    void setAudioParams(int rate, int size, int channels);  // sets the audio capture data
    bool sendAVData(int servicePort, unsigned char *videoData, int videoLength,
                        unsigned char *audioData, int audioLength); // sends an AV data message
    bool getAVData(int servicePort, long long *timestamp, unsigned char **videoData, int *videoLength,
                        unsigned char **audioData, int *audioLength); // gets AV data if available

    bool sendMulticastData(int servicePort, unsigned char *data, int dataLength); // sends a generic multicast message
    bool sendE2EData(int servicePort, unsigned char *data, int dataLength); // sends a generic E2E message

    bool getMulticastData(int servicePort, unsigned char **data, int *dataLength);  // get generic multicast data
    bool getE2EData(int servicePort, unsigned char **data, int *dataLength);  // get generic E2E data

    //  functions called by client

    void setConnected(bool state);                          // sets whether link connected or not
    bool isDirectoryRequested();                            // true if a directory has been requested

private:
    SyntroPythonMain *m_main;

    bool m_connected;
    bool m_directoryRequested;

    int m_argc;
    char **m_argv;
};

#endif // SYNTROPYTHONGLUE_H

