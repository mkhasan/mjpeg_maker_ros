// server.cpp,v 1.7 2000/03/19 20:09:23 jcej Exp


#include "ace/Signal.h"
#include "mjpeg_maker/client_acceptor.h"

#define RTSP_PORT 8554



int quit = 0;


static const u_short PORT = RTSP_PORT;//ACE_DEFAULT_SERVER_PORT;


#define CTRL_C 2
#define CTRL_Z 20
#define BROKEN_PIPE 13



namespace mjpeg_maker {

ACE_Reactor *reactor;

void Finish() {

	reactor->close();
	quit=1;
}

void * main1 (void *)
{

   printf("url=rtsp://127.0.0.1:%d/mjpeg/1\n", PORT);


   //ACE_Reactor reactor;
   reactor = new ACE_Reactor();
   Client_Acceptor peer_acceptor;

    if (peer_acceptor.open (ACE_INET_Addr (PORT),
                          reactor) == -1) {
    	ACE_ERROR ((LM_ERROR,
                       "%p\n",
                       "open"));
    	return NULL;
    }

//    reactor.close();

    //ACE_Sig_Action sa ((ACE_SignalHandler) handler, SIGTERM |SIGINT |SIGPIPE|SIGQUIT | SIGTSTP);
  //ACE_Sig_Action sa2 ((ACE_SignalHandler) handler2, SIGPIPE);

  /* Like ACE_ERROR_RETURN, the ACE_DEBUG macro gets used quite a bit.
    It's a handy way to generate uniform debug output from your
    program.  */
  ACE_DEBUG ((LM_DEBUG,
              "(%P|%t) starting up server daemon\n"));

  while (!quit)
    reactor->handle_events ();

  ACE_DEBUG ((LM_DEBUG,
              "(%P|%t) shutting down server daemon\n"));

  return NULL;
}

}

#if !defined(ACE_HAS_GNU_REPO)
#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Acceptor <Client_Handler, ACE_SOCK_ACCEPTOR>;
template class ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Acceptor <Client_Handler, ACE_SOCK_ACCEPTOR>
#pragma instantiate ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
#endif /* ACE_HAS_GNU_REPO */

