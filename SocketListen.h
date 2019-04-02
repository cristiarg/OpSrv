#pragma once

#include "SocketMessage.h"

#include <Ws2tcpip.h>

class CSocketListen final {
public:
  CSocketListen(/*const char* ac_sHostName,*/ int ac_nPort/*, int ac_nSockType, int ac_nProtocol, int ac_nFamily = AF_UNSPEC*/);
  ~CSocketListen();

  bool openBindListen();
  UXSocketMessage accept();

private:
  const int port;
  
  struct sockaddr_in local_addr;
  SOCKET socket_listen;
};

