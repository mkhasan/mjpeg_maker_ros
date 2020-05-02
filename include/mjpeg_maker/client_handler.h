// client_handler.h,v 1.9 1999/02/03 01:26:56 schmidt Exp

#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H


#include "ace/Svc_Handler.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "ace/SOCK_Stream.h"

namespace mjpeg_maker {

class Client_Handler : public ACE_Svc_Handler <ACE_SOCK_STREAM, ACE_NULL_SYNCH>
{
public:
  typedef ACE_Svc_Handler <ACE_SOCK_STREAM, ACE_NULL_SYNCH> inherited;


  Client_Handler (void);

  void destroy (void);

  int open (void *acceptor);

  int close (u_long flags = 0);

  virtual int handle_close (ACE_HANDLE handle = ACE_INVALID_HANDLE,
                            ACE_Reactor_Mask mask = ACE_Event_Handler::ALL_EVENTS_MASK);

protected:

  int svc (void);

  int handle_input (ACE_HANDLE handle);

  int process (char *rdbuf, int rdbuf_len);

  ~Client_Handler (void);
};

}

#endif /* CLIENT_HANDLER_H */
