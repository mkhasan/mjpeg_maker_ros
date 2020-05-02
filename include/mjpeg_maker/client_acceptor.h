// client_acceptor.h,v 1.5 1999/09/22 03:13:38 jcej Exp

#ifndef CLIENT_ACCEPTOR_H
#define CLIENT_ACCEPTOR_H


#include "ace/Acceptor.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "ace/SOCK_Acceptor.h"


#include "client_handler.h"

namespace mjpeg_maker {

typedef ACE_Acceptor <Client_Handler, ACE_SOCK_ACCEPTOR> Client_Acceptor_Base;

class Client_Acceptor : public Client_Acceptor_Base
{
public:
  typedef Client_Acceptor_Base inherited;

  Client_Acceptor (int thread_per_connection = 1)
    : thread_per_connection_ (thread_per_connection)
  {
  }

  int thread_per_connection (void)
  {
    return this->thread_per_connection_;
  }

protected:
  int thread_per_connection_;
};

}

#endif /* CLIENT_ACCEPTOR_H */
