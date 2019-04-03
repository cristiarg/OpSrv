#include "SocketListen.h"

#include <sstream>

#define gm_Error                                                              \
{                                                                             \
  fprintf (stderr,"Problem in %s: %s\n",__FUNCTION__, gai_strerrorA (lc_r));  \
  exit(1);                                                                    \
}

CSocketListen::CSocketListen(/*const char* ac_sHostName,*/ int ac_nPort /*, int ac_nSockType, int ac_nProtocol, int ac_nFamily*/)
  //: mc_p(nullptr)
  : port { ac_nPort }
  , local_addr { 0 }
  , socket_listen { INVALID_SOCKET }
{
  //::ZeroMemory( &local_addr, sizeof(local_addr) );

  local_addr.sin_family      = AF_INET;
  local_addr.sin_addr.s_addr = INADDR_ANY;

  local_addr.sin_port        = htons( ac_nPort );

  //struct addrinfo lv_Hints = { 0 };

  //lv_Hints.ai_family = ac_nFamily;
  //lv_Hints.ai_socktype = ac_nSockType;
  //lv_Hints.ai_protocol = ac_nProtocol;

  //std::stringstream ss;
  //ss << ac_nPort;

  //unsigned int lc_r = getaddrinfo(ac_sHostName, ss.str().c_str(), &lv_Hints, &mc_p);
  //if (lc_r != 0) {
  //  gm_Error;
  //}
  //if (mc_p->ai_next != nullptr) {
  //  fprintf(stderr, "Ambiguous in %s: more than 1 answer\n", __FUNCTION__);
  //  exit(1);
  //}
}

CSocketListen::~CSocketListen()
{
  if (socket_listen != INVALID_SOCKET) {
    closesocket(socket_listen);
  }
}

bool CSocketListen::openBindListen()
{
  socket_listen = socket(AF_INET, SOCK_STREAM ,0); // TCP socket
  if (socket_listen == INVALID_SOCKET) {
    fprintf(stderr, "socket() failed with error %d\n", WSAGetLastError());
    WSACleanup();
    return false;
  }

  // bind() associates a local address and port combination with the
  // socket just created. This is most useful when the application is a 
  // server that has a well-known port that clients know about in advance.
  //
  if (bind(socket_listen, (struct sockaddr*)&local_addr, sizeof(local_addr)) == SOCKET_ERROR) {
    fprintf(stderr, "bind() failed with error %d\n", WSAGetLastError());
    WSACleanup();
    return false;
  }

  if (listen(socket_listen, 5) == SOCKET_ERROR) {
    fprintf(stderr, "listen() failed with error %d\n", WSAGetLastError());
    WSACleanup();
    return false;
  }

  printf("Server listening on port %d ...\n", port);
  return true;
}

UXProtcol CSocketListen::accept()
{
  struct sockaddr_in from_addr;
  int from_addr_len = sizeof( from_addr ); // cannot be const
  SOCKET socket_message = ::accept( socket_listen, (struct sockaddr*)&from_addr, &from_addr_len );
  if (socket_message == INVALID_SOCKET) {
    fprintf(stderr, "accept() error %d\n", WSAGetLastError());
    WSACleanup();
    return std::make_unique< CProtocol >( INVALID_SOCKET );
  }
  else {
    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(from_addr.sin_addr), str, INET_ADDRSTRLEN);

    printf("accepted connection from %s, port %d\n", /*inet_ntoa(from_addr.sin_addr)*/ str, htons(from_addr.sin_port));
    return std::make_unique< CProtocol >( socket_message );
  }
}

