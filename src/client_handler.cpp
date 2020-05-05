#include "mjpeg_maker/client_acceptor.h"
#include "mjpeg_maker/client_handler.h"
#include "mjpeg_maker/CStreamer.h"
#include "mjpeg_maker/CRtspSession.h"
#include "mjpeg_maker/fake_source.h"

//void * Start(void * arg);

#define ACE_DEBUG(...)


namespace mjpeg_maker {

Client_Handler::Client_Handler (void)
{
}

Client_Handler::~Client_Handler (void)
{
}

void
Client_Handler::destroy (void)
{
  this->reactor ()->remove_handler (this,
                                    ACE_Event_Handler::READ_MASK
                                    | ACE_Event_Handler::DONT_CALL);

  delete this;
}

int
Client_Handler::open (void *void_acceptor)
{
	ACE_Log_Msg::disable_debug_messages ();
  ACE_INET_Addr addr;

  ACE_DEBUG ((LM_DEBUG,
  					  "(%P|%t) new connection ++++++++++++++++++++++++++++++++++++"));
  if (this->peer ().get_remote_addr (addr) == -1)
    return -1;

  Client_Acceptor *acceptor = (Client_Acceptor *) void_acceptor;

  if (acceptor->thread_per_connection ())
    return this->activate (THR_DETACHED);

  this->reactor (acceptor->reactor ());

  if (this->reactor ()->register_handler (this,
                                          ACE_Event_Handler::READ_MASK) == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "(%P|%t) can't register with reactor\n"),
                      -1);

  ACE_DEBUG ((LM_DEBUG,
              "(%P|%t) connected with %s\n", addr.get_host_name ()));


  return 0;
}

int
Client_Handler::close(u_long flags)
{
  ACE_UNUSED_ARG (flags);

  this->destroy ();

  return 0;
}

int
Client_Handler::handle_input (ACE_HANDLE handle)
{
  ACE_UNUSED_ARG (handle);

  char buf[BUFSIZ];

  return this->process (buf, sizeof (buf));
}

int
Client_Handler::handle_close (ACE_HANDLE handle,
                              ACE_Reactor_Mask mask)
{
  ACE_UNUSED_ARG (handle);
  ACE_UNUSED_ARG (mask);

  printf("handler closeing");
  this->destroy ();
  return 0;
}

int
Client_Handler::svc(void)
{
	char buf[BUFSIZ];

	// Forever...

	CStreamer    Streamer(this);                  // our streamer for UDP/TCP based RTP transport
	CRtspSession RtspSession(this,&Streamer);

	int err;
	char *b;



	//StreamSource streamSource;

	// 768 × 576
	// /home/usrc/hdd/opencv/samples/data/vtest.avi
	//streamSource = new FakeSource(FakeSource::WIDTH, FakeSource::HEIGHT, &Streamer, "/media/hasan/External/Movie/IceAge.avi");



	// /media/hasan/External/hdd/water_detection/clips/smoke.mp4 (1920 x 1080)

	//streamSource = new FakeSource(1920, 1080, &Streamer, "/media/hasan/External/hdd/water_detection/clips/smoke.mp4");

	// /media/hasan/External/Movie/IceAge.avi : (520 x 274)
	//streamSource = new FakeSource(520, 274, &Streamer, "/media/hasan/External/Movie/IceAge.avi");

	// /home/hasan/hdd/vtest.avi : (768 x 576)



	//FakeSource streamSource(&Streamer, RtspSession.GetStreamID());
	//StreamSource * fakeSource = new FakeSource(&Streamer, RtspSession.GetStreamID());


	bool Stop = false;




	int bytes_read;

	while (!Stop)
	{
		memset(buf,0x00,sizeof(buf));

		if ((bytes_read = this->process(buf, sizeof (buf))) == -1)
		{
			return -1;
		}



		if ((buf[0] == 'O') || (buf[0] == 'D') || (buf[0] == 'S') || (buf[0] == 'P') || (buf[0] == 'T'))
		{
			if (buf[0] == 'T')
			{
				//printf("stopped \n");
			}
			RTSP_CMD_TYPES C = RtspSession.Handle_RtspRequest(buf,bytes_read);
			if (C == RTSP_PLAY)
			{
				//TheImageProc::instance()->get_frame = 1;
				//cout << "done 2" << endl;
				//ACE_OS::sleep(ACE_Time_Value(0,500000));
				printf("started .........................................................\n");
				//streamSource->SetStreamID(RtspSession.GetStreamID());

				Streamer.SetStreamerID(RtspSession.GetStreamID());
				Streamer.streamStarted = 1;

			}
			else if (C == RTSP_TEARDOWN)
			{
				Stop = true;

				printf("stopped ...\n");
				Streamer.streamStarted = 0;

			}


		};
	};

	close();

	Streamer.finished = 1;


	printf("return value is %d \n", b);

	free(b);


	//delete fakeSource;

  return 0;
}

int
Client_Handler::process (char *rdbuf,
                         int rdbuf_len)
{



	ssize_t bytes_read;
	bytes_read = this->peer ().recv (rdbuf, rdbuf_len);

	switch (bytes_read )
	{
		case -1:
		  ACE_ERROR_RETURN ((LM_ERROR,
							 "(%P|%t) %p bad read\n",
							 "client"),
							-1);
		case 0:
		  ACE_ERROR_RETURN ((LM_ERROR,
							 "(%P|%t) closing daemon (fd = %d)\n",
							 this->get_handle ()),
							-1);
		default:
		  rdbuf[bytes_read] = 0;
		  ACE_DEBUG ((LM_DEBUG,
					  "(%P|%t) from client: %s",
					  rdbuf));
	}

	/*

	ACE_DEBUG ((LM_DEBUG,
			  "(%P|%t) from client: %s",
			  rdbuf));
			  */


	//cout << "done " << endl;
	return bytes_read;
}

}
