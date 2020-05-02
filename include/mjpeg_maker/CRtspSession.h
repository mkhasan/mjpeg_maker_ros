// MediaLAN 02/2013
// CRtspSession
// - parsing of RTSP requests and generation of RTSP responses


#ifndef _CRtsp_Session_H
#define _CRtsp_Session_H

#include <iostream>


#include "client_handler.h"


#include "CStreamer.h"

using namespace std;

#define nullptr NULL
#define _snprintf snprintf


namespace mjpeg_maker {
// supported command types
enum RTSP_CMD_TYPES
{
    RTSP_OPTIONS,
    RTSP_DESCRIBE,
    RTSP_SETUP,
    RTSP_PLAY,
    RTSP_TEARDOWN,
    RTSP_UNKNOWN
};

#define RTSP_BUFFER_SIZE       10000    // for incoming requests, and outgoing responses
#define RTSP_PARAM_STRING_MAX  200  

class CRtspSession
{
	char buf[200];
public:
    CRtspSession(Client_Handler *_aClientHandler, CStreamer * aStreamer);
    ~CRtspSession();

    RTSP_CMD_TYPES Handle_RtspRequest(char const * aRequest, unsigned aRequestSize);
    int            GetStreamID();

private:

    inline void send(Client_Handler * clientHandler, char *p, int len, int not_used)
    {
    	if (clientHandler->peer().send_n(p, len) == -1)
    	{
    		p[len] = 0;
    		cout << "Error in sending " << p << endl;

    		char *p1 = "hello";
    		int ret = clientHandler->peer().send(p1, strlen(p1));
    		cout << "return is: " << ret << endl;
    	}
    }

    void Init();
    bool ParseRtspRequest(char const * aRequest, unsigned aRequestSize);
    char const * DateHeader();

    // RTSP request command handlers
    void Handle_RtspOPTION();
    void Handle_RtspDESCRIBE();
    void Handle_RtspSETUP(); 
    void Handle_RtspPLAY();

    // global session state parameters
    int            m_RtspSessionID;
    Client_Handler * m_RtspClientHandler;                     // RTSP socket of that session
    int            m_StreamID;                                // number of simulated stream of that session
    u_short        m_ClientRTPPort;                           // client port for UDP based RTP transport
    u_short        m_ClientRTCPPort;                          // client port for UDP based RTCP transport  
    bool           m_TcpTransport;                            // if Tcp based streaming was activated
    CStreamer    * m_Streamer;                                // the UDP or TCP streamer of that session

    // parameters of the last received RTSP request

    RTSP_CMD_TYPES m_RtspCmdType;                             // command type (if any) of the current request
    char           m_URLPreSuffix[RTSP_PARAM_STRING_MAX];     // stream name pre suffix 
    char           m_URLSuffix[RTSP_PARAM_STRING_MAX];        // stream name suffix
    char           m_CSeq[RTSP_PARAM_STRING_MAX];             // RTSP command sequence number
    char           m_URLHostPort[RTSP_BUFFER_SIZE];           // host:port part of the URL
    unsigned       m_ContentLength;                           // SDP string size
};

}
#endif
