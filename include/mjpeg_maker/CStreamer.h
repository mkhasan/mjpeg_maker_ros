// MediaLAN 02/2013
// CRtspSession
// - JPEG packetizer and UDP/TCP based streaming

#ifndef _CStreamer_H
#define _CStreamer_H

#include <fstream>
#include <ace/Task.h>
#include <ace/SOCK_Dgram.h>

//#include "stream_source.h"

#ifndef DWORD
#define DWORD unsigned int
#endif


namespace mjpeg_maker {
class Client_Handler;
class StreamSource;

class CStreamer
{

	friend class Client_Handler;
	const unsigned char QUALITY_FACTOR = 0x5e;

public:

	const double frameRate = 40.0;

	int streamStarted;

	int finished;

	char *data;
	int dataLen;

private:
	StreamSource * streamSource;

public:

    CStreamer(Client_Handler *_aClientHandler);
    ~CStreamer();

    void    InitTransport(u_short aRtpPort, u_short aRtcpPort, bool TCP);
    u_short GetRtpServerPort();
    u_short GetRtcpServerPort();
    //void    StreamImage(char * data, int imageWidth, int imageHeight);
    void    StreamImage(char * data, int len, int width, int height);


    unsigned char GetQualityFactor();
    void SetStreamerID(int streamID);


private:

public:
	static bool IsSOI(char x, char y);
	static bool IsAPP0(char x, char y);
	static bool IsDQT(char x, char y);
	static bool IsSOF(char x, char y);
	static bool IsDHT(char x, char y);
	static bool IsSOS(char x, char y);
	static int GetLength(char x, char y);
private:
	static int GetPayLoad(const char *data, int data_len, int & width, int & height);
	static int GetDataFromFile(char * data);
	static std::ifstream::pos_type filesize(const char* filename);
    void    SendRtpPacket(char * Jpeg, int JpegLen, int width, int height);
    void    SendRtpPacket(char * Jpeg, int JpegLen, int width, int height, bool isLastPacket, unsigned int offset);

    char * GetData(int image_len, int & payload_len);



    ACE_SOCK_Dgram  m_RtpSocket;          // RTP socket for streaming RTP packets to client
    ACE_SOCK_Dgram  m_RtcpSocket;         // RTCP socket for sending/receiving RTCP packages

    u_short m_RtpClientPort;      // RTP receiver port on client 
    u_short m_RtcpClientPort;     // RTCP receiver port on client
    u_short m_RtpServerPort;      // RTP sender port on server 
    u_short m_RtcpServerPort;     // RTCP sender port on server

    u_short m_SequenceNumber;
    DWORD   m_Timestamp;
    int     m_SendIdx;
    bool    m_TCPTransport;
    Client_Handler * m_ClientHandler;
};

}
#endif
